/* 
 * File:    Service.h
 * Author:  Aleksey Pyatkin
 *
 * Created on 09.05.2013, 0:52
 */

#pragma once

#include <iostream>
#include <string.h>

using namespace std;

/**
 * Сервисный класс. Содержит методы, предназначенные для выполнения общих задач,
 * таких как вывод сообщений на экран в разных форматах и т.п.
 */
class Service {
    
    public:
        void printMessage(int type, string message); // Вывод сообщений на экран
    
};