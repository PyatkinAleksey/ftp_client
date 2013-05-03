/* 
 * File:    ProtocolInterpreter.h
 * Author:  Aleksey Pyatkin
 *
 * Created on 2 Май 2013 г., 15:37
 */

#pragma once

#include <iostream>
#include <string.h>
#include <winsock2.h>
#include "UserInterface.h"

using namespace std;

/**
 * Класс, отвечающий за открытие и закрытие управляющего соединения с сервером,
 * а также за отправку FTP-команд и управление процессом передачи данных.
 */
class ProtocolInterpreter {
    public:
        ProtocolInterpreter(UserInterface *ui); // Инициализация интерпретатора протокола
        void openControlConnection();           // Открытие управляющего соединения
        void closeControlConnection();          // Закрытие управляющего соединения
        void setAddress(string address);        // Установка значения адреса сервера
        void setUser(string user);              // Установка значения имени пользователя
        void setPassword(string password);      // Установка значения пароля
        void setType(string type);              // Установка значения типа представления файлов
        void setMode(string mode);              // Установка значения режима передачи файлов
        void sendCommand(string command);       // Отправка команды
        
    private:
        static const int MAX_BUF_LEN = 4096;    // Максимальный размер буфера
        UserInterface *ui;                      // Объект пользовательского интерфейса
        string address;                         // Адрес сервера
        string user;                            // Имя пользователя
        string password;                        // Пароль
        string type;                            // Тип представления файлов
        string mode;                            // Режим передачи файлов
        SOCKET connectionSocket;                // Сокет управляющего соединения
        char replyBuffer[MAX_BUF_LEN];          // Буфер откликов
        string commandBuffer;                   // Буфер команд
        int result;                             // Код результата
        
        void printReply();                      // Вывод отклика от сервера
        void sendUser();                        // Отправка команды USER
        void sendPass();                        // Отправка команды PASS
        void sendType();                        // Отправка команды TYPE
        void sendMode();                        // Отправка команды MODE
        void sendQuit();                        // Отправка команды QUIT
        void sendNoop();                        // Отправка команды NOOP
};