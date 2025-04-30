/*  Reserch

mark and sweep method
referance counter
https://github.com/atlaxon/garbage-collector.git
https://youtu.be/4qLf0FJMyf0?si=kKdgKBEu5sZ9RJB6

*/


/*


Alright, let's delve deeper into the Mark and Sweep garbage collection algorithm.

**Mark and Sweep: A Deeper Dive**

The Mark and Sweep algorithm is a classic tracing garbage collection technique. It operates in two distinct phases:

1.  **Mark Phase:**
    * The goal of this phase is to identify all objects that are still "alive" or reachable from the program's root set.
    * The algorithm starts by identifying the root set. This set typically includes:
        * Global variables
        * Local variables on the stack
        * CPU registers
    * From each root object, the algorithm recursively traverses the object graph, marking each reachable object.
    * "Marking" usually involves setting a bit in the object's header.
    * This traversal can be implemented using depth-first search (DFS) or breadth-first search (BFS).
    * If an object is reachable from any root, it is considered "live."
2.  **Sweep Phase:**
    * After the mark phase, the sweep phase takes over.
    * It iterates through the entire heap, examining each object.
    * If an object's mark bit is set, it means the object is live, and the mark bit is reset for the next garbage collection cycle.
    * If an object's mark bit is not set, it means the object is unreachable and considered "garbage."
    * The garbage object's memory is then reclaimed and made available for future allocations.

**Detailed Breakdown of the Mark Phase**

* **Traversal:**
    * The traversal process is crucial. It must visit every object that is reachable from the root set.
    * Depth-first search is a common choice, as it is relatively simple to implement using a stack.
    * Breadth-first search can also be used, but it might require more memory.
* **Object Marking:**
    * The mark bit is typically a single bit within the object's header.
    * Using a bit is efficient, as it minimizes the overhead associated with marking.
    * Sometimes instead of a single bit, a more complex marking scheme is used, that allows for tracking different states of the object.
* **Handling Circular References:**
    * Mark and sweep handles circular references correctly.
    * Even if objects form a cycle, they will be marked if they are reachable from a root.
    * If they are not reachable, they will be correctly identified as garbage.

**Detailed Breakdown of the Sweep Phase**

* **Heap Iteration:**
    * The sweep phase must iterate through the entire heap, which can be time-consuming.
    * This iteration is often the most time-consuming part of the garbage collection process.
* **Memory Reclamation:**
    * Reclaiming memory involves freeing the memory occupied by garbage objects.
    * This can be done by adding the freed memory to a free list or by using other memory management techniques.
* **Fragmentation:**
    * Mark and sweep can lead to memory fragmentation.
    * After repeated garbage collection cycles, the heap may contain many small, scattered free blocks.
    * This can make it difficult to allocate large objects.

**Advantages of Mark and Sweep:**

* Handles circular references.
* Relatively simple to implement.

**Disadvantages of Mark and Sweep:**

* Can introduce pauses in program execution.
* Can lead to memory fragmentation.
* The entire heap must be traversed.

**Improvements and Variations:**

* **Tri-color marking:** This helps with concurrent garbage collection, and helps to reduce pauses.
* **Compaction:** After sweeping, the live objects can be moved to contiguous memory locations, reducing fragmentation.
* **Generational garbage collection:** Divides the heap into generations, collecting younger objects more frequently.

**In summary:**

Mark and sweep is a foundational garbage collection algorithm. Understanding its principles is essential for comprehending more advanced garbage collection techniques. While it has limitations, it provides a solid base for memory management.


*/


#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm> // Add this for std::remove

class GarbageCollector;

// Base class for all collectable objects
class GCObject {
public:
    bool marked = false;
    virtual ~GCObject() = default;
    virtual void mark() = 0;
    
protected:
    friend class GarbageCollector;
    virtual void addReferences(std::vector<GCObject*>& refs) = 0;
};

// Garbage Collector class
class GarbageCollector {
private:
    std::unordered_set<GCObject*> heap;
    std::vector<GCObject*> roots;

public:
    void addRoot(GCObject* root) {
        roots.push_back(root);
    }

    void removeRoot(GCObject* root) {
        // Use erase-remove idiom with fully qualified std::remove
        roots.erase(std::remove(roots.begin(), roots.end(), root), roots.end());
    }

    void allocate(GCObject* obj) {
        heap.insert(obj);
    }

    void collect() {
        // Mark phase
        mark();

        // Sweep phase
        sweep();
    }

    // Add public method to get heap size
    size_t heapSize() const {
        return heap.size();
    }

private:
    void mark() {
        // Mark all roots as reachable
        for (GCObject* root : roots) {
            root->mark();
        }
    }

    void sweep() {
        auto it = heap.begin();
        while (it != heap.end()) {
            GCObject* obj = *it;
            if (!obj->marked) {
                // Object wasn't reached - delete it
                it = heap.erase(it);
                delete obj;
            } else {
                // Reset marked flag for next GC cycle
                obj->marked = false;
                ++it;
            }
        }
    }
};

// Example of a collectable object
class MyObject : public GCObject {
private:
    std::vector<GCObject*> references;

public:
    void addReference(GCObject* obj) {
        references.push_back(obj);
    }

    void mark() override {
        if (marked) return; // Prevent infinite recursion
        marked = true;
        for (GCObject* ref : references) {
            ref->mark();
        }
    }

protected:
    void addReferences(std::vector<GCObject*>& refs) override {
        for (GCObject* ref : references) {
            refs.push_back(ref);
        }
    }
};

// Demonstration
int main() {
    GarbageCollector gc;

    // Create some objects
    MyObject* root1 = new MyObject();
    MyObject* child1 = new MyObject();
    MyObject* child2 = new MyObject();
    MyObject* orphan = new MyObject();

    // Set up object graph
    root1->addReference(child1);
    child1->addReference(child2);
    // 'orphan' is not referenced by anyone

    // Register with garbage collector
    gc.allocate(root1);
    gc.allocate(child1);
    gc.allocate(child2);
    gc.allocate(orphan);

    // Add roots
    gc.addRoot(root1);

    std::cout << "Before GC: " << gc.heapSize() << " objects in heap\n";

    // Run garbage collection
    gc.collect();

    std::cout << "After GC: " << gc.heapSize() << " objects in heap\n";
    // Should be 3 objects left (root1, child1, child2), orphan was collected

    // Clean up remaining objects (in a real system, this would be done automatically)
    gc.removeRoot(root1); // Remove roots before cleanup
    gc.collect(); // This will clean up everything

    return 0;
}

