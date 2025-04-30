/*
 * Problem: First Unique Character in a String (LeetCode #387)
 * Description: Given a string, find the first non-repeating character 
 *              and return its index. If it doesn't exist, return -1.
 * Solution: Using HashTable to count character frequencies.
 */

 #include "ds.hpp"
 #include <string>
 #include <iostream>
 
 int firstUniqChar(const std::string& s) {
     HashTable charCount;
     
     // Count character frequencies
     for (char c : s) {
         int* count = charCount.get(&c);
         if (count) {
             charCount.insert(&c, *count + 1);
         } else {
             charCount.insert(&c, 1);
         }
     }
     
     // Find first unique character
     for (int i = 0; i < s.size(); i++) {
         int* count = charCount.get(&s[i]);
         if (count && *count == 1) {
             return i;
         }
     }
     
     return -1;
 }
 
 int main() {
     std::string s = "kals";
     int index = firstUniqChar(s);
     
     std::cout << "First unique character in \"" << s << "\" is at index: " 
               << index << std::endl;
     return 0;
 }

