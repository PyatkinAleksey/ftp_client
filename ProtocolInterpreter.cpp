/* 
 * File:   ProtocolInterpreter.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 22:50
 */

#include "ProtocolInterpreter.h"

/**
 * Инициализация интерпретатора протокола.
 * 
 * @param ui Объект пользовательского интерфейса.
 */
ProtocolInterpreter::ProtocolInterpreter(UserInterface *ui) {
    this->ui = ui;
}

/**
 * Установка поля адреса сервера.
 * 
 * @param address Адрес сервера.
 */
void ProtocolInterpreter::setAddress(string address) {
    this->address = address;
}

/**
 * Установка поля имени пользователя.
 * 
 * @param user Имя пользователя.
 */
void ProtocolInterpreter::setUser(string user) {
    this->user = user;
}

/**
 * Установка пароля.
 * 
 * @param password Пароль.
 */
void ProtocolInterpreter::setPassword(string password) {
    this->password = password;
}

/**
 * Установка типов представления файлов.
 * 
 * @param type Тип представления файлов.
 */
void ProtocolInterpreter::setType(string type) {
    this->type = type;
}

/**
 * Открытие управляющего соединения.
 */
void ProtocolInterpreter::openControlConnection() {
    WORD wVersionRequested;
    WSADATA wsaData;
    sockaddr_in clientAddress;
    
    wVersionRequested = MAKEWORD(2, 2);
    result = WSAStartup(wVersionRequested, &wsaData);
    if (result != NO_ERROR) {
        ui->printMessage(2, "Connection failed!");
        return;
    }
    // Создание сокета
    connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectionSocket == INVALID_SOCKET) {
        ui->printMessage(2, "Invalid socket!");
        WSACleanup();
        return;
    }
    // Открытие управляющего соединения
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr(address.c_str());
    clientAddress.sin_port = htons(21);
    result = connect(connectionSocket, (SOCKADDR*)&clientAddress, sizeof(clientAddress));
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "Connection error!");
        result = closesocket(connectionSocket);
        if (result == SOCKET_ERROR) {
            ui->printMessage(2, "Close socket error!");
        }
        WSACleanup();
        return;
    }
    // Получение отклика-приветствия
    do {
        result = recv(connectionSocket, replyBuffer, MAX_BUF_LEN, 0);
        if (result > 0) {
            ui->printMessage(0, replyBuffer);
            if (strstr(replyBuffer, "220 ") != NULL) {
                // Приветствие получено
                break;
            }
            memset(replyBuffer, 0, MAX_BUF_LEN);
        }
    } while(result > 0);
}

/**
 * Закрытие управляющего соединения.
 */
void ProtocolInterpreter::closeControlConnection() {
    result = closesocket(connectionSocket);
    WSACleanup();
}

/**
 * Вывод на экран отклика от FTP-сервера.
 */
void ProtocolInterpreter::printReply() {
    memset(replyBuffer, 0, MAX_BUF_LEN);
    do {
        result = recv(connectionSocket, replyBuffer, MAX_BUF_LEN, 0);
        if (result > 0) {
            ui->printMessage(0, replyBuffer);
            break;
        }
    } while(result > 0);
}

/**
 * Отправка команды FTP-серверу.
 * 
 * @param command FTP-команда.
 */
void ProtocolInterpreter::sendCommand(string command) {
    if (command == "USER") {
        sendUser();
    } else if (command == "PASS") {
        sendPass();
    } else if (command == "TYPE") {
        sendType();
    } else if (command == "QUIT") {
        sendQuit();
    } else if (command == "NOOP") {
        sendNoop();
    } else {
        ui->printMessage(1, "Unknown command!");
    }
}

/**
 * Отправка команды USER.
 */
void ProtocolInterpreter::sendUser() {
    commandBuffer = "USER " + user + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "USER sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
    if (strstr(replyBuffer, "331 ")) { // Требуется ввод дополнительной информации (например, E-Mail-адрес)
        ui->printMessage(0, "PASS ***\n");
        sendCommand("PASS");
    }
}

/**
 * Отправка команды PASS.
 */
void ProtocolInterpreter::sendPass() {
    if (password == "") {
        ui->printMessage(1, "Enter your password:");
        cin >> password;
        ui->setPassword(password);
    }
    commandBuffer = "PASS " + password + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "Additional information sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды TYPE.
 */
void ProtocolInterpreter::sendType() {
    commandBuffer = "TYPE " + type + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "TYPE sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды QUIT.
 */
void ProtocolInterpreter::sendQuit() {
    commandBuffer = "QUIT\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "QUIT sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды NOOP.
 */
void ProtocolInterpreter::sendNoop() {
    commandBuffer = "NOOP\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "NOOP sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}