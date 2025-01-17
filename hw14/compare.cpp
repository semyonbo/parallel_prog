#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>
#include <omp.h>
#include <functional>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <iomanip>  // Для setw, setprecision

using namespace std;

double sum_no_parallel(const vector<double>& arr) {
    return accumulate(arr.begin(), arr.end(), 0.0);
}

double sum_sse(const vector<double>& arr) {
    size_t size = arr.size();
    __m128d sum = _mm_setzero_pd();
    
    size_t i = 0;
    for (; i + 1 < size; i += 2) {
        __m128d val = _mm_loadu_pd(&arr[i]);
        sum = _mm_add_pd(sum, val);
    }

    double result[2];
    _mm_storeu_pd(result, sum);
    double total_sum = result[0] + result[1];
    
    for (; i < size; ++i) {
        total_sum += arr[i];
    }
    
    return total_sum;
}

double sum_combined(const vector<double>& arr, size_t p) {
    size_t n = arr.size();
    size_t portion_size = n / p;
    vector<double> partial_sums(p, 0.0);

    #pragma omp parallel for
    for (size_t i = 0; i < p; ++i) {
        size_t start = i * portion_size;
        size_t end = (i + 1) * portion_size;
        partial_sums[i] = accumulate(arr.begin() + start, arr.begin() + end, 0.0);
    }

    while (p > 1) {
        #pragma omp parallel for
        for (size_t i = 0; i < p / 2; ++i) {
            partial_sums[i] += partial_sums[i + p / 2];
        }
        p /= 2;
    }

    return partial_sums[0];
}


double sum_openmp_combined(const vector<double>& arr) {
    size_t n = arr.size();
    double result = 0.0;

    #pragma omp parallel for reduction(+:result)
    for (size_t i = 0; i < n; ++i) {
        result += arr[i];
    }

    return result;
}


double sum_openmp_sections(const vector<double>& arr) {
    size_t n = arr.size();
    double result = 0.0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            for (size_t i = 0; i < n / 2; ++i) {
                result += arr[i];
            }
        }
        
        #pragma omp section
        {
            for (size_t i = n / 2; i < n; ++i) {
                result += arr[i];
            }
        }
    }

    return result;
}


template <typename Func>
double measure_time(Func sum_func, const vector<double>& arr) {
    auto start = chrono::high_resolution_clock::now();
    double result = sum_func(arr);
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duration = end - start;
    return duration.count() *1e6;  // Возвращаем время в микросекундах
}

int main() {
    vector<size_t> sizes = {8192, 16384, 32768};
    size_t num_threads = 8;  // Количество потоков (p)

    cout << left << setw(10) << "Size"
         << setw(20) << "No Parallel (us)"
         << setw(20) << "SSE (us)"
         << setw(20) << "Combined Method (us)"
         << setw(20) << "OpenMP Combined (us)"
         << setw(20) << "OpenMP Sections (us)" << endl;

    cout << "-----------------------------------------------------------------------------------------------\n";
    
    for (size_t max_size : sizes) {
        vector<double> arr(max_size, 1.0);
    
        double time_no_parallel = measure_time(sum_no_parallel, arr);
        double time_sse = measure_time(sum_sse, arr);
        double time_combined = measure_time([=](const std::vector<double>& arr) { return sum_combined(arr, num_threads); }, arr);
        double time_openmp_combined = measure_time(sum_openmp_combined, arr);
        double time_openmp_sections = measure_time(sum_openmp_sections, arr);

        double speedup_sse = time_no_parallel / time_sse;
        double speedup_combined = time_no_parallel / time_combined;
        double speedup_openmp_combined = time_no_parallel / time_openmp_combined;
        double speedup_openmp_sections = time_no_parallel / time_openmp_sections;
        
        // Выводим результаты
        cout << left << setw(10) << max_size
             << setw(20) << fixed << setprecision(2) << time_no_parallel
             << setw(20) << fixed << setprecision(2) << time_sse
             << setw(20) << fixed << setprecision(2) << time_combined
             << setw(20) << fixed << setprecision(2) << time_openmp_combined
             << setw(20) << fixed << setprecision(2) << time_openmp_sections << endl;

        cout << left << setw(10) << max_size
             << setw(20) << "Speedup:"
             << setw(20) << fixed << setprecision(2) << speedup_sse
             << setw(20) << fixed << setprecision(2) << speedup_combined
             << setw(20) << fixed << setprecision(2) << speedup_openmp_combined
             << setw(20) << fixed << setprecision(2) << speedup_openmp_sections << endl;
    }

    return 0;
}
