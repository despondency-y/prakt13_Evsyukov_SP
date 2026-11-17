// prakt13_Evsyukov_SP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

const int MAX_PROCESSES = 5;

struct RaceParticipant {
    int id;
    DWORD pid;
    HANDLE hProcess;
    HANDLE hThread; 
    int priorityClass;
    bool finished;
    double finishTime;
};


RaceParticipant racers[MAX_PROCESSES];
int racerCount = 0;
DWORD startTime;


string GetPriorityName(int priority) {
    if (priority == IDLE_PRIORITY_CLASS) return "IDLE";
    if (priority == BELOW_NORMAL_PRIORITY_CLASS) return "BELOW_NORMAL";
    if (priority == NORMAL_PRIORITY_CLASS) return "NORMAL";
    if (priority == ABOVE_NORMAL_PRIORITY_CLASS) return "ABOVE_NORMAL";
    if (priority == HIGH_PRIORITY_CLASS) return "HIGH";
    return;
}


int ChoosePriority() {
    int choice;
    cout << "1. IDLE\n2. BELOW_NORMAL\n3. NORMAL\n4. ABOVE_NORMAL\n5. HIGH" << endl;
    cout << "Выберите приоритет: ";
    cin >> choice;

    if (choice == 1) return IDLE_PRIORITY_CLASS;
    if (choice == 2) return BELOW_NORMAL_PRIORITY_CLASS;
    if (choice == 3) return NORMAL_PRIORITY_CLASS;
    if (choice == 4) return ABOVE_NORMAL_PRIORITY_CLASS;
    if (choice == 5) return HIGH_PRIORITY_CLASS;
    return NORMAL_PRIORITY_CLASS;
}


void StartProcess(int id, int startRange, int endRange) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));


    wchar_t cmdLine[256];
    swprintf_s(cmdLine, L"C:\\Users\\st310-04\\Desktop\\Worker\\x64\\Debug\\Worker.exe %d %d", startRange, endRange);


    if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cout << "--- Ошибка создания процесса " << id << ". Код: " << GetLastError() << endl;
        return;
    }


    racers[id - 1].id = id;
    racers[id - 1].pid = pi.dwProcessId;
    racers[id - 1].hProcess = pi.hProcess;
    racers[id - 1].hThread = pi.hThread; 
    racers[id - 1].finished = false;
}

int main() {
    setlocale(LC_ALL, "rus");

    int startRange, endRange;
    cout << "Введите начало диапазона простых чисел: ";
    cin >> startRange;
    cout << "Введите конец диапазона: ";
    cin >> endRange;

    cout << "Количество процессов (от 2 до 5): ";
    cin >> racerCount;


    cout << "\n\t--- НАСТРОЙКА ПРИОРИТЕТОВ ---" << endl;
    for (int i = 0; i < racerCount; i++) {
        cout << "\nУчастник №" << (i + 1) << ":" << endl;
        racers[i].priorityClass = ChoosePriority();
    }


    cout << "\nСоздание процессов..." << endl;
    for (int i = 0; i < racerCount; i++) {
        StartProcess(i + 1, startRange, endRange);
    }

    for (int i = 0; i < racerCount; i++) {
        SetPriorityClass(racers[i].hProcess, racers[i].priorityClass);

        ResumeThread(racers[i].hThread); 
        CloseHandle(racers[i].hThread);
    }

    cout << "\n\t--- ГОНКА НАЧАЛАСЬ ---" << endl;
    startTime = GetTickCount();

    int finishedCount = 0;

    while (finishedCount < racerCount) {
        finishedCount = 0;
        cout << "\n\t--- ТЕКУЩИЙ СТАТУС ---" << endl;

        for (int i = 0; i < racerCount; i++) {
            DWORD exitCode;
            GetExitCodeProcess(racers[i].hProcess, &exitCode);

            if (exitCode == STILL_ACTIVE) {
                cout << "Участник " << racers[i].id << " (PID: " << racers[i].pid << ") | Приоритет: " << GetPriorityName(racers[i].priorityClass) << " | Статус: В ПРОЦЕССЕ" << endl;
            }
            else {
                if (!racers[i].finished) {
                    racers[i].finished = true;
                    racers[i].finishTime = (GetTickCount() - startTime) / 1000.0;
                    cout << ">>> - Участник " << racers[i].id << " ФИНИШИРОВАЛ | Время: " << racers[i].finishTime << " сек." << endl;
                }
                else {
                    cout << "Участник " << racers[i].id << " | Время: " << racers[i].finishTime << " сек. | ЗАВЕРШЕН" << endl;
                }
                finishedCount++;
            }
        }


        if (finishedCount < racerCount) {
            cout << "\nВведите ID участника для смены приоритета (0 - пропустить): ";
            int changeId;
            cin >> changeId;

            if (changeId >= 1 && changeId <= racerCount && !racers[changeId - 1].finished) {
                cout << "Новый приоритет для участника " << changeId << ":" << endl;
                int newPriority = ChoosePriority();
                racers[changeId - 1].priorityClass = newPriority;
                SetPriorityClass(racers[changeId - 1].hProcess, newPriority);
                cout << "Приоритет изменен" << endl;
            }
        }
        Sleep(1000);
    }


    for (int i = 0; i < racerCount - 1; i++) {
        for (int j = 0; j < racerCount - i - 1; j++) {
            if (racers[j].finishTime > racers[j + 1].finishTime) {
                RaceParticipant temp = racers[j];
                racers[j] = racers[j + 1];
                racers[j + 1] = temp;
            }
        }
    }

    cout << "\n\t --- ИТОГОВАЯ ТАБЛИЦА ---" << endl;
    cout << "Место\tID\tPID\tПриоритет\tВремя (сек)" << endl;
    for (int i = 0; i < racerCount; i++) {
        cout << (i + 1) << "\t" << racers[i].id << "\t" << racers[i].pid << "\t" << GetPriorityName(racers[i].priorityClass) << "\t\t" << racers[i].finishTime << endl;
    }


    for (int i = 0; i < racerCount; i++) {
        CloseHandle(racers[i].hProcess);
    }

    return 0;
}


