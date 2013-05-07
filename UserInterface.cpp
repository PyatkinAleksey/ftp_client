/* 
 * File:   UserInterface.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 16:38
 */

#include "UserInterface.h"

/**
 * Установка локального адреса для сохранения файлов, получаемых от FTP-сервера.
 * 
 * @param path Путь.
 */
void UserInterface::setLocalPath(string path) {
    this->localPath = path;
}

/**
 * Установка адреса сервер.
 * 
 * @param address Адрес сервера.
 */
void UserInterface::setAddress(string address) {
    this->address = address;
}

/**
 * Установка имени пользователя.
 * 
 * @param user Имя пользователя.
 */
void UserInterface::setUser(string user) {
    this->user = user;
}

/**
 * Установка пароля.
 * 
 * @param password Пароль.
 */
void UserInterface::setPassword(string password) {
    this->password = password;
}

/**
 * Установить тип представления файлов.
 * 
 * @param type Тип представления файлов.
 */
void UserInterface::setType(string type) {
    this->type = type;
}

/**
 * Установить режим передачи файлов.
 * 
 * @param mode Режим передачи файлов.
 */
void UserInterface::setMode(string mode) {
    this->mode = mode;
}

/**
 * Установка структуры файла.
 * 
 * @param structure Структура файла.
 */
void UserInterface::setStructure(string structure) {
    this->structure = structure;
}

/**
 * Установить путь к файлу.
 * 
 * @param path Путь.
 */
void UserInterface::setPath(string path) {
    this->path = path;
}

/**
 * Установка использования пассивного режима.
 * 
 * @param passive Флаг использования пассивного режима (0 - активный, другое - пассивный).
 */
void UserInterface::setPassive(int passive) {
    this->passive = passive;
}

/**
 * Осуществление соединения, посредством использования интерпретатора протокола.
 */
void UserInterface::connect() {
    ProtocolInterpreter *pi; // Указатель на объект интерпретатора протокола
    char tmp[1024];
    string command;
    pi = new ProtocolInterpreter(this);
    
    pi->setAddress(address);
    pi->setUser(user);
    pi->setPassword(password);
    pi->openControlConnection();
    pi->sendCommand("USER");
    pi->setType(type);
    pi->sendCommand("TYPE");
    pi->setMode(mode);
    pi->sendCommand("MODE");
    pi->setStructure(structure);
    pi->sendCommand("STRU");
    do {
        cout << "Please, enter your command:" << endl;
        cin.getline(tmp, 1024);
        command = tmp;
        doCommand(command);
    } while (command != "quit");

//    setPath("incoming/robots.txt");
//    pi->setPath(path);
//    pi->setPassive(passive);
//    if (passive) {
//        pi->sendCommand("PASV");
//    } else {
//        pi->sendCommand("PORT");
//    }
//    pi->sendCommand("STOR");
//    pi->sendCommand("DELE");
//    pi->sendCommand("NOOP");
    pi->sendCommand("QUIT");
    pi->closeControlConnection();
}

/**
 * Выполнение команд пользовательского интерфейса.
 * 
 * @param command Команда.
 */
void UserInterface::doCommand(string command) {
    path = command.substr(4, command.substr(4).find(" "));
    return;
    if (command.substr(0, 3) == "get") {
        pi->setPassive(passive);
        if (passive) {
            pi->sendCommand("PASV");
        } else {
            pi->sendCommand("PORT");
        }
        path = command.substr(4, command.substr(4).find(" "));
        pi->setPath(path);
        pi->setLocalPath(localPath);
        pi->sendCommand("RETR");
    } else if (command == "help") {
        cout << "You can use following commands:" << endl;
        cout << "get <path> - to get a file" << endl;
    }
}

/**
 * Вывод сообщений на экран.
 * 
 * @param type Тип сообщений (0 - простой вывод, 1 - информационное сообщение, 2 - ошибка).
 * @param message Сообщение для вывода.
 */
void UserInterface::printMessage(int type, string message) {
    switch (type) {
        case 0:
            printf("%s", message.c_str());
            break;
        case 1:
            printf("Info: %s\n", message.c_str());
            break;
        case 2:
            printf("Error: %s\n", message.c_str());
            break;
        default: break;
    }
}