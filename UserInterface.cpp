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
    do {
        service->printMessage(3, "Please, enter your command:");
        cin.getline(tmp, 1024);
        command = tmp;
        doCommand(command);
    } while (command != "quit");
    pi->sendCommand("QUIT");
    pi->closeControlConnection();
}

/**
 * Выполнение команд пользовательского интерфейса.
 * 
 * @param command Команда.
 */
void UserInterface::doCommand(string command) {
    if (command.substr(0, 3) == "get") {
        pi->setPassive(passive);
        if (passive) {
            pi->sendCommand("PASV");
        } else {
            pi->sendCommand("PORT");
        }
        path = command.substr(4, command.substr(4).find(" "));
        localPath = command.substr(command.find("to ") + 3);
        pi->setPath(path);
        pi->setLocalPath(localPath);
        pi->sendCommand("RETR");
    } else if (command.substr(0, 4) == "send") {
        pi->setPassive(passive);
        if (passive) {
            pi->sendCommand("PASV");
        } else {
            pi->sendCommand("PORT");
        }
        path = command.substr(5, command.substr(5).find(" "));
        pi->setPath(path);
        pi->sendCommand("STOR");
    } else if (command.substr(0, 6) == "delete") {
        path = command.substr(7, command.substr(7).find(" "));
        pi->setPath(path);
        pi->sendCommand("DELE");
    } else if (command.substr(0, 3) == "cwd") {
        path = command.substr(4, command.substr(4).find(" "));
        pi->setPath(path);
        pi->sendCommand("CWD");
    } else if (command == "abort") {
        pi->sendCommand("ABOR");
    } else if (command.substr(0, 4) == "type") {
        type = command.substr(5, command.substr(5).find(" "));
        pi->setType(type);
        pi->sendCommand("TYPE");
    } else if (command.substr(0, 4) == "mode") {
        mode = command.substr(5, command.substr(5).find(" "));
        pi->setMode(mode);
        pi->sendCommand("MODE");
    } else if (command.substr(0, 6) == "struct") {
        structure = command.substr(7, command.substr(7).find(" "));
        pi->setStructure(structure);
        pi->sendCommand("STRU");
    } else if (command == "noop") {
        pi->sendCommand("NOOP");
    } else if (command == "help") {
        service->printMessage(0, "\tYou can use following commands:\n");
        service->printMessage(0, "\tget <path> to <path> - to get a file from server;\n");
        service->printMessage(0, "\tsend <path> - to send a file to server;\n");
        service->printMessage(0, "\tdelete <path> - to delete file from server;\n");
        service->printMessage(0, "\tabort - to abort current operation;\n");
        service->printMessage(0, "\ttype <type> - switch to type;\n");
        service->printMessage(0, "\tmode <mode> - switch to mode;\n");
        service->printMessage(0, "\tstruct <structure> - switch to structure;\n");
        service->printMessage(0, "\tnoop - no operation;\n");
        service->printMessage(0, "\tquit - close the client.\n");
    } else if (command == "quit") {
    } else {
        service->printMessage(2, "Unknown command!");
    }
}

UserInterface::~UserInterface() {
    delete pi;
    delete service;
}