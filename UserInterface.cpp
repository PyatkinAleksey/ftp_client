/* 
 * File:   UserInterface.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 16:38
 */

#include "UserInterface.h"

/**
 * Инициализация класса, получение свойств из конфигурационного файла и т.д.
 */
UserInterface::UserInterface() {
    service = new Service();
    pi = new ProtocolInterpreter();
    char currentDirectory[MAX_PATH];
    char charParameter[256];
    int intParameter;
    
    iniPath = "";
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    iniPath.append(currentDirectory).append("\\config.ini");
    GetPrivateProfileString("connection", "address", "127.0.0.1", charParameter, 100, iniPath.c_str());
    setAddress(charParameter);
    GetPrivateProfileString("connection", "user", "anonymous", charParameter, 100, iniPath.c_str());
    setUser(charParameter);
    GetPrivateProfileString("connection", "password", "", charParameter, 100, iniPath.c_str());
    setPassword(charParameter);
    GetPrivateProfileString("modes", "type", "A N", charParameter, 100, iniPath.c_str());
    setType(charParameter);
    GetPrivateProfileString("modes", "mode", "S", charParameter, 100, iniPath.c_str());
    setMode(charParameter);
    GetPrivateProfileString("modes", "structure", "F", charParameter, 100, iniPath.c_str());
    setStructure(charParameter);
    intParameter = GetPrivateProfileInt("modes", "passive", 1, iniPath.c_str());
    setPassive(intParameter);
    GetPrivateProfileString("paths", "path", "", charParameter, 100, iniPath.c_str());
    setPath(charParameter);
    GetPrivateProfileString("paths", "localPath", "", charParameter, 100, iniPath.c_str());
    setLocalPath(charParameter);
}

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
    char tmp[1024];
    string command;
    
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
        service->printMessage(3, "Please, enter your command:");
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
    service->printMessage(1, path);
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
    } else if (command.substr(0, 4) == "help") {
        service->printMessage(1, "You can use following commands:");
        service->printMessage(1, "get <path> - to get a file");
    }
}

UserInterface::~UserInterface() {
    delete pi;
    delete service;
}