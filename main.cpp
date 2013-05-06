/* 
 * File:   main.cpp
 * Author: Aleksey Pyatkin
 *
 * Created on 02.05.2013, 15:02
 */

#include <windows.h>
#include <string.h>
#include "UserInterface.h"

int main() {
    UserInterface ui;
    string iniPath = "";
    char currentDirectory[MAX_PATH];
    char charParameter[256];
    int intParameter;
    
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    iniPath.append(currentDirectory).append("\\config.ini");
    GetPrivateProfileString("connection", "address", "127.0.0.1", charParameter, 100, iniPath.c_str());
    ui.setAddress(charParameter);
    GetPrivateProfileString("connection", "user", "anonymous", charParameter, 100, iniPath.c_str());
    ui.setUser(charParameter);
    GetPrivateProfileString("connection", "password", "", charParameter, 100, iniPath.c_str());
    ui.setPassword(charParameter);
    GetPrivateProfileString("modes", "type", "A N", charParameter, 100, iniPath.c_str());
    ui.setType(charParameter);
    GetPrivateProfileString("modes", "mode", "S", charParameter, 100, iniPath.c_str());
    ui.setMode(charParameter);
    GetPrivateProfileString("modes", "structure", "F", charParameter, 100, iniPath.c_str());
    ui.setStructure(charParameter);
    intParameter = GetPrivateProfileInt("modes", "passive", 1, iniPath.c_str());
    ui.setPassive(intParameter);
    GetPrivateProfileString("paths", "path", "", charParameter, 100, iniPath.c_str());
    ui.setPath(charParameter);
    GetPrivateProfileString("paths", "localPath", "", charParameter, 100, iniPath.c_str());
    ui.setLocalPath(charParameter);
    ui.connect();
    
    return 0;
}