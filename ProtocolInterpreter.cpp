/* 
 * File:   ProtocolInterpreter.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 22:50
 */

#include "ProtocolInterpreter.h"

ProtocolInterpreter::ProtocolInterpreter(UserInterface ui, string address, string user) {
    this->ui = ui;
    this->address = address;
    this->user = user;
}

void ProtocolInterpreter::ftpConnect() {
    WORD wVersionRequested;
    WSADATA wsaData;
    sockaddr_in clientAddress;
    
    wVersionRequested = MAKEWORD(2, 2);
    result = WSAStartup(wVersionRequested, &wsaData);
    if (result != NO_ERROR) {
        ui.printMessage(2, "Connection failed!");
        return;
    }
    connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectionSocket == INVALID_SOCKET) {
        ui.printMessage(2, "Invalid socket!");
        WSACleanup();
        return;
    }
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr(address.c_str());
    clientAddress.sin_port = htons(21);
    result = connect(connectionSocket, (SOCKADDR*)&clientAddress, sizeof(clientAddress));
    if (result == SOCKET_ERROR) {
        ui.printMessage(2, "Connection error!");
        result = closesocket(connectionSocket);
        if (result == SOCKET_ERROR) {
            ui.printMessage(2, "Close socket error!");
        }
        WSACleanup();
        return;
    }
    do {
        result = recv(connectionSocket, buf, MAX_BUF_LEN, 0);
        if (result > 0) {
            ui.printMessage(0, buf);
            if (buf[result-6] == '2' && buf[result-5] == '2' && buf[result-4] == '0') {
                // Приветствие получено
                break;
            }
            memset(buf, 0, MAX_BUF_LEN);
        }
    } while(result > 0);
    authorize();
    result = closesocket(connectionSocket);
    WSACleanup();
}

void ProtocolInterpreter::authorize() {
    // Отправка команды USER
    memset(buf, 0, MAX_BUF_LEN);
    strcat(buf, "USER ");
    strcat(buf, user.c_str());
    strcat(buf, "\r\n");
    printf("%s", buf);
    result = send(connectionSocket, buf, MAX_BUF_LEN, 0);
    if (result == SOCKET_ERROR) {
        ui.printMessage(2, "USER sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    // Получение отклика
    memset(buf, 0, MAX_BUF_LEN);
    do {
        result = recv(connectionSocket, buf, MAX_BUF_LEN, 0);
        if (result > 0) {
            ui.printMessage(0, buf);
            break;
        }
    } while(result > 0);
}