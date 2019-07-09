#include "TimerMgr.h"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

void  test()
{
    cout << "test"<<endl;
}

void temp_func()
{
    cout << "--------------" << endl;
}

int  main()
{
    TimerManager manager;
    MyTimer timer1(manager);
    timer1.start(&test, 180, MyTimer::TimerType::CIRCLE);
    MyTimer timer2(manager);
    timer2.start(&temp_func, 200, MyTimer::TimerType::ONCE);
    MyTimer timer3(manager);
    timer3.start(&temp_func, 100, MyTimer::TimerType::ONCE);

    getchar();
    getchar();
    
    return 0;
}