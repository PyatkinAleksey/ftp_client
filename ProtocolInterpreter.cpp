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
 */
void ProtocolInterpreter::setPortData() {
    WSADATA wsaData;
    char hostName[255];
    struct hostent *hostEntry;
    char *ip;
    int p1, p2;
    char port1[5], port2[5];
    
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    gethostname(hostName, 255);
    hostEntry = gethostbyname(hostName);
    ip = inet_ntoa(*(struct in_addr*)*hostEntry->h_addr_list);
    while (strstr(ip, ".")) {
        *(strstr(ip, ".")) = ',';
    }
    portData = "";
    portData.append(ip);
    srand(time(NULL));
    port = rand()%16382 + 49152; // Случайное число в диапазоне 49152-65534
    p2 = port % 256;
    p1 = (port - p2) / 256;
    itoa(p1, port1, 10);
    itoa(p2, port2, 10);
    portData.append(",");
    portData.append(port1);
    portData.append(",");
    portData.append(port2);
    WSACleanup();
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
 * Получение и установка номера динамического порта из 227 отклика команды PASV.
 * 
 * @return Номер порта.
 */
void ProtocolInterpreter::setPort() {
    char *symbol;
    char num[5];
    int i;

    port = 0;
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
}

/**
 * Открытие управляющего соединения.
 */
void ProtocolInterpreter::openControlConnection() {
    WORD wVersionRequested;
    WSADATA wsaData;
    sockaddr_in clientAddress;
    struct hostent *hostEntry;
    
    wVersionRequested = MAKEWORD(2, 2);
    result = WSAStartup(wVersionRequested, &wsaData);
    if (result != NO_ERROR) {
        ui->printMessage(2, "Connection failed!");
        return;
    }
    // Получение IP-адреса по имени хоста, если требуется
    hostEntry = gethostbyname(address.c_str());
    address = inet_ntoa(*(struct in_addr*)*hostEntry->h_addr_list);
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
    } else if (command == "STOR") {
        sendStor();
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
    ui->printMessage(0, "USER " + user + "\n");
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
    ui->printMessage(0, "PASS " + password + "\n");
    commandBuffer = "PASS " + password + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "Additional information sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
    if (strstr(replyBuffer, "530 ")) { // Превышено максимальное кол-во возможных соединений
        exit(1);
    }
}

/**
 * Отправка команды TYPE.
 */
void ProtocolInterpreter::sendType() {
    ui->printMessage(0, "TYPE " + type + "\n");
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
    ui->printMessage(0, "MODE " + mode + "\n");
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
    ui->printMessage(0, "STRU " + structure + "\n");
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
    setPortData();
    ui->printMessage(0, "PORT " + portData + "\n");
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
    ui->printMessage(0, "PASV\n");
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
        if (strstr(replyBuffer, "227 ")) {
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
        } else {
            ui->printMessage(2, "You should run PASV command first.");
            return;
        }
    } else {
        udtp->setPort(port);
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    ui->printMessage(0, "RETR " + path + "\n");
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
            sendCommand("PORT");
            passive = 0;
            ui->setPassive(passive);
            udtp->setPassive(passive);
            udtp->setPort(port);
            connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
            sendCommand("RETR");
        } else {
            TerminateThread(connection, 1);
            CloseHandle(connection);
            ui->printMessage(1, "Active mode using failed! Try passive mode.");
            sendCommand("PASV");
            passive = 1;
            ui->setPassive(passive);
            udtp->setPassive(passive);
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
            sendCommand("RETR");
        }
    } else {
        if (!strstr(replyBuffer, "550 ")) {
            udtp->retrieve();
            printReply();
        }
    }
    if (passive) {
        udtp->closeConnection();
    }
}

/**
 * Отправка команды STOR.
 */
void ProtocolInterpreter::sendStor() {
    udtp->setAddress(address);
    udtp->setPath(path);
    udtp->setPassive(passive);
    if (passive) {
        if (strstr(replyBuffer, "227 ")) {
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
        } else {
            ui->printMessage(2, "You should run PASV command first.");
            return;
        }
    } else {
        udtp->setPort(port);
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    ui->printMessage(0, "STOR " + path + "\n");
    commandBuffer = "STOR " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        ui->printMessage(2, "STOR sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return;
    }
    printReply();
    if (strstr(replyBuffer, "550 ") || strstr(replyBuffer, "553 ")) { // Отсутствуют права
        return;
    } else if (strstr(replyBuffer, "425 ")) {
        if (passive) {
            udtp->closeConnection();
            ui->printMessage(1, "Passive mode using failed! Try active mode.");
            sendCommand("PORT");
            passive = 0;
            ui->setPassive(passive);
            udtp->setPassive(passive);
            udtp->setPort(port);
            connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
            sendCommand("STOR");
        } else {
            TerminateThread(connection, 1);
            CloseHandle(connection);
            ui->printMessage(1, "Active mode using failed! Try passive mode.");
            sendCommand("PASV");
            passive = 1;
            ui->setPassive(passive);
            udtp->setPassive(passive);
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
            sendCommand("STOR");
        }
    } else {
        udtp->store();
        printReply();
    }
}

/**
 * Отправка команды QUIT.
 */
void ProtocolInterpreter::sendQuit() {
    ui->printMessage(0, "QUIT\n");
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
    ui->printMessage(0, "NOOP\n");
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