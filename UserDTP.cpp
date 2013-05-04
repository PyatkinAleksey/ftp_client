/* 
 * File:   UserDTP.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 04.05.2013, 2:01
 */

#include <winsock2.h>

#include "UserDTP.h"
#include "UserInterface.h"

/**
 * Инициализация пользовательского процесса передачи данных.
 */
UserDTP::UserDTP(UserInterface *ui) {
    this->ui = ui;
    this->connected = 0;
}

/**
 * Установка локального пути для сохранения файлов, получаемых от сервера.
 * 
 * @param path Путь.
 */
void UserDTP::setLocalPath(string path) {
    this->localPath = path;
}

/**
 * Установка адреса сервера.
 * 
 * @param address Адрес сервера.
 */
void UserDTP::setAddress(string address) {
    this->address = address;
}

/**
 * Установка пути к файлу.
 * 
 * @param path Путь.
 */
void UserDTP::setPath(string path) {
    this->path = path;
}

/**
 * Установка порта для соединения по данным.
 * 
 * @param port Порт.
 */
void UserDTP::setPort(int port) {
    this->port = port;
}

/**
 * Установить флаг использования пассивного режима.
 * 
 * @param passive Флаг (0 - активный режима, другое - пассивный).
 */
void UserDTP::setPassive(int passive) {
    this->passive = passive;
}

/**
 * Проверка, осуществлено ли соединение по данным.
 * 
 * @return 0 - нет соединения, другое - соединение установлено.
 */
int UserDTP::isConnected() {
    return connected;
}

/**
 * Открытие соединения по данным (прослушивающий сокет).
 */
void UserDTP::openConnection() {
    WSADATA wsaData;
    sockaddr_in dataAddress;
    
    if (isConnected()) {
        return;
    }
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) {
        ui->printMessage(2, "Connection failed!");
        return;
    }
    dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (dataSocket == INVALID_SOCKET) {
        ui->printMessage(2, "Invalid socket!");
        WSACleanup();
        return;
    }
    dataAddress.sin_family = AF_INET;
    if (passive) {
        dataAddress.sin_addr.s_addr = inet_addr(address.c_str());
        dataAddress.sin_port = htons(port);
        result = connect(dataSocket, (SOCKADDR*)&dataAddress, sizeof(dataAddress));
        if (result == SOCKET_ERROR) {
            ui->printMessage(2, "Socket error!");
            result = closesocket(dataSocket);
            if (result == SOCKET_ERROR) {
                ui->printMessage(2, "Socket error!");
            }
            WSACleanup();
            return;
        }
        connected = 1;
    } else {
        dataAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        dataAddress.sin_port = htons(1174);
        result = bind(dataSocket, (SOCKADDR*)&dataAddress, sizeof(dataAddress));
        if (result == SOCKET_ERROR) {
            ui->printMessage(2, "Socket error!");
            result = closesocket(dataSocket);
            if (result == SOCKET_ERROR) {
                ui->printMessage(2, "Socket error!");
            }
            WSACleanup();
            return;
        }
        if (listen(dataSocket, SOMAXCONN) == SOCKET_ERROR) {
            ui->printMessage(2, "Socket error!");
            closesocket(dataSocket);
            WSACleanup();
            return;
        }
        acceptSocket = accept(dataSocket, NULL, NULL);
        if (acceptSocket == INVALID_SOCKET) {
            ui->printMessage(2, "Invalid accept socket!");
            closesocket(dataSocket);
            WSACleanup();
            return;
        } else {
            connected = 1;
        }
    }
}

/**
 * Закрытие соединения по данным (прослушивающий сокет).
 */
void UserDTP::closeConnection() {
    closesocket(dataSocket);
    WSACleanup();
}

/**
 * Выполнение передачи файлов от сервера.
 */
void UserDTP::retrieve() {
    ofstream stream;
    string fullPath = localPath + path;

    stream.open(fullPath.c_str(), ofstream::out);
    do {
        memset(buffer, 0, 4096);
        result = recv(dataSocket, buffer, 4096, 0);
        if (result > 0) {
            if (strstr(buffer, "226 ")) {
                ui->printMessage(0, buffer);
            }
            stream << buffer;
        }
    } while(result > 0);
    stream.close();
    ui->printMessage(1, "Transfer completed!\n" + path + " -> " + fullPath);
}