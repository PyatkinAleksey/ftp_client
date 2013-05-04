/* 
 * File:   UserInterface.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 16:38
 */

#include "UserInterface.h"
#include "ProtocolInterpreter.h"

/**
 * Вызов функций получения свойств соединения.
 */
UserInterface::UserInterface() {
    setLocalPath("D:\\");
    setAddress("80.250.160.25");
    setUser("anonymous");
    setPassword("");
    setType("A N");
    setMode("S");
    setStructure("F");
    setPath("robots.txt");
    setPortData("109,229,241,245,4,150");
    setPassive(0);
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
 * Установка данных для команды PORT.
 * 
 * @param portData Данные (IP-адрес и порт).
 */
void UserInterface::setPortData(string portData) {
    this->portData = portData;
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
    ProtocolInterpreter *pi = new ProtocolInterpreter(this);
    pi->setAddress(address);
    pi->setUser(user);
    pi->setPassword(password);
    pi->openControlConnection();
    printMessage(0, "USER " + user + "\n");
    pi->sendCommand("USER");
    pi->setType(type);
    printMessage(0, "TYPE A N\n");
    pi->sendCommand("TYPE");
    pi->setMode(mode);
    printMessage(0, "MODE S\n");
    pi->sendCommand("MODE");
    pi->setStructure(structure);
    printMessage(0, "STRU F\n");
    pi->sendCommand("STRU");
    pi->setPassive(passive);
    if (passive) {
        printMessage(0, "PASV\n");
        pi->sendCommand("PASV");
    } else {
        pi->setPortData(portData);
        printMessage(0, "PORT " + portData + "\n");
        pi->sendCommand("PORT");
    }
    pi->setPath(path);
    pi->setLocalPath(localPath);
    printMessage(0, "RETR " + path + "\n");
    pi->sendCommand("RETR");
    printMessage(0, "NOOP\n");
    pi->sendCommand("NOOP");
    printMessage(0, "QUIT\n");
    pi->sendCommand("QUIT");
    pi->closeControlConnection();
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