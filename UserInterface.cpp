/* 
 * File:   UserInterface.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 16:38
 */

#include "UserInterface.h"
#include "ProtocolInterpreter.h"

UserInterface::UserInterface() {
    setAddress();
    setUser();
}

void UserInterface::setAddress() {
    address = "80.250.160.25"; //ftp.ru.debian.org
}

void UserInterface::setUser() {
    user = "anonymous";
}

void UserInterface::connect() {
    ProtocolInterpreter *pi = new ProtocolInterpreter(*this, address, user);
    pi->ftpConnect();
}

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