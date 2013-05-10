/* 
 * File:    UserDTP.h
 * Author:  Aleksey Pyatkin
 *
 * Created on 04.05.2013, 1:54
 */

#pragma once

#include <fstream>
#include <list>
#include "Service.h"

using namespace std;

/**
 * Класс пользовательского процесса передачи данных. Его задача - прослушивание
 * соединения от сервера, а также передача и получение файлов.
 */
class UserDTP {
    public:
        UserDTP();                          // Инициализация пользовательского процесса передачи данных
        ~UserDTP();                         // Освобождение ресурсов
        void openConnection();              // Открытие соединения данных
        void closeConnection();             // Закрытие соединения по данным
        list<string> fileList();            // Получить список имен файлов и директорий
        void fullList();                    // Получение списка файлов и директорий
        void retrieve();                    // Выполнение передачи файлов от сервера
        void store();                       // Выполнение передачи файлов на сервер
        void setLocalPath(string path);     // Установка локального пути для сохранения файлов, получаемых от сервера
        void setAddress(string address);    // Установка адреса сервера
        void setPath(string path);          // Установка пути к файлу
        void setPort(int port);             // Установка порта для соединения по данным
        void setPassive(int passive);       // Установка флага использования пассивного режима
        int isConnected();                  // Проверка, осуществлено ли соединение по данным
        
    private:
        Service *service;       // Объект сервисного класса
        SOCKET dataSocket;      // Сокет соединения данных
        SOCKET acceptSocket;    // Сокет для принятия входящих соединений
        int result;             // Код результата последней операции
        string localPath;       // Локальный путь для сохранения файлов
        string address;         // Адрес сервера
        string path;            // Путь к файлу
        int port;               // Порт для соединения по данным
        int passive;            // Флаг использования пассивного режима
        char buffer[4096];      // Буфер
        int connected;          // Флаг осуществлено ли соединение по данным
};