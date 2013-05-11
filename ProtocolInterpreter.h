/* 
 * File:    ProtocolInterpreter.h
 * Author:  Aleksey Pyatkin
 *
 * Created on 2 Май 2013 г., 15:37
 */

#pragma once

#include <iostream>
#include <string.h>
#include <list>
#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <time.h>
#include "Service.h"
#include "UserDTP.h"

using namespace std;

/**
 * Класс, отвечающий за открытие и закрытие управляющего соединения с сервером,
 * а также за отправку FTP-команд и управление процессом передачи данных.
 */
class ProtocolInterpreter {
    public:
        list<string> fileList;                  // Список имен файлов и директорий, полученный командой NLST
        
        ProtocolInterpreter();                  // Инициализация интерпретатора протокола
        ~ProtocolInterpreter();                 // Освобождение ресурсов
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
        void setNewPath(string newPath);        // Установка нового имени файла для команды переименования
        void setPortData();                     // Установка данных для команды PORT
        void setPort();                         // Установка номера динамического порта из 227 отклика команды PASV
        void setPassive(int passive);           // Установка флага использования пассивного режима
        int sendCommand(string command);        // Отправка команды
        
    private:
        static const int MAX_BUF_LEN = 4096;    // Максимальный размер буфера
        Service *service;                       // Объект сервисного класса
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
        string newPath;                         // Новое имя файла для команды переименования
        string portData;                        // Данные для команды PORT
        int port;                               // Номер порта для соединения
        int passive;                            // Флаг использования пассивного режима
        SOCKET connectionSocket;                // Сокет управляющего соединения
        char replyBuffer[MAX_BUF_LEN];          // Буфер откликов
        string commandBuffer;                   // Буфер команд
        int result;                             // Код результата
        
        void printReply();                              // Вывод отклика от сервера
        int sendUser();                                 // Отправка команды USER
        int sendPass();                                 // Отправка команды PASS
        int sendType();                                 // Отправка команды TYPE
        int sendMode();                                 // Отправка команды MODE
        int sendStru();                                 // Отправка команды STRU
        int sendPort();                                 // Отправка команды PORT
        int sendPasv();                                 // Отправка команды PASV
        int sendNlst();                                 // Отправка команды NLST
        int sendList();                                 // Отправка команды LIST
        int sendRetr();                                 // Отправка команды RETR
        int sendStor();                                 // Отправка команды STOR
        int sendRnto();                                 // Отправка команды RNTO
        int sendRnfr();                                 // Отправка команды RNFR
        int sendDele();                                 // Отправка команды DELE
        int sendMkd();                                  // Отправка команды MKD
        int sendRmd();                                  // Отправка команды RMD
        int sendCwd();                                  // Отправка команды CWD
        int sendCdup();                                 // Отправка команды CDUP
        int sendPwd();                                  // Отправка команды PWD
        int sendAbor();                                 // Отправка команды ABOR
        int sendRein();                                 // Отправка команды REIN
        int sendQuit();                                 // Отправка команды QUIT
        int sendSyst();                                 // Отправка команды SYST
        int sendStat();                                 // Отправка команды STAT
        int sendNoop();                                 // Отправка команды NOOP
        friend DWORD WINAPI startDTP(LPVOID parameter); // Поточная функция передачи файлов
};