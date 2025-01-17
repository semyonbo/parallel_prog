#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <chrono>

using namespace std;
using namespace tbb;

long long sum_of_squares_seq(const vector<int>& numbers) {
    long long sum = 0;
    for (int num : numbers) {
        sum += num * num;
    }
    return sum;
}

long long sum_of_squares_parallel_stl(const vector<int>& numbers) {
    return transform_reduce(execution::par, numbers.begin(), numbers.end(), 0LL, plus<long long>(), [](int num) {
        return num * num;
    });
}

long long sum_of_squares_tbb(const vector<int>& numbers) {
    return parallel_reduce(blocked_range<size_t>(0, numbers.size()), 0LL,
        [&](const blocked_range<size_t>& r, long long init) -> long long {
            for (size_t i = r.begin(); i != r.end(); ++i) {
                init += numbers[i] * numbers[i];
            }
            return init;
        },
        plus<long long>()
    );
}

int main() {
    vector<int> numbers(100000000); 
    iota(numbers.begin(), numbers.end(), 1);

    auto start = chrono::high_resolution_clock::now();
    long long result_seq = sum_of_squares_seq(numbers);
    auto end = chrono::high_resolution_clock::now();
    auto duration_seq = chrono::duration_cast<chrono::milliseconds>(end - start);

    start = chrono::high_resolution_clock::now();
    long long result_parallel_stl = sum_of_squares_parallel_stl(numbers);
    end = chrono::high_resolution_clock::now();
    auto duration_parallel_stl = chrono::duration_cast<chrono::milliseconds>(end - start);

    start = chrono::high_resolution_clock::now();
    long long result_tbb = sum_of_squares_tbb(numbers);
    end = chrono::high_resolution_clock::now();
    auto duration_tbb = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "seq: " << result_seq << ", time: " << duration_seq.count() << " ms" << endl;
    cout << "std par: " << result_parallel_stl << ", time: " << duration_parallel_stl.count() << " ms" << endl;
    cout << "tbb par: " << result_tbb << ", time: " << duration_tbb.count() << " ms" << endl;

    if (result_seq == result_parallel_stl && result_seq == result_tbb) {
        cout << "Match." << endl;
    } else {
        cout << "Doesn't match!" << endl;
    }

    return 0;
}
