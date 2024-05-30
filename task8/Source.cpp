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


static void BM_aaaa(benchmark::State& state, int size, int numThreads) {
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    initializeMatrix(matrix, size);

    for (auto _ : state) {
        int maxMin = std::numeric_limits<int>::min();

        std::vector<int> mins(size);
#pragma omp parallel for num_threads(numThreads)
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                mins[i] = std::min(mins[i], matrix[i][j]);
            }
        }

        int maxOfAllMins = mins[0];
        for (int i = 1; i < size; ++i) {
            maxOfAllMins = std::max(maxOfAllMins, mins[i]);
        }
        benchmark::DoNotOptimize(maxOfAllMins);
    }
}

int main(int argc, char** argv) {
    for (int N : {5000, 10000, 20000}) {
        for (int numThread : {1, 2, 4, 8}) {
            benchmark::RegisterBenchmark(std::format("BM_tr/size{}/threads{}", N, numThread),
                BM_aaaa,
                N,
                numThread)
                ->Unit(benchmark::kMillisecond);
        }
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}