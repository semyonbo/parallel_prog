#include <iostream>
#include <vector>
#include <cuda_runtime.h>

__global__ void dot_product_kernel(const float* d_a, const float* d_b, float* d_result, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    // Каждый поток обрабатывает одну пару элементов
    if (idx < n) {
        atomicAdd(d_result, d_a[idx] * d_b[idx]);  // Атомарно обновляем результат
    }
}

int main() {
    int n = 1024;  // Размер векторов
    std::vector<float> a(n, 1.0f);  // Вектор a, заполняем единицами
    std::vector<float> b(n, 2.0f);  // Вектор b, заполняем двойками

    float* d_a;
    float* d_b;
    float* d_result;
    float h_result = 0.0f;

    // Выделяем память на устройстве
    cudaMalloc(&d_a, n * sizeof(float));
    cudaMalloc(&d_b, n * sizeof(float));
    cudaMalloc(&d_result, sizeof(float));

    // Копируем данные с хоста на устройство
    cudaMemcpy(d_a, a.data(), n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b.data(), n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_result, &h_result, sizeof(float), cudaMemcpyHostToDevice);

    // Запускаем ядро CUDA с 256 потоками в блоке
    int block_size = 256;
    int num_blocks = (n + block_size - 1) / block_size;
    dot_product_kernel<<<num_blocks, block_size>>>(d_a, d_b, d_result, n);

    // Ждем завершения вычислений
    cudaDeviceSynchronize();

    // Копируем результат с устройства на хост
    cudaMemcpy(&h_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);

    // Выводим результат
    std::cout << "Скалярное произведение: " << h_result << std::endl;

    // Освобождаем память
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_result);

    return 0;
}
