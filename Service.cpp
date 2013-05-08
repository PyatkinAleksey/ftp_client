/* 
 * File:   Service.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 09.05.2013, 0:55
 */

#include "Service.h"

/**
 * Вывод сообщений на экран.
 * 
 * @param type Тип сообщений (0 - простой вывод, 1 - информационное сообщение,
 *      2 - ошибка, 3 - ввод информации).
 * @param message Сообщение для вывода.
 */
void Service::printMessage(int type, string message) {
    switch (type) {
        case 0:
            cout << message;
            break;
        case 1:
            cout << "Info: " << message << endl;
            break;
        case 2:
            cout << "Error: " << message << endl;
            break;
        case 3:
            cout << "> " << message << endl;
            cout << "> ";
            break;
        default: break;
    }
}