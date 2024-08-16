
#include "HWS_DisplayPrintCheck.h"

bool DisplayMessageBox_YesNo( std::string szMessage )
{
    std::wstring stemp = std::wstring(szMessage.begin(), szMessage.end());
    LPCWSTR sw = stemp.c_str();


    int msgboxID = MessageBox(
        NULL,
        sw,
        (LPCWSTR)L"HWS Display and Printer Check Utility.",
        MB_ICONQUESTION
        | MB_YESNO
    );

    return (msgboxID == IDYES);

}

void DisplayMessageBox_OnlyMessage(std::string szMessage)
{
    std::wstring stemp = std::wstring(szMessage.begin(), szMessage.end());
    LPCWSTR sw = stemp.c_str();


    int msgboxID = MessageBox(
        NULL,
        sw,
        (LPCWSTR)L"HELP - HWS Display and Printer Check Utility.",
        MB_ICONWARNING
        
    );

    

}



bool fileExists(std::string szFileName)
{
    std::ifstream file(szFileName);
    if (file.is_open()) {
        file.close();
        return true;
    }
    return false;

}


void createBlankFile(std::string szFileName)
{
    std::ofstream file(szFileName);
    if (file.is_open()) {
        file << "" << std::endl;
        file.close();
    }

}



std::string TCharToString(TCHAR* inputString)
{
    size_t i;
    char* pMBBuffer = (char*)malloc(INFO_BUFFER_SIZE);
    wcstombs_s(&i, pMBBuffer, (size_t)INFO_BUFFER_SIZE, inputString, (size_t)INFO_BUFFER_SIZE - 1);
    return std::string(pMBBuffer);
}