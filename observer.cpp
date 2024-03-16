#include "observer.h"


Subject::Subject() {
    observers = std::vector<Observer *>();
}

Subject::Subject(std::string sFile) {
    observers = std::vector<Observer *>();
    sFileName = sFile;
}

Subject::~Subject() {
}

// добаляю подписчиков
void Subject::Attach(Observer * newObserver) {
    observers.push_back(newObserver);
}

// задаю значение
void Subject::SetValue(std::queue <std::string> * _queue) {
    cmd_queue = _queue;
    Notify();
}

// измещаю подписчиков
void Subject::Notify() {
    for (Observer * currObs : observers) {
        currObs->Update_Thread();
        //currObs->Update();
    }
}

// возвращаю имя файла лога
std::string Subject::GetLogFileName() {
    return sFileName;
}

// возвращаю очередь с данными
std::queue<std::string> * Subject::GetCmdQueue() {
    return cmd_queue;
}

// =========================================================================================

Observer::Observer(Subject * modif, std::string sFile) {
    ptrSubject = modif;
    loger = Loger(sFile + ".log");
    sFileName = sFile;
}

void Observer::Update() {
    std::queue<std::string> * queue_data = ptrSubject->GetCmdQueue();
    if (queue_data->empty()) {
        // если очередь пуста, смысл что-то печатать
        return;
    }
    
    int idx = 0;

    std::string s_print = "bulk: ";
    for( ; !queue_data->empty(); queue_data->pop()) {
        s_print += queue_data->front() + ", ";
        idx++;
    }
    
    print_value((std::string) s_print);

    loger.AddRecord(s_print);

    s_print = "";
}

// вывод в консоль и файл потоками
void Observer::Update_Thread() {
    std::queue<std::string> * queue_print = ptrSubject->GetCmdQueue();  // очередь для записи в консоль
    std::queue<std::string> queue_file1 = * ptrSubject->GetCmdQueue();    // очередь для записи в файл
    std::queue<std::string> queue_file2 = * ptrSubject->GetCmdQueue();    // очередь для записи в файл

    if (queue_print->empty()) {
        // если очередь пуста, смысл что-то печатать
        return;
    }

    auto prnt_console = [](std::queue<std::string> *_q_cmd) {
        //int idx = 0;
        std::string s_print = "bulk: ";
        for( ; !_q_cmd->empty(); _q_cmd->pop()) {
            s_print += _q_cmd->front() + ", ";
            //idx++;
        }

        print_value((std::string) s_print);
        s_print = "";
    };

    std::mutex m2;

    auto prnt_file = [&m2](std::queue<std::string> * _q_cmd, std::string s_file, int num_thread) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard lock(m2);

        std::string s_data = "bulk: ";
        fstream file_add(s_file, ios::app);
        
        if (!file_add.is_open()) {
                return;
        }

        int idx = 1;
        if (num_thread == 1) {
        // если первый поток. то отбираем нечетные команды  и пишем их в файл
            while (!_q_cmd->empty()) {
                if (idx % 2 != 0) {
                    s_data += _q_cmd->front() + ", ";
                }
                _q_cmd->pop();
                idx++;
            }
        }
             
        if (num_thread == 2) {
        // если первый поток. то отбираем четные команды  и пишем их в файл
            while (!_q_cmd->empty()) {
                if (idx % 2 == 0) {
                    s_data += _q_cmd->front() + ", ";
                }
                _q_cmd->pop();
                idx++;
            }
        }
        file_add << s_data << std::endl;
        
        file_add.close();
    };

    //поток вывода в консоль
    std::thread t_print_console(prnt_console, queue_print);
    t_print_console.join();

    // записываем в файл    
    /*
    не получилось использовать одну очередь для обоих потоков и распарралелить работу потоков.
    сделал проще: в один файл пишутся четные команды в другой нечетные
    */
    std::thread t_print_file1(prnt_file, &queue_file1, sFileName + "_1.log", 1);
    std::thread t_print_file2(prnt_file, &queue_file2, sFileName + "_2.log", 2);

    t_print_file1.join();
    t_print_file2.join();
}

Observer::~Observer()
{
}
