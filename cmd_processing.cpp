#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "lib.h"
#include "print_lib.h"
#include "loger.h"
#include "observer.h"

static int n_cnt_block_cmd = 0;    // кол-во команд в блоке. задается командной строкой
static int n_cnt_open = 0;        // кол-во открытых скобок.
static int n_cnt_close = 0;        // кол-во открытых скобок.

// очищаем очередь
void eraze_queue(std::queue<std::string> * _queue)
{
    while (!_queue->empty())
    {
        _queue->pop();
    }
    
}

// печатаем очередь
void print_queue(std::queue<std::string> * _queue, Loger * _loger)
{
    if (_queue->empty()) {
        // если очередь пуста, смысл что-то печатать
        return;
    }
    int idx = 0;
    std::string s_print = "bulk: ";

    for( ; !_queue->empty(); _queue->pop()) {
        s_print += _queue->front() + ", ";
        idx++;
    }
    print_value((std::string) s_print);
    _loger->AddRecord(s_print);
    
    s_print = "";
}


int main()
{
    std::queue<std::string> cmd_queue;

    std::cout << "Указать кол-во команд в статическом блоке : ";
    std::cin >> n_cnt_block_cmd;
    std::cout << std::endl;

    std::string s_line = "";
    int n_cnt_input_cmd = 0;        // кол-во введенных команд.
    std::cin >> s_line;
    // получаю текущее время
    time_t tm_new = time(0);    
    // формируется имя файла logа
    std::string s_file_name = "bulk" + std::to_string(tm_new);
    
    Subject subject = Subject();                                // создаю объект
    Observer observer = Observer(&subject, s_file_name);        // создаю подписчика
    subject.Attach(&observer);                                  // добавляю к объекту подписчика
    
    Loger lgr(s_file_name);
    
    while (s_line.compare("EOF") != 0 && s_line.compare("eof") != 0 )
    {
        if((s_line != "{") && s_line != "}") {
            if (n_cnt_open == 0) {
                n_cnt_input_cmd++;
            }
            cmd_queue.push(s_line);
            sleep(1);
        }
        if(s_line == "{") {
            if (n_cnt_open == 0) {
                // если первый раз ввели { - начинаем вводить динамический блок
                // print_queue(&cmd_queue, &lgr);
                subject.SetValue(&cmd_queue);                   // вызов изменения в объекте
                n_cnt_input_cmd = 0;
            }
            n_cnt_open++;
        }
        if(s_line == "}") {
            // если ввели } и счеткики открытых и закрытых скобок равны, то значит закончился динамический блок
            n_cnt_close++;
            if (n_cnt_open == n_cnt_close) {
                // print_queue(&cmd_queue, &lgr);
                subject.SetValue(&cmd_queue);                  // вызов изменения в объекте
                n_cnt_open = 0;
                n_cnt_close = 0;
                n_cnt_input_cmd = 0;
            }
        }
        if ((n_cnt_input_cmd == n_cnt_block_cmd) && (n_cnt_open == 0)) {
            // выводим результат если ввели нужное число команд и не вводили динамический блок
            // print_queue(& cmd_queue, &lgr);
            subject.SetValue(&cmd_queue);                      // вызов изменения в объекте
            n_cnt_input_cmd = 0;
        }
        std::cin >> s_line;
    }

    // если что-то осталось в очереди, то выводим это что-то
    if(!cmd_queue.empty()) {
        // print_queue(&cmd_queue, &lgr);
        subject.SetValue(&cmd_queue);                         // вызов изменения в объекте
    }
    
    return 0;
}
