/* 
 * File:   ProtocolInterpreter.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 22:50
 */

#include "ProtocolInterpreter.h"

/**
 * Поточная функция для запуска передачи файлов отдельным потоком.
 * 
 * @param parameter Должен быть передан указатель на объект класса ProtocolInterpreter
 * 
 * @return 
 */
DWORD WINAPI startDTP(LPVOID parameter) {
    ProtocolInterpreter *pi = (ProtocolInterpreter*)parameter;
    pi->udtp->openConnection();
    
    return 0;
}

/**
 * Инициализация интерпретатора протокола.
 * 
 * @param ui Объект пользовательского интерфейса.
 */
ProtocolInterpreter::ProtocolInterpreter(UserInterface *ui) {
    this->ui = ui;
    this->udtp = new UserDTP(ui);
}

/**
 * Установка локального пути для сохранения файлов, получаемых от сервера.
 * 
 * @param path Путь.
 */
void ProtocolInterpreter::setLocalPath(string path) {
    this->localPath = path;
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
 * Установка режима передачи файлов.
 * 
 * @param mode Режим передачи файлов.
 */
void ProtocolInterpreter::setMode(string mode) {
    this->mode = mode;
}

/**
 * Установка структуры файлов.
 * 
 * @param structure Структура файла
 */
void ProtocolInterpreter::setStructure(string structure) {
    this->structure = structure;
}

/**
 * Установка пути к файлу.
 * 
 * @param path Путь.
 */
void ProtocolInterpreter::setPath(string path) {
    this->path = path;
}

/**
 * Установить данные для команды PORT.
 * 
 * @param portData Данные для команды PORT (IP-адрес и порт).
 */
void ProtocolInterpreter::setPortData(string portData) {
    this->portData = portData;
}

/**
 * Установка флага использования пассивного режима.
 * 
 * @param passive Флаг (0 - активный режима, другое - пассивный).
 */
void ProtocolInterpreter::setPassive(int passive) {
    this->passive = passive;
}

/**
 * Получение номера динамического порта из 227 отклика команды PASV.
 * 
 * @return Номер порта.
 */
int ProtocolInterpreter::getPort() {
    char *symbol;
    char num[5];
    int i;
    int port = 0;
    
    while (symbol = strstr(replyBuffer, ",")) {
        *symbol = '.';
    }
    // Получение первого числа
    symbol = strstr(replyBuffer, address.c_str()) + address.length() + 1;
    i = 0;
    while (*symbol != '.') {
        num[i] = *symbol;
        i++;
        symbol++;
    }
    num[i] = 0;
    port = atoi(num)*256;
    // Получение второго числа
    i = 0;
    symbol++;
    while (*symbol == '0' || *symbol == '1' || *symbol == '2' || *symbol == '3' ||
            *symbol == '4' || *symbol == '5' || *symbol == '6' || *symbol == '7' ||
            *symbol == '8' || *symbol == '9') {
        num[i] = *symbol;
        i++;
        symbol++;
    }
    num[i] = 0;
    port += atoi(num);
    
    return port;
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
    } else if (command == "MODE") {
        sendMode();
    } else if (command == "STRU") {
        sendStru();
    } else if (command == "PORT") {
        sendPort();
    } else if (command == "PASV") {
        sendPasv();
    } else if (command == "RETR") {
        sendRetr();
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
 * Отправка команды MODE.
 */
void ProtocolInterpreter::sendMode() {
    commandBuffer = "MODE " + mode + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "MODE sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды STRU.
 */
void ProtocolInterpreter::sendStru() {
    commandBuffer = "STRU " + structure + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "STRU sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды PORT.
 */
void ProtocolInterpreter::sendPort() {
    commandBuffer = "PORT " + portData + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "PORT sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды PASV.
 */
void ProtocolInterpreter::sendPasv() {
    commandBuffer = "PASV\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "PASV sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
}

/**
 * Отправка команды RETR.
 */
void ProtocolInterpreter::sendRetr() {
    udtp->setAddress(address);
    udtp->setPath(path);
    udtp->setLocalPath(localPath);
    udtp->setPassive(passive);
    if (passive) {
        udtp->setPort(getPort());
        udtp->openConnection();
    } else {
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    commandBuffer = "RETR " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "RETR sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
    if (strstr(replyBuffer, "425 ")) {
        if (passive) {
            udtp->closeConnection();
            ui->printMessage(1, "Passive mode using failed! Try active mode.");
            ui->printMessage(0, "PORT " + portData + "\n");
            sendCommand("PORT");
            passive = 0;
            ui->setPassive(passive);
            udtp->setPassive(passive);
            connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
            ui->printMessage(0, "RETR " + path + "\n");
            sendCommand("RETR");
        } else {
            TerminateThread(connection, 1);
            CloseHandle(connection);
            ui->printMessage(1, "Active mode using failed! Try passive mode.");
            ui->printMessage(0, "PASV\n");
            sendCommand("PASV");
            passive = 1;
            ui->setPassive(passive);
            udtp->setPassive(passive);
            udtp->setPort(getPort());
            udtp->openConnection();
            ui->printMessage(0, "RETR " + path + "\n");
            sendCommand("RETR");
        }
    } else {
        udtp->retrieve();
        printReply();
    }
    if (passive) {
        udtp->closeConnection();
    } else {
        CloseHandle(connection);
    }
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