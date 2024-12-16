#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>
#include <cstdlib>
#include <ctime>

using namespace std;

pair<int, int> find_min_max(const vector<int>& vec) {
    int min_val = numeric_limits<int>::max();
    int max_val = numeric_limits<int>::min();

    #pragma omp parallel shared(vec, min_val, max_val)
    {
        int local_min = numeric_limits<int>::max();
        int local_max = numeric_limits<int>::min();

        // Параллельный цикл с использованием чанков
        #pragma omp for schedule(static, 10)
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] < local_min) local_min = vec[i];
            if (vec[i] > local_max) local_max = vec[i];
        }

        // Сравнение локальных результатов в критической секции
        #pragma omp critical
        {
            if (local_min < min_val) min_val = local_min;
            if (local_max > max_val) max_val = local_max;
        }
    }

    return pair<int, int> {min_val, max_val};
}

int main() {

    srand(time(0));

    size_t vector_size = 1000;
    std::vector<int> vec(vector_size);
    for (size_t i = 0; i < vector_size; ++i) {
        vec[i] = std::rand() % 2001 - 1000; // Диапазон от -1000 до 1000
    }

    
    pair<int, int> min_max = find_min_max(vec);

    std::cout << "Min value: " << min_max.first << std::endl;
    std::cout << "Max value: " << min_max.second << std::endl;

    return 0;
}
