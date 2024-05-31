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


int largestPrimeDivisor(int num) {
    if (num <= 1) {
        return 1; // Обработка случая, когда число меньше или равно 1
    }

    int largestPrime = 1;
    for (int i = 2; i * i <= num; ++i) {
        while (num % i == 0) {
            largestPrime = i;
            num /= i;
        }
    }
    if (num > 1) {
        largestPrime = num;
    }
    return largestPrime;
}

vector<int> tr(vector<int> numbers, int N = 1) {
#pragma omp parallel for num_threads(N)
    for (int i = 0; i < numbers.size(); ++i) {
        numbers[i] = largestPrimeDivisor(numbers[i]);
    }
    return numbers;
}

int task6() {
    int N;

    std::cout << "Введите размер вектора (N): ";
    std::cin >> N;

    // Создаем вектор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100000, 1'000'000);
    std::vector<int> numbers(N);
    std::generate(numbers.begin(), numbers.end(), [&]() { return dist(gen); });

    // Заменяем каждый элемент на его наибольший простой делитель
    tr(numbers);

    // Выводим результаты (для проверки)
    std::cout << "Результат:" << std::endl;
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}

static void BM_tr(benchmark::State& state, int size, int numThreads) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100000, 1000000);
    std::vector<int> numbers(size);
    std::generate(numbers.begin(), numbers.end(), [&]() { return dist(gen); });

    for (auto _ : state) {
        auto res = tr(numbers, numThreads);
        benchmark::DoNotOptimize(res);
    }
}


int main(int argc, char** argv) {
    for (int N : {1e6, 2e6, 1e7}) {
        for (int numThread : {1, 2, 4, 8}) {
            benchmark::RegisterBenchmark(std::format("BM_tr/size{}/threads{}", N, numThread),
                BM_tr,
                N,
                numThread)
                ->Unit(benchmark::kMillisecond);
        }
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}