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
    SOCKET connectionSocket;
    sockaddr_in clientAddress;
    char buf[4096];
    int result;
    
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
        result = recv(connectionSocket, buf, 4096, 0);
        if (result > 0) {
            ui.printMessage(0, buf);
            memset(buf, 0, 4096);
        }
    } while(result > 0);
    result = closesocket(connectionSocket);
    WSACleanup();
}