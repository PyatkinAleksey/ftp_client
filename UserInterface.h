/* 
 * File:   UserInterface.h
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 15:01
 */

#pragma once

#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

class UserInterface {
    public:
        UserInterface();
        void connect();
        void printMessage(int type, string message);

    private:
        string address;
        string user;
        void setAddress();
        void setUser();
};