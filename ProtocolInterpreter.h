/* 
 * File:    ProtocolInterpreter.h
 * Author:  Aleksey Pyatkin
 *
 * Created on 2 Май 2013 г., 15:37
 */

#pragma once

#include <string.h>
#include <winsock2.h>
#include "UserInterface.h"

using namespace std;

class ProtocolInterpreter {
    public:
        ProtocolInterpreter(UserInterface ui, string address, string user);
        void ftpConnect();
        
    private:
        static const int MAX_BUF_LEN = 4096;
        UserInterface ui;
        string address;
        string user;
        SOCKET connectionSocket;
        char buf[MAX_BUF_LEN];
        int result;
        
        void authorize();
};