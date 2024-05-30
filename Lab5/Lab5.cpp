// Lab5.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <omp.h>
#include <string>

using namespace std;

#include <omp.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

int task1() {
	system("chcp 1251>nul");

	int nthreads, tid, requested_threads;

	// Запрос количества потоков у пользователя
	cout << "Введите количество потоков: ";
	cin >> requested_threads;

	// Установка количества потоков для параллельной области
	omp_set_num_threads(requested_threads);

	// Создание параллельной области
#pragma omp parallel private(tid)
	{
		// печать номера потока
		tid = omp_get_thread_num();
		printf("Hello World from thread = %d\n", tid);
		// Печать количества потоков – только master
		if (tid == 0) {
			nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n", nthreads);
		}
	} // Завершение параллельной области
	return 0;
}


int task2() {
    system("chcp 1251>nul");

    int N;

    // Запрос количества итераций у пользователя
    cout << "Введите N: ";
    cin >> N;

    // Установка количества потоков
    omp_set_num_threads(2);

    // Создание параллельной области
#pragma omp parallel
    {
        int tid = omp_get_thread_num();

        if (tid == 0) {
            // Первый поток: вывод чисел от 1 до N
            for (int i = 1; i <= N; i++) {
                cout << "Поток 1: " << i << endl;
            }
        }
        else if (tid == 1) {
            // Второй поток: вывод слова "HELLO" N раз
            for (int i = 1; i <= N; i++) {
                cout << "Поток 2: HELLO" << endl;
            }
        }
    } // Конец параллельной области

    return 0;
}




int task3() {
    system("chcp 1251>nul");

    int N;
    std::cout << "Введите размер массивов N: ";
    std::cin >> N;

    // Инициализация массивов
    std::vector<int> A(N), B(N), C(N);

    // Заполнение массивов данными
    for (int i = 0; i < N; i++) {
        A[i] = i + 1;  // Пример: элементы от 1 до N
        B[i] = i + 2;  // Пример: элементы от 2 до N+1
    }

    // Параллельное вычисление поэлементного произведения
#pragma omp parallel for schedule(static, 4)
    for (int i = 0; i < N; i++) {
        C[i] = A[i] * B[i];
    }

    // Вывод результата
    std::cout << "Произведение массивов:" << std::endl;
    for (int i = 0; i < N; i++) {
        std::cout << C[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}

int task4a() {
    system("chcp 1251>nul");

    int N = 100;
    std::vector<int> vec(N, 0);
    for (int i = 0; i < N; i++) {
        vec[i] = i;
    }

    int max_val = std::numeric_limits<int>::min();

#pragma omp parallel for reduction(max:max_val)
    for (int i = 0; i < N; i++) {
        if (vec[i] > max_val) {
            max_val = vec[i];
        }
    }

    std::cout << "Максимальное значение: " << max_val << std::endl;
    return 0;
}

int task4b() {
    system("chcp 1251>nul");

    int N = 100;
    std::vector<int> vec(N, 0);
    for (int i = 0; i < N; i++) {
        vec[i] = i;
    }

    int max_val = std::numeric_limits<int>::min();

#pragma omp parallel for
    for (int i = 0; i < N; i++) {
#pragma omp critical
        {
            if (vec[i] > max_val) {
                max_val = vec[i];
            }
        }
    }

    std::cout << "Максимальное значение: " << max_val << std::endl;
    return 0;
}

// Работает некорректно
int task4c() {
    system("chcp 1251>nul");

    int N = 100;
    std::vector<int> vec(N, 0);
    for (int i = 0; i < N; i++) {
        vec[i] = i;
    }

    int max_val = std::numeric_limits<int>::min();

#pragma omp parallel for
    for (int i = 0; i < N; i++) {
#pragma omp atomic write
        max_val = std::max(max_val, vec[i]);
    }

    std::cout << "Максимальное значение: " << max_val << std::endl;
    return 0;
}

template <typename T>
void printArray(const std::vector<T>& arr) {
    std::cout << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i < arr.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

int task5() {
    system("chcp 1251>nul");
    int N, M, K;

    std::cout << "Введите число N: ";
    std::cin >> N;

    std::cout << "Введите число потоков для вычисления суммы (M): ";
    std::cin >> M;

    std::cout << "Введите число потоков для вычисления длины вектора (K): ";
    std::cin >> K;

    // Создаем вектор с N элементами после ввода N
    std::vector<double> vector(N);

    // Заполняем вектор случайными числами
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10.0, 10.0);

    for (int i = 0; i < N; ++i) {
        vector[i] = dis(gen);
    }

    std::cout << "Вектор ";
    printArray(vector);

    // Создаем две параллельные секции для одновременного выполнения задач
#pragma omp parallel sections
    {
        // Первая секция для вычисления суммы
#pragma omp section
        {
            double sum = 0;
#pragma omp parallel for num_threads(M) reduction(+:sum)
                for (int i = 0; i < vector.size(); i++) {
                    sum += vector[i];
                }

#pragma omp critical
            std::cout << "Сумма чисел вектора " << sum << std::endl;
        }

        // Вторая секция для вычисления длины вектора
#pragma omp section
        {
            double vector_length = 0.0;
#pragma omp parallel for num_threads(K) reduction(+:vector_length)
            for (int i = 0; i < vector.size(); i++) {
                vector_length += vector[i] * vector[i];
            }

            vector_length = std::sqrt(vector_length);
#pragma omp critical
            std::cout << "Длина вектора: " << vector_length << std::endl;
            
        }
    }

    return 0;
}




int main()
{
	task5();
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
