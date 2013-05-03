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
    setAddress();
    setUser();
}

/**
 * Получение адреса сервер.
 */
void UserInterface::setAddress() {
    address = "80.250.160.25"; //ftp.ru.debian.org
}

/**
 * Получение имени пользователя.
 */
void UserInterface::setUser() {
    user = "anonymous";
}

/**
 * Осуществление соединения, посредством использования интерпретатора протокола.
 */
void UserInterface::connect() {
    ProtocolInterpreter *pi = new ProtocolInterpreter(this);
    pi->setAddress(address);
    pi->setUser(user);
    pi->openControlConnection();
    pi->sendCommand("USER");
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