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
    setAddress("80.250.160.25");
    setUser("anonymous");
    setPassword("");
    setType("A N");
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