#include <iostream>
#include <cuda_runtime.h>

__device__ bool check_condition(int row, int col) {
    // Пример условной функции
    return (row + col) % 2 == 0;  // Условие: если сумма индексов чётная
}

__global__ void count_threads(int* res, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (row < rows && col < cols) {
        if (check_condition(row, col)) {
            atomicAdd(res, 1);  // Атомарное увеличение глобальной переменной
        }
    }
}

int main() {
    const int rows = 1024;  // Размер сетки
    const int cols = 1024;
    
    int* d_res;
    int h_res = 0;

    // Выделяем память на устройстве для переменной res
    cudaMalloc(&d_res, sizeof(int));
    cudaMemcpy(d_res, &h_res, sizeof(int), cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);  // Размер блока
    dim3 numBlocks((cols + 15) / 16, (rows + 15) / 16);  // Количество блоков

    // Запускаем ядро
    count_threads<<<numBlocks, threadsPerBlock>>>(d_res, rows, cols);
    cudaDeviceSynchronize();

    // Копируем результат обратно на хост
    cudaMemcpy(&h_res, d_res, sizeof(int), cudaMemcpyDeviceToHost);

    // Выводим результат
    std::cout << "Количество нитей, удовлетворяющих условию: " << h_res << std::endl;

    // Освобождаем память
    cudaFree(d_res);

    return 0;
}
