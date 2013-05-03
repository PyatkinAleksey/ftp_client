/* 
 * File:   UserInterface.h
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 15:01
 */

#pragma once

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

/**
 * Класс, с которым взаимодействует пользователь. Отвечает за получение свойств
 * соединения (адрес сервера, имя пользователя, пароль и т.д.), взаимодействие
 * с интерпретатором протокола. Также содержит вспомогательные функции (такие как
 * вывод сообщений на экран и т.д.).
 */
class UserInterface {
    public:
        UserInterface();                                // Вызов функций получения свойств соединения
        void setAddress(string address);                // Установка адреса сервера
        void setUser(string user);                      // Установка имени пользователя
        void setPassword(string password);              // Установить пароль
        void setType(string type);                      // Установить тип представления файлов
        void connect();                                 // Осуществление соединения через объект интерпретатора протокола
        void printMessage(int type, string message);    // Вывод сообщений на экран

    private:
        string address;     // Адрес сервера
        string user;        // Имя пользователя
        string password;    // Пароль
        string type;        // Тип представления файлов
};