
/*
 * Problem: Implement Stack using Queues (LeetCode #225)
 * Description: Implement a last-in-first-out (LIFO) stack using only two queues.
 * Solution: Using Queue to simulate stack operations.
 */

 #include "ds.hpp"
 #include <iostream>
 
 class MyStack {
 private:
     Queue<int> q1;
     Queue<int> q2;
     
 public:
     void push(int x) {
         // Push to q2 first
         q2.enqueue(x);
         
         // Move all elements from q1 to q2
         while (!q1.empty()) {
             q2.enqueue(q1.dequeue());
         }
         
         // Swap q1 and q2
         Queue<int> temp = q1;
         q1 = q2;
         q2 = temp;
     }
     
     int pop() {
         return q1.dequeue();
     }
     
     int top() {
         return q1.front();
     }
     
     bool empty() {
         return q1.empty();
     }
 };
 
 int main() {
     MyStack stack;
     stack.push(1);
     stack.push(2);
     stack.push(3);
     
     std::cout << "Top: " << stack.top() << std::endl; // 3
     std::cout << "Pop: " << stack.pop() << std::endl; // 3
     std::cout << "Top: " << stack.top() << std::endl; // 2
     
     return 0;
 }