#!/usr/bin/python3

# Intended to be run on the output of "operf -gl" (or "operf -g")
# and then valgrind run on its output file.
#
# E.g.
#
# operf -gl ./a.out
# op2kcg -o a.out.callgrind
# kcachegrind a.out.callgrind &
#
# The opreport command must be available
#
# See https://valgrind.org/docs/manual/cl-format.html for output format
#
# To use with call counts from gprof, in a separate directory
# compile with "-pg", run, and run "gprof -q -b > gprof.out"
# Then copy resulting gprof.out to directory of operf run.
#
# To use with call counts from valgrind, run
# valgrind --tool=callgrind --compress-strings=no --separate-recs=1
#          --callgrind-out-file=callgrind.out ./a.out
#
# Version 0.1, Jan 2020
# Version 0.15, Feb 2020, support for recursive functions
#
# Perhaps to be found at http://www.mjr19.org.uk/sw/op2kcg
#  where there may also be further documentation / examples
#
# (c) 2020 MJ Rutter, may be distributed under the GPL v3
#  https://www.gnu.org/licenses/gpl-3.0.en.html
# The author accepts no responsibility for correctness, fitness for
# purpose, bugs, etc. The user is responsible for determining whether
# this script meets any given need.

import subprocess as s
import sys
import argparse


parser=argparse.ArgumentParser(description="Converts profile data "
                               "from a run of 'operf -gl' to "
                               "kcachegrind/callgrind format.",
                               epilog='(c) 2020 MJ Rutter')
parser.add_argument('-v', '--version', action='store_true',dest='version',
                    help='show version')
parser.add_argument('-o', '--output',dest='outname',default='-',
                    help='output file (default: stdout)')
parser.add_argument('-pg', action='store_true',dest='pg',
                    help='additionally process gprof.out file')
parser.add_argument('-cg', action='store_true',dest='cg',
                    help='additionally process callgrind.out file')
args=parser.parse_args()
outname=args.outname

if (args.version):
    print("op2kcg version 0.1\n")
    exit(0)

# First read profile

prof={}

try:
    opr=s.Popen(["opreport","-gdf"],stdout=s.PIPE)
except:
    print("Failed to run opreport. Quitting.",file=sys.stderr)
    exit(1)
    
with opr.stdout as f:
    lines=f.readlines()

# Skip header

process=False

for line in lines:
    line=line.decode('utf-8').rstrip()
    if (line.startswith("Counted")):
        (dummy,event,dummy2)=line.split(maxsplit=2)
    if ("vma" in line):
        process=True
        continue
    if not process: continue
    if (line[0] in "0123456789abcdef"):
        (vas,count,dummy,source,obj,func)=line.split(maxsplit=5)
        if (source != "(no"):
            (filename,lineno)=source.split(':')
        else:
            (junk,obj,func)=func.split(maxsplit=2)
            lineno=0
    else:
        (vas,count,dummy,source)=line.split(maxsplit=3)
        if (":" in source):
            (filename,lineno)=source.split(':')
        else:
            filename='/unknown'
            lineno='0'
        
        p="0x%s %s %s\n" % (vas,lineno,count)
        if (lineno):
            if func in prof:
                prof[func]=prof[func]+p
            else:
                prof[func]=p

if (outname=='-'):
    out=sys.stdout
else:
    out=open(outname,'w')

out.write("# callgrind format\n")
out.write("version: 1\n")
out.write("creator: op2kcd\n")
# positions must precede events
out.write("positions: instr line\n")
out.write("events: %s\n" % event)

# Should we read a gprof.out file?

gprof={}
if (args.pg):
    try:
        with open("gprof.out") as f:
            lines=f.readlines()
    except:
        print("Failed to read gprof.out, continuing anyway",file=sys.stderr)
        lines=['']
        
    in_func=False
    for line in lines:
        line=line.rstrip()
        if (line==''): continue
        if (line[0]=='['):
            try:
                (junk,junk,junk,junk,junk,func,junk)=line.split(maxsplit=6)
            except:
                (junk,junk,junk,junk,func,junk)=line.split(maxsplit=5)
            in_func=True
            gprof[func]={}
            continue
        if (line.startswith("-------------")):
            in_func=False
            continue
        if in_func:
            fields=len(line.split())
            if (fields>=5):  # standard, with ref
                (junk,junk,counts,name,junk)=line.split(maxsplit=4)
                (count,junk)=counts.split('/')
            elif (fields==4): # standard, no ref
                (junk,junk,counts,name)=line.split(maxsplit=3)
                (count,junk)=counts.split('/')
            elif (fields==3):  # recursive
                (count,name,junk)=line.split(maxsplit=2)
            elif (fields==2):  # recursive, no ref (impossible?)
                (count,name)=line.split(maxsplit=1)
            gprof[func][name]=count
elif (args.cg):
    try:
        with open("callgrind.out") as f:
            lines=f.readlines()
    except:
        print("Failed to read callgrind.out, continuing anyway",file=sys.stderr)
        lines=['']

    func="(impossible)"
    name="(impossible)"
    for line in lines:
        line=line.rstrip()
        if (line==''): continue

        if (line.startswith("fn=")):
            func=line[3:]
            gprof[func]={}
        elif (line.startswith("cfn=")):
            name=line[4:]
        elif (line.startswith("calls=")):
            line=line[6:]
            (count,junk)=line.split()
            if name in gprof[func]:
                gprof[func][name]=str(float(gprof[func][name])+float(count))
            else:
                gprof[func][name]=count
        
# Then read call tree

opr=s.Popen(["opreport","-gcwf"],stdout=s.PIPE,stderr=s.DEVNULL)
with opr.stdout as f:
    lines=f.readlines()

process=False
in_func=False
child_found=False

for line in lines:
    line=line.decode('utf-8').rstrip()
    if not process:
        if (line.startswith("-------------")): process=True
        continue
    
    if line[0] in "0123456789abcdef":
        (vas,count,dummy,source,obj,func)=line.split(maxsplit=5)
        if (source != "(no"):
            (filename,lineno)=source.split(':')
        else:
            (junk,obj,func)=func.split(maxsplit=2)
            lineno=0
            filename="(none)"
        out.write("fl=%s\n" % filename)
        out.write("ob=%s\n" % obj)
        out.write("fn=%s\n" % func)
        if func in prof:
            out.write(prof[func])
        else:
            out.write("0x%s %s %s\n" % (vas,lineno,count))
        caller_line=lineno
        caller_vas=vas
        caller=func
        in_func=True
    else:
        if (in_func):
            if (line.startswith("-------------")):
                in_func=False
                out.write("\n")
                continue
            if "[self]" in line: continue
            child_found=True
            (vas,count,dummy,source,obj,func)=line.split(maxsplit=5)
            lineno=0
            if (":" in source):
                (filename,lineno)=source.split(':')
            else:
                try:
                    (junk,obj,func)=func.split(maxsplit=2)
                except:
                    pass
            out.write("cfi=%s\n" % filename)
            out.write("cob=%s\n" % obj)
            out.write("cfn=%s\n" % func)
            try:
                c=gprof[caller][func]
            except:
                c="1"
            out.write("calls=%s %s\n" % (c, lineno))
            out.write("0x%s %s %s\n" % (caller_vas,caller_line,count))
            caller_vas=vas
        
if not child_found:
    print("\nNo children in call tree. Was operf run with -g?",file=sys.stderr)
