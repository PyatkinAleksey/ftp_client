/* 
 * File:   UserInterface.h
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 15:01
 */

#pragma once

#include <iostream>
#include <string.h>
#include <list>
#include "Service.h"
#include "ProtocolInterpreter.h"

using namespace std;

/**
 * Класс, с которым взаимодействует пользователь. Отвечает за получение свойств
 * соединения (адрес сервера, имя пользователя, пароль и т.д.), взаимодействие
 * с интерпретатором протокола. Также содержит вспомогательные функции (такие как
 * вывод сообщений на экран и т.д.).
 */
class UserInterface {
    public:
        UserInterface();                        // Инициализация класса
        ~UserInterface();                       // Освобождение ресурсов
        void connect();                         // Осуществление соединения через объект интерпретатора протокола
        void doCommand(string command);         // Выполнение команд пользовательского интерфейса
        
    private:
        Service *service;           // Объект сервисного класса
        ProtocolInterpreter *pi;    // Объект интерпретатора протокола
        string iniPath;             // Путь к конфигурационному файлу
        string localPath;           // Локальный адрес для сохранения файлов
        string address;             // Адрес сервера
        string user;                // Имя пользователя
        string password;            // Пароль
        string type;                // Тип представления файлов
        string mode;                // Режим передачи файлов
        string structure;           // Структура файла
        string path;                // Путь к файлу
        string newPath;             // Новое имя файла для команды rename
        int passive;                // Флаг использования пассивного режима
};