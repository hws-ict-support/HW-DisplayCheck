#pragma once

#include "resource.h"

#include "framework.h"
#include "HWS_DisplayPrintCheck.h"
#include <WinUser.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <iostream>
#include <fstream>
#include <WinSpool.h>
#include <atlstr.h>
#include "DpiHelper.h"
#include <filesystem>

#define MAX_LOADSTRING 100
#define INFO_BUFFER_SIZE 1024


DISPLAYCONFIG_TOPOLOGY_ID GetDisplayConf();
void setDefaultPrinter(std::string wantedPrinter);
bool fileExists(std::string szFileName);
std::string TCharToString(TCHAR* inputString);
bool DisplayMessageBox_YesNo(std::string szMessage);
void DisplayMessageBox_OnlyMessage(std::string szMessage);
void createBlankFile(std::string szFileName);