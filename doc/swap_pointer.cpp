#include <iostream>
#include <memory>  
#include <vector>
using namespace std;

int main() {
    unique_ptr<int> a = make_unique<int>();
    unique_ptr<int> b = make_unique<int>();

    *a = 5;   
    *b = 10;

    cout << "Before swap (unique_ptr): a = " << *a << ", b = " << *b << endl;
    cout << "Before swap (unique_ptr) addresses: a = " << &a << ", b = " << &b << endl;

    std::swap(*a, *b); 
    cout << "After swap (unique_ptr): a = " << *a << ", b = " << *b << endl;
    cout << "After swap (unique_ptr) addresses: a = " << &a << ", b = " << &b << endl;

    shared_ptr<vector<int>> vec1 = make_shared<vector<int>>(vector<int>{1, 2, 3});
    shared_ptr<vector<int>> vec2 = make_shared<vector<int>>(vector<int>{10, 20, 30});

    std::swap(*vec1, *vec2);
    for (int i = 0; i < vec1->size(); i++) cout << (*vec1)[i] << " ";
    cout << endl;

    for (int i = 0; i < vec2->size(); i++) cout << (*vec2)[i] << " ";
    cout << endl;
    
    return 0;
}
