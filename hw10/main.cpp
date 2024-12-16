#include <iostream>
#include <fstream>
#include <omp.h>

#define N 10 // Number of vectors
#define M 5 // Size of each vector

using namespace std;

void read_vectors(double vectors[N][M]) {
    ifstream file("vectors.txt");
    if (!file) {
        cerr << "Error opening file." << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            file >> vectors[i][j];
        }
    }

    file.close();
}

double dot_product(double *a, double *b, int m) {
    double result = 0.0;

    #pragma omp parallel for reduction(+:result)
    for (int i = 0; i < m; i++) {
        result += a[i] * b[i];
    }

    return result;
}

int main() {
    double vectors[N][M];
    double total_dot_product = 0.0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            read_vectors(vectors);
            cout << "Vectors have been read." << endl;
        }
    }

    #pragma omp parallel for reduction(+:total_dot_product)
    for (int i = 0; i < N; i++) {
        total_dot_product += dot_product(vectors[i], vectors[i], M);
    }

    
    cout << "dot product: " << total_dot_product << endl;

    return 0;
}