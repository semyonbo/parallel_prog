#include <iostream>
#include <vector>
#include <numeric>
#include <omp.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    const size_t vector_size = 1000000;
    vector<int> vec(vector_size);

    
    srand(time(0));
    for (size_t i = 0; i < vector_size; ++i) {
        vec[i] = rand() % 100;
    }

    double start_time, end_time;

    start_time = omp_get_wtime();
    long long sequential_sum = accumulate(vec.begin(), vec.end(), 0LL);
    end_time = omp_get_wtime();
    cout << "Sequential sum: " << sequential_sum
              << ", Time: " << (end_time - start_time) << " seconds" << endl;

    // atomic
    long long atomic_sum = 0;
    start_time = omp_get_wtime();
    #pragma omp parallel for
    for (size_t i = 0; i < vec.size(); ++i) {
        #pragma omp atomic
        atomic_sum += vec[i];
    }
    end_time = omp_get_wtime();
    cout << "Atomic sum: " << atomic_sum
              << ", Time: " << (end_time - start_time) << " seconds" << endl;

    // critical
    long long critical_sum = 0;
    start_time = omp_get_wtime();
    #pragma omp parallel for
    for (size_t i = 0; i < vec.size(); ++i) {
        #pragma omp critical
        {
            critical_sum += vec[i];
        }
    }
    end_time = omp_get_wtime();
    cout << "Critical sum: " << critical_sum
              << ", Time: " << (end_time - start_time) << " seconds" << endl;

    // lock
    long long locked_sum = 0;
    omp_lock_t lock;
    omp_init_lock(&lock);
    start_time = omp_get_wtime();
    #pragma omp parallel for
    for (size_t i = 0; i < vec.size(); ++i) {
        omp_set_lock(&lock);
        locked_sum += vec[i];
        omp_unset_lock(&lock);
    }
    end_time = omp_get_wtime();
    omp_destroy_lock(&lock);
    cout << "Locked sum: " << locked_sum
              << ", Time: " << (end_time - start_time) << " seconds" << endl;

    // reduction
    long long reduction_sum = 0;
    start_time = omp_get_wtime();
    #pragma omp parallel for reduction(+:reduction_sum)
    for (size_t i = 0; i < vec.size(); ++i) {
        reduction_sum += vec[i];
    }
    end_time = omp_get_wtime();
    cout << "Reduction sum: " << reduction_sum
              << ", Time: " << (end_time - start_time) << " seconds" << endl;

    return 0;
}
