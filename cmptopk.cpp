#include <algorithm>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <vector>
using namespace std;
using namespace std::chrono;

typedef int (*testfunc)(vector<int> &, size_t);

map<string, testfunc> testfuncmap;

#define ADDFUNC(func)                                                          \
    namespace {                                                                \
    struct addfunc##func {                                                     \
        addfunc##func() { testfuncmap[#func] = func; }                         \
    } annoy##func;                                                             \
    }

void display(const vector<int> &data, bool flag = false) {
    if (!flag)
        return;
    for (int i : data) {
        cout << i << ' ';
    }
    cout << endl;
}

int func1(vector<int> &data, size_t k) {
    cout << "std::pop_heap and std::push_heap" << endl;
    vector<int> heap;
    heap.reserve(k);
    for (int i : data) {
        if (heap.size() < k) {
            heap.push_back(i);
            push_heap(heap.begin(), heap.end());
        } else {
            if (i < heap.front()) {
                pop_heap(heap.begin(), heap.end());
                heap.back() = i;
                push_heap(heap.begin(), heap.end());
            }
        }
    }
    return heap.front();
}
ADDFUNC(func1);

int func2(vector<int> &data, size_t k) {
    cout << "replace heap top and adjust heap" << endl;
    vector<int> heap;
    heap.reserve(k);
    for (int i : data) {
        if (heap.size() < k) {
            heap.push_back(i);
            push_heap(heap.begin(), heap.end());
        } else {
            if (i < heap.front()) {
                heap.front() = i;
                bool adjust = true;
                int st = 0;
                while (adjust) {
                    int largest = st;
                    int left = (st << 1) + 1;
                    int right = (st << 1) + 2;
                    int size = heap.size();
                    if (left < size && heap[left] > heap[largest])
                        largest = left;
                    if (right < size && heap[right] > heap[largest])
                        largest = right;
                    if (largest != st) {
                        std::swap(heap[st], heap[largest]);
                        st = largest;
                    } else {
                        adjust = false;
                    }
                }
            }
        }
    }
    return heap.front();
}
ADDFUNC(func2);

int func3(vector<int> &data, size_t k) {
    cout << "std::partial_sort" << endl;
    partial_sort(data.begin(), data.begin() + k, data.end());
    display(data);
    return data[k - 1];
}
ADDFUNC(func3);

int func4(vector<int> &data, size_t k) {
    cout << "std::nth_element" << endl;
    nth_element(data.begin(), data.begin() + k - 1, data.end());
    display(data);
    return data[k - 1];
}
ADDFUNC(func4);

int func5(vector<int> &data, size_t k) {
    cout << "my partition function" << endl;

    int left = 0, right = data.size() - 1;
    while (true) {
        int l = left, r = right;
        int pivot = data[r];
        while (l < r) {
            while (l < r && data[l] <= pivot)
                ++l;
            data[r] = data[l];
            while (l < r && data[r] >= pivot)
                --r;
            data[l] = data[r];
        }
        data[l] = pivot;
        if (l < k - 1) {
            left = l + 1;
        } else if (l > k - 1) {
            right = l - 1;
        } else {
            return data[l];
        }
    }
}
ADDFUNC(func5);

int func6(vector<int> &data, size_t k) {
    cout << "std::priority_queue" << endl;
    priority_queue<int, vector<int>, less<int>> prique;
    for (int i : data) {
        if (prique.size() < k) {
            prique.push(i);
        } else if (prique.top() > i) {
            prique.pop();
            prique.push(i);
        }
    }
    return prique.top();
}
ADDFUNC(func6);

int main() {
    const int N = 10000000;
    const int k = 1000000;
    const int expected_result = 31415926;
    mt19937 gen(time(0));
    uniform_int_distribution<> dis_left(expected_result - N,
                                        expected_result - 1);
    uniform_int_distribution<> dis_right(expected_result + 1,
                                         expected_result + N);
    vector<int> data;
    data.reserve(N);
    for (int i = 0; i < k - 1; i++) {
        data.push_back(dis_left(gen));
    }
    // make sure the correct result has only one value.
    data.push_back(expected_result);
    for (int i = k; i < N; i++) {
        data.push_back(dis_right(gen));
    }
    display(data);

    for (auto it : testfuncmap) {
        auto tic1 = high_resolution_clock::now();
        auto data_copy = data;
        auto toc1 = high_resolution_clock::now();
        auto t1 = duration_cast<milliseconds>(toc1 - tic1).count();
        cout << it.first << ": ";
        auto tic2 = high_resolution_clock::now();
        int result = it.second(data_copy, k);
        auto toc2 = high_resolution_clock::now();
        auto t2 = duration_cast<milliseconds>(toc2 - tic2).count();
        cout << "result = " << result << " "
             << (result == expected_result ? "CORRECT" : "WRONG") << endl;
        cout << "copy data time used = " << t1 << "ms" << endl;
        cout << "pure compute time used = " << t2 << "ms" << endl << endl;
    }
}
