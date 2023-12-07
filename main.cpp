#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <Winuser.h>

#include <list>
#include <memory>

using namespace std;

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Ntdll")
#pragma comment(lib, "Comctl32")

#include <objidl.h>
#include <gdiplus.h>
#include <winternl.h>
#include <CommCtrl.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>

HANDLE forksSemathore;

enum State
{
    eating,
    waiting,
    thinking
};

struct philosopher
{
    bool leftForkOwner;
    HANDLE leftForkMutex;
    State state;
    clock_t inactiveWaitingStart;
};

const long long MAX_THINKING_TIME = 1000000;
const long long MAX_EATING_TIME = 1000000;
LPCWSTR timer_name = L"ttimer";
LPCWSTR semathore_name = L"semathore";

size_t PHILOSOPHERS_COUNT = 5;
philosopher* PHILOSOPHERS = new philosopher[PHILOSOPHERS_COUNT]();

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    size_t id = (size_t)lpParam;
    DWORD thread_id = GetCurrentThreadId();
    philosopher& phil = PHILOSOPHERS[id];
    philosopher& nextPhil = PHILOSOPHERS[(id + 1) % PHILOSOPHERS_COUNT];

    HANDLE timer = CreateWaitableTimerW(NULL, FALSE, (std::to_wstring(id) + timer_name).c_str());

    while (true)
    {
       phil.state = thinking;

       LARGE_INTEGER liDueTime;
       liDueTime.QuadPart = -(long long)(rand()*777 % MAX_THINKING_TIME);
       SetWaitableTimer(timer, &liDueTime, NULL, NULL, NULL, FALSE);
       WaitForSingleObject(timer, INFINITE);

       phil.state = waiting;
       
       phil.inactiveWaitingStart = clock();

       CONST HANDLE handles[] = { forksSemathore, phil.leftForkMutex };

       WaitForMultipleObjects(2, handles, TRUE, INFINITE);
       phil.leftForkOwner = TRUE;
       WaitForSingleObject(nextPhil.leftForkMutex, INFINITE);
       nextPhil.leftForkOwner = FALSE;

       phil.state = eating;

       liDueTime.QuadPart = -(long long)(rand()*777 % MAX_EATING_TIME);
       SetWaitableTimer(timer, &liDueTime, NULL, NULL, NULL, FALSE);
       WaitForSingleObject(timer, INFINITE);

       phil.state = thinking;

       ReleaseMutex(phil.leftForkMutex);
       ReleaseMutex(nextPhil.leftForkMutex);
       ReleaseSemaphore(forksSemathore, 1, NULL);

       phil.leftForkOwner = FALSE;
    }

    CloseHandle(timer);

    return 0;
}

void CALLBACK EverySecond(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
    for (size_t i = 0; i < PHILOSOPHERS_COUNT; i++)
    {
        philosopher& phil = PHILOSOPHERS[i];

        if (phil.state == waiting)
            std::cout << "(" << clock() - phil.inactiveWaitingStart << ")";
        else if (phil.state == thinking)
            std::cout << "(thinking)";
        else
            std::cout << "(eating)";
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    forksSemathore = CreateSemaphore(NULL, PHILOSOPHERS_COUNT - 1, PHILOSOPHERS_COUNT - 1, semathore_name);

    for (size_t i = 0; i < PHILOSOPHERS_COUNT; i++)
    {
        PHILOSOPHERS[i].state = thinking;
        PHILOSOPHERS[i].leftForkMutex = CreateMutexW(NULL, NULL, std::to_wstring(i).c_str());
    }

    for (size_t i = 0; i < PHILOSOPHERS_COUNT; i++)
    {
        CreateThread(NULL, NULL, &MyThreadFunction, (LPVOID)i, NULL, NULL);
    }

    HANDLE timer = CreateWaitableTimerW(NULL, FALSE, timer_name);

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -1000LL;
    SetWaitableTimer(timer, &liDueTime, 1000, &EverySecond, NULL, FALSE);

    while (true)
    {
        SleepEx(INFINITE, TRUE);
    }

    return 0;
}