// Worker.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <cmath>

using namespace std;

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {

    setlocale(LC_ALL, "Russian");

    if (argc != 3) {
        cout << "--- Ошибка: нужно передать начало и конец диапазона" << endl;
        return 1;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);

    int primeCount = 0;

    // Ищем простые числа
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            primeCount++;
        }
    }

    cout << "Найдено простых чисел: " << primeCount << endl;

    Sleep(1000);
    return 0;
}

