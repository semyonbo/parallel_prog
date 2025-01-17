#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <chrono>
#include <atomic>

using namespace std;
using namespace tbb;

long long sum_of_squares_seq(int N) {
    vector<int> numbers(N);
    iota(numbers.begin(), numbers.end(), 1);
    long long sum = 0;
    for_each(execution::seq, numbers.begin(), numbers.end(), [&sum](long long num) {
        sum += num * num;
    });

    return sum;
}

long long sum_of_squares_parallel_stl(int N) {
    vector<int> numbers(N);
    iota(numbers.begin(), numbers.end(), 1);

    atomic<long long> sum(0);
    for_each(execution::par, numbers.begin(), numbers.end(), [&sum](long long num) {
        sum += num * num;  
    });

    return sum;
}


long long sum_of_squares_tbb(int N) {
    atomic<long long> sum(0);

    parallel_for(blocked_range<int>(1, N + 1), [&sum](const blocked_range<int>& r) {
        long long local_sum = 0;
        for (long long i = r.begin(); i != r.end(); ++i) {
            local_sum += i * i;
        }
        sum += local_sum;
    });

    return sum;
}

int main() {
    int N = 100000000;  
    auto start = chrono::high_resolution_clock::now();
    long long result_seq = sum_of_squares_seq(N);
    auto end = chrono::high_resolution_clock::now();
    auto duration_seq = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    start = chrono::high_resolution_clock::now();
    long long result_parallel_stl = sum_of_squares_parallel_stl(N);
    end = chrono::high_resolution_clock::now();
    auto duration_parallel_stl = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    start = chrono::high_resolution_clock::now();
    long long result_tbb = sum_of_squares_tbb(N);
    end = chrono::high_resolution_clock::now();
    auto duration_tbb = chrono::duration_cast<chrono::milliseconds>(end - start);

    string res_std;
    if (result_seq == result_parallel_stl) {
        res_std =  "Match";
    } else {
        res_std =  "Doesn't match!";
    }
    string res_tbb;
    if (result_seq == result_tbb) {
        res_tbb =  "Match";
    } else {
        res_tbb =  "Doesn't match!";
    }


    cout << "-----------Seq Result-----------" << endl;
    cout << result_seq << ", time: " << duration_seq.count() << " ms" << endl;
    cout << "-----------STD Parallel Result-----------" << endl;
    cout <<  result_parallel_stl << ", time: " << duration_parallel_stl.count() << " ms, " << res_std << endl;
    cout << "-----------TBB Parallel Result-----------" << endl;
    cout <<  result_tbb << ", time: " << duration_tbb.count() << " ms, " << res_tbb<<endl;

    return 0;
}
