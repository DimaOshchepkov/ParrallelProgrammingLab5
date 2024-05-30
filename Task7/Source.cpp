#include <iostream>
#include <omp.h>
#include <string>

using namespace std;

#include <omp.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <format>
#include <benchmark/benchmark.h>
#include <format>

using Matrix = std::vector<std::vector<int>>;

// Функция для инициализации матрицы размером N x N
void initializeMatrix(Matrix& matrix, int N) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

// Функция умножения матриц
void multiplyMatrices(const Matrix& A, const Matrix& B, Matrix& C, int numThreads) {
#pragma omp parallel for collapse(2) num_threads(numThreads)
    for (int i = 0; i < C.size(); ++i) {
        for (int j = 0; j < C.front().size(); ++j) {
            C[i][j] = 0;
            for (int k = 0; k < C.size(); ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

static void BM_mult(benchmark::State& state, int size, int numThreads) {
    Matrix A(size, std::vector<int>(size));
    Matrix B(size, std::vector<int>(size));
    Matrix C(size, std::vector<int>(size));

    initializeMatrix(A, size);
    initializeMatrix(B, size);


    for (auto _ : state) {
        multiplyMatrices(A, B, C, numThreads);
    }
}

int main(int argc, char** argv) {
    for (int N : {500, 1000, 2000}) {
        for (int numThread : {1, 2, 4, 8}) {
            benchmark::RegisterBenchmark(std::format("BM_tr/size{}/threads{}", N, numThread),
                BM_mult,
                N,
                numThread)
                ->Unit(benchmark::kMillisecond);
        }
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}