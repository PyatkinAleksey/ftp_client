/* 
 * File:   UserDTP.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 04.05.2013, 2:01
 */

#include <winsock2.h>
#include "UserDTP.h"

/**
 * Инициализация пользовательского процесса передачи данных.
 */
UserDTP::UserDTP() {
    service = new Service();
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
        closeConnection();
    }
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) {
        service->printMessage(2, "Connection failed!");
        return;
    }
    dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (dataSocket == INVALID_SOCKET) {
        service->printMessage(2, "Invalid socket!");
        WSACleanup();
        return;
    }
    dataAddress.sin_family = AF_INET;
    if (passive) {
        dataAddress.sin_addr.s_addr = inet_addr(address.c_str());
        dataAddress.sin_port = htons(port);
        result = connect(dataSocket, (SOCKADDR*)&dataAddress, sizeof(dataAddress));
        if (result == SOCKET_ERROR) {
            service->printMessage(2, "Socket error!");
            result = closesocket(dataSocket);
            if (result == SOCKET_ERROR) {
                service->printMessage(2, "Socket error!");
            }
            WSACleanup();
            return;
        }
        connected = 1;
    } else {
        dataAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        dataAddress.sin_port = htons(port);
        result = bind(dataSocket, (SOCKADDR*)&dataAddress, sizeof(dataAddress));
        if (result == SOCKET_ERROR) {
            service->printMessage(2, "Socket error!");
            result = closesocket(dataSocket);
            if (result == SOCKET_ERROR) {
                service->printMessage(2, "Socket error!");
            }
            WSACleanup();
            return;
        }
        if (listen(dataSocket, SOMAXCONN) == SOCKET_ERROR) {
            service->printMessage(2, "Socket error!");
            closesocket(dataSocket);
            WSACleanup();
            return;
        }
        acceptSocket = accept(dataSocket, NULL, NULL);
        if (acceptSocket == INVALID_SOCKET) {
            service->printMessage(2, "Invalid accept socket!");
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
    connected = 0;
}

/**
 * Получить список имен файлов и директорий.
 * 
 * @return Массив имен файлов и директорий.
 */
list<string> UserDTP::fileList() {
    list<string> filelist;
    
    do {
        memset(buffer, 0, 4096);
        result = recv(dataSocket, buffer, 4096, 0);
        if (result > 0) {
            filelist.push_back(buffer);
        }
    } while (result > 0);
    
    return filelist;
}

/**
 * Получение списка файлов и директорий.
 */
void UserDTP::fullList() {
    do {
        memset(buffer, 0, 4096);
        result = recv(dataSocket, buffer, 4096, 0);
        if (result > 0) {
            service->printMessage(0, buffer);
        }
    } while (result > 0);
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
                service->printMessage(0, buffer);
            }
            stream << buffer;
        }
    } while(result > 0);
    stream.close();
    service->printMessage(1, "Transfer completed!\n" + path + " -> " + fullPath);
}

/**
 * Выполнение передачи файлов на сервер.
 */
void UserDTP::store() {
    ifstream stream;
    string buffer;
    
    stream.open(path.c_str(), ifstream::in);
    while (getline(stream, buffer)) {
        buffer.append("\n");
        result = send(dataSocket, buffer.c_str(), buffer.length(), 0);
        if (result <= 0) {
            service->printMessage(2, "Transfer error!");
            return;
        }
    }
    stream.close();
    closeConnection();
    service->printMessage(1, "Transfer sompleted!");
}

UserDTP::~UserDTP() {
    delete service;
}