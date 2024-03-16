#pragma once

#include <vector>
#include <string>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "loger.h"
#include "print_lib.h"

// класс объекта
class Subject
{
private:
    std::vector <class Observer *> observers;
    std::string sFileName;
    std::queue<std::string> * cmd_queue;
public:
    Subject();
    Subject(std::string sFile);

    // добаляю подписчиков
    void Attach(Observer * newObserver);

    // задать значение
    void SetValue(std::queue<std::string> * _queue);

    // извещаю подписчиков
    void Notify();
    
    // возвращаю имя файла лога
    std::string GetLogFileName();
    
    // возвращаю очередь с данными
    std::queue<std::string> * GetCmdQueue();

    ~Subject();
};

// =========================================================================================

// класс подписчика
class Observer
{
private:
    Subject * ptrSubject;
    Loger loger = Loger("");

    // для многопоточки
    std::string sFileName = "";                 // имя файла
public:
    static std::mutex m1;                       // мьютекс
    static std::condition_variable cond_data;   // условная переменная для разруливания потоков записи в файл
    static std::atomic<bool> bFinishing;

public:
    Observer(Subject * modif, std::string sFile);
    
    // вывод в консоль и файл одновременно без потока
    void Update();

    // вывод в консоль и файл потоками
    void Update_Thread();
    ~Observer();

private:
};

