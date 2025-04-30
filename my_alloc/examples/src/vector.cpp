/*
 * Problem: Two Sum (LeetCode #1)
 * Description: Given an array of integers nums and an integer target, 
 *              return indices of the two numbers such that they add up to target.
 * Solution: Using Vector to store and search for complement values.
 */

 #include "ds.hpp"
 #include <iostream>
 
 Vector<int> twoSum(Vector<int>& nums, int target) {
     Vector<int> result;
     
     for (int i = 0; i < nums.size(); i++) {
         for (int j = i + 1; j < nums.size(); j++) {
             if (nums[i] + nums[j] == target) {
                 result.push_back(i);
                 result.push_back(j);
                 return result;
             }
         }
     }
     
     return result; // Return empty vector if no solution found
 }
 
 int main() {
     Vector<int> nums;
     nums.push_back(2);
     nums.push_back(7);
     nums.push_back(11);
     nums.push_back(15);
     
     Vector<int> result = twoSum(nums, 9);
     
     std::cout << "Indices: [" << result[0] << ", " 
               << result[1] << "]" << std::endl;
     
     return 0;
 }

