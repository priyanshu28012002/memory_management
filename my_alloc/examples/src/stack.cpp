
/*
 * Problem: Valid Parentheses (LeetCode #20)
 * Description: Given a string containing just '(', ')', '{', '}', '[' and ']',
 *              determine if the input string is valid.
 * Solution: Using Stack to track opening brackets.
 */

 #include "ds.hpp"
 #include <iostream>
 
 bool isValid(const std::string& s) {
     Stack<char> stack;
     HashTable bracketMap;
     
     // Create mapping of closing to opening brackets
     bracketMap.insert(")", '(');
     bracketMap.insert("}", '{');
     bracketMap.insert("]", '[');
     
     for (char c : s) {
         int* mapped = bracketMap.get(&c);
         if (mapped) {
             // Closing bracket
             if (stack.empty()) {
                 return false;
             }
             
             char top = stack.pop();
             if (top != *mapped) {
                 return false;
             }
         } else {
             // Opening bracket
             stack.push(c);
         }
     }
     
     return stack.empty();
 }
 
 int main() {
     std::string s = "()[]{";
     bool valid = isValid(s);
     
     std::cout << "The string \"" << s << "\" is " 
               << (valid ? "valid" : "invalid") << std::endl;
     return 0;
 }

