/* 
 * File:    ProtocolInterpreter.h
 * Author:  Aleksey Pyatkin
 *
 * Created on 2 Май 2013 г., 15:37
 */

#pragma once

#include <iostream>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <time.h>
#include "UserInterface.h"
#include "UserDTP.h"

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
        void setLocalPath(string path);         // Установка локального пути для передачи файлов
        void setAddress(string address);        // Установка значения адреса сервера
        void setUser(string user);              // Установка значения имени пользователя
        void setPassword(string password);      // Установка значения пароля
        void setType(string type);              // Установка значения типа представления файлов
        void setMode(string mode);              // Установка значения режима передачи файлов
        void setStructure(string structure);    // Установка структуры файла
        void setPath(string path);              // Установка пути к файлу
        void setPortData();                     // Установка данных для команды PORT
        void setPort();                         // Установка номера динамического порта из 227 отклика команды PASV
        void setPassive(int passive);           // Установка флага использования пассивного режима
        void sendCommand(string command);       // Отправка команды
        
    private:
        static const int MAX_BUF_LEN = 4096;    // Максимальный размер буфера
        UserInterface *ui;                      // Объект пользовательского интерфейса
        UserDTP *udtp;                          // Объект пользовательского процесса передачи данных
        HANDLE connection;                      // Хэндлер потока соединения данных
        string localPath;                       // Локальный путь для сохранения передаваемых файлов
        string address;                         // Адрес сервера
        string user;                            // Имя пользователя
        string password;                        // Пароль
        string type;                            // Тип представления файлов
        string mode;                            // Режим передачи файлов
        string structure;                       // Структура файла
        string path;                            // Путь к файлу
        string portData;                        // Данные для команды PORT
        int port;                               // Номер порта для соединения
        int passive;                            // Флаг использования пассивного режима
        SOCKET connectionSocket;                // Сокет управляющего соединения
        char replyBuffer[MAX_BUF_LEN];          // Буфер откликов
        string commandBuffer;                   // Буфер команд
        int result;                             // Код результата
        
        void printReply();                              // Вывод отклика от сервера
        void sendUser();                                // Отправка команды USER
        void sendPass();                                // Отправка команды PASS
        void sendType();                                // Отправка команды TYPE
        void sendMode();                                // Отправка команды MODE
        void sendStru();                                // Отправка команды STRU
        void sendPort();                                // Отправка команды PORT
        void sendPasv();                                // Отправка команды PASV
        void sendRetr();                                // Отправка команды RETR
        void sendStor();                                // Отправка команды STOR
        void sendAbor();                                // Отправка команды ABOR
        void sendQuit();                                // Отправка команды QUIT
        void sendNoop();                                // Отправка команды NOOP
        friend DWORD WINAPI startDTP(LPVOID parameter); // Поточная функция передачи файлов
};