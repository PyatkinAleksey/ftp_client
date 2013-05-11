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
ProtocolInterpreter::ProtocolInterpreter() {
    this->service = new Service();
    this->udtp = new UserDTP();
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
 * Установка нового имени файла для команды переименования.
 * 
 * @param newPath Новое имя файла.
 */
void ProtocolInterpreter::setNewPath(string newPath) {
    this->newPath = newPath;
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
        service->printMessage(2, "Connection failed!");
        return;
    }
    // Получение IP-адреса по имени хоста, если требуется
    hostEntry = gethostbyname(address.c_str());
    address = inet_ntoa(*(struct in_addr*)*hostEntry->h_addr_list);
    // Создание сокета
    connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectionSocket == INVALID_SOCKET) {
        service->printMessage(2, "Invalid socket!");
        WSACleanup();
        return;
    }
    // Открытие управляющего соединения
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr(address.c_str());
    clientAddress.sin_port = htons(21);
    result = connect(connectionSocket, (SOCKADDR*)&clientAddress, sizeof(clientAddress));
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "Connection error!");
        result = closesocket(connectionSocket);
        if (result == SOCKET_ERROR) {
            service->printMessage(2, "Close socket error!");
        }
        WSACleanup();
        return;
    }
    // Получение отклика-приветствия
    do {
        result = recv(connectionSocket, replyBuffer, MAX_BUF_LEN, 0);
        if (result > 0) {
            service->printMessage(0, replyBuffer);
            if (strstr(replyBuffer, "220 ")) {
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
    string code = "000 ";
    
    memset(replyBuffer, 0, MAX_BUF_LEN);
    do {
        result = recv(connectionSocket, replyBuffer, MAX_BUF_LEN, 0);
        if (result > 0) {
            if (code == "000 ") {
                code[0] = replyBuffer[0];
                code[1] = replyBuffer[1];
                code[2] = replyBuffer[2];
            }
            service->printMessage(0, replyBuffer);
            if (strstr(replyBuffer, code.c_str())) {
                break;
            }
        }
    } while(result > 0);
}

/**
 * Отправка команды FTP-серверу.
 * 
 * @param command FTP-команда.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendCommand(string command) {
    int success;
    
    if (command == "USER") {
        success = sendUser();
    } else if (command == "PASS") {
        success = sendPass();
    } else if (command == "TYPE") {
        success = sendType();
    } else if (command == "MODE") {
        success = sendMode();
    } else if (command == "STRU") {
        success = sendStru();
    } else if (command == "PORT") {
        success = sendPort();
    } else if (command == "PASV") {
        success = sendPasv();
    } else if (command == "NLST") {
        success = sendNlst();
    } else if (command == "LIST") {
        success = sendList();
    } else if (command == "RETR") {
        success = sendRetr();
    } else if (command == "STOR") {
        success = sendStor();
    } else if (command == "RNTO") {
        success = sendRnto();
    } else if (command == "RNFR") {
        success = sendRnfr();
    } else if (command == "DELE") {
        success = sendDele();
    } else if (command == "MKD") {
        success = sendMkd();
    } else if (command == "RMD") {
        success = sendRmd();
    } else if (command == "CWD") {
        success = sendCwd();
    } else if (command == "CDUP") {
        success = sendCdup();
    } else if (command == "PWD") {
        success = sendPwd();
    } else if (command == "ABOR") {
        success = sendAbor();
    } else if (command == "REIN") {
        success = sendRein();
    } else if (command == "QUIT") {
        success = sendQuit();
    } else if (command == "SYST") {
        success = sendSyst();
    } else if (command == "STAT") {
        success = sendStat();
    } else if (command == "NOOP") {
        success = sendNoop();
    } else {
        service->printMessage(1, "Unknown command!");
    }
    
    return success;
}

/**
 * Отправка команды USER.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, 1 - требуется дополнительная
 * информация в качестве пароля, другое - успешно).
 */
int ProtocolInterpreter::sendUser() {
    service->printMessage(0, "USER " + user + "\n");
    commandBuffer = "USER " + user + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "USER sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "331 ")) { // Требуется ввод дополнительной информации в качестве пароля (например, E-Mail-адрес)
        return 1;
    } else if (strstr(replyBuffer, "230 ")) {
        return 2;
    } else {
        return 0;
    }
}

/**
 * Отправка команды PASS.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendPass() {
    service->printMessage(0, "PASS " + password + "\n");
    commandBuffer = "PASS " + password + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "Additional information sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "530 ")) { // Превышено максимальное кол-во возможных соединений
        exit(1);
    } else {
        return 1;
    }
}

/**
 * Отправка команды TYPE.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendType() {
    service->printMessage(0, "TYPE " + type + "\n");
    commandBuffer = "TYPE " + type + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "TYPE sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "200 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды MODE.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendMode() {
    service->printMessage(0, "MODE " + mode + "\n");
    commandBuffer = "MODE " + mode + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "MODE sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "200 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды STRU.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendStru() {
    service->printMessage(0, "STRU " + structure + "\n");
    commandBuffer = "STRU " + structure + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "STRU sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "200 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды PORT.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendPort() {
    setPortData();
    service->printMessage(0, "PORT " + portData + "\n");
    commandBuffer = "PORT " + portData + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "PORT sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "200 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды PASV.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendPasv() {
    service->printMessage(0, "PASV\n");
    commandBuffer = "PASV\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "PASV sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "227 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды NLST.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendNlst() {
    if (path == "#") {
        commandBuffer = "NLST\r\n";
        service->printMessage(0, "NLST\n");
    } else {
        commandBuffer = "NLST " + path + "\r\n";
        service->printMessage(0, "NLST " + path + "\n");
    }
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "NLST sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    udtp->setPassive(passive);
    udtp->setAddress(address);
    if (passive) {
        if (strstr(replyBuffer, "227 ")) {
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
        } else {
            service->printMessage(2, "You should run PASV command first.");
            return 0;
        }
    } else {
        udtp->setPort(port);
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    udtp->setPath(path);
    fileList = udtp->fileList();
    printReply();
    printReply();
    if (strstr(replyBuffer, "226 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды LIST.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendList() {
    if (path == "#") {
        commandBuffer = "LIST\r\n";
        service->printMessage(0, "LIST\n");
    } else {
        commandBuffer = "LIST " + path + "\r\n";
        service->printMessage(0, "LIST " + path + "\n");
    }
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "LIST sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    udtp->setPassive(passive);
    udtp->setAddress(address);
    if (passive) {
        if (strstr(replyBuffer, "227 ")) {
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
        } else {
            service->printMessage(2, "You should run PASV command first.");
            return 0;
        }
    } else {
        udtp->setPort(port);
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    udtp->setPath(path);
    udtp->fullList();
    printReply();
    printReply();
    if (strstr(replyBuffer, "226 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды RETR.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendRetr() {
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
            service->printMessage(2, "You should run PASV command first.");
            return 0;
        }
    } else {
        udtp->setPort(port);
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    service->printMessage(0, "RETR " + path + "\n");
    commandBuffer = "RETR " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "RETR sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "425 ")) {
        if (passive) {
            udtp->closeConnection();
            service->printMessage(1, "Passive mode using failed! Try active mode.");
            sendCommand("PORT");
            passive = 0;
            udtp->setPassive(passive);
            udtp->setPort(port);
            connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
            sendCommand("RETR");
        } else {
            TerminateThread(connection, 1);
            CloseHandle(connection);
            service->printMessage(1, "Active mode using failed! Try passive mode.");
            sendCommand("PASV");
            passive = 1;
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
    if (strstr(replyBuffer, "226 ") || strstr(replyBuffer, "250 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды STOR.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendStor() {
    udtp->setAddress(address);
    udtp->setPath(path);
    udtp->setPassive(passive);
    if (passive) {
        if (strstr(replyBuffer, "227 ")) {
            setPort();
            udtp->setPort(port);
            udtp->openConnection();
        } else {
            service->printMessage(2, "You should run PASV command first.");
            return 0;
        }
    } else {
        udtp->setPort(port);
        connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
    }
    service->printMessage(0, "STOR " + path + "\n");
    commandBuffer = "STOR " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "STOR sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "550 ") || strstr(replyBuffer, "553 ")) { // Отсутствуют права
        return 0;
    } else if (strstr(replyBuffer, "425 ")) {
        if (passive) {
            udtp->closeConnection();
            service->printMessage(1, "Passive mode using failed! Try active mode.");
            sendCommand("PORT");
            passive = 0;
            udtp->setPassive(passive);
            udtp->setPort(port);
            connection = CreateThread(NULL, 0, startDTP, this, 0, NULL);
            sendCommand("STOR");
        } else {
            TerminateThread(connection, 1);
            CloseHandle(connection);
            service->printMessage(1, "Active mode using failed! Try passive mode.");
            sendCommand("PASV");
            passive = 1;
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
    if (strstr(replyBuffer, "226 ") || strstr(replyBuffer, "250 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды RNTO.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendRnto() {
    service->printMessage(0, "RNTO " + newPath + "\n");
    commandBuffer = "RNTO " + newPath + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "RNTO sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "250 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды RNFR.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendRnfr() {
    service->printMessage(0, "RNFR " + path + "\n");
    commandBuffer = "RNFR " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "RNFR sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "350 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды DELE.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendDele() {
    service->printMessage(0, "DELE " + path + "\n");
    commandBuffer = "DELE " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "DELE sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "250 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды MKD.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendMkd() {
    service->printMessage(0, "MKD " + path + "\n");
    commandBuffer = "MKD " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "MKD sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "257 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды RMD.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendRmd() {
    service->printMessage(0, "RMD " + path + "\n");
    commandBuffer = "RMD " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "RMD sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "250 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды CWD.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendCwd() {
    service->printMessage(0, "CWD " + path + "\n");
    commandBuffer = "CWD " + path + "\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "CWD sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "250 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды CDUP.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendCdup() {
    service->printMessage(0, "CDUP\n");
    commandBuffer = "CDUP\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "CDUP sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "200 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды PWD.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendPwd() {
    service->printMessage(0, "PWD\n");
    commandBuffer = "PWD\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "PWD sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "257 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды ABOR.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendAbor() {
    service->printMessage(0, "ABOR\n");
    commandBuffer = "ABOR\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "ABOR sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "426 ")) { // Ненормальное завершение команды (вернется еще один отклик)
        printReply();
    }
    if (strstr(replyBuffer, "225 ") || strstr(replyBuffer, "226 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды REIN.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendRein() {
    service->printMessage(0, "REIN\n");
    commandBuffer = "REIN\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "REIN sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "220 ") || strstr(replyBuffer, "120 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды QUIT.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendQuit() {
    service->printMessage(0, "QUIT\n");
    commandBuffer = "QUIT\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "QUIT sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "221 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды SYST.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendSyst() {
    service->printMessage(0, "SYST\n");
    commandBuffer = "SYST\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "SYST sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "215 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды STAT.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendStat() {
    if (path == "#") {
        service->printMessage(0, "STAT\n");
        commandBuffer = "STAT\r\n";
    } else {
        service->printMessage(0, "STAT " + path + "\n");
        commandBuffer = "STAT " + path + "\r\n";
    }
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "STAT sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "211 ") || strstr(replyBuffer, "212 ") || strstr(replyBuffer, "213 ")) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Отправка команды NOOP.
 * 
 * @return Флаг успешности выполнения (0 - не успешно, другое - успешно).
 */
int ProtocolInterpreter::sendNoop() {
    service->printMessage(0, "NOOP\n");
    commandBuffer = "NOOP\r\n";
    result = send(connectionSocket, commandBuffer.c_str(), commandBuffer.length(), 0);
    if (result == SOCKET_ERROR) {
        service->printMessage(2, "NOOP sending error!");
        closesocket(connectionSocket);
        WSACleanup();
        return 0;
    }
    printReply();
    if (strstr(replyBuffer, "200 ")) {
        return 1;
    } else {
        return 0;
    }
}

ProtocolInterpreter::~ProtocolInterpreter() {
    delete service;
    delete udtp;
}