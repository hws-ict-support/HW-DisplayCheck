

#include "HWS_DisplayPrintCheck.h"




void setDefaultPrinter(std::string wantedPrinter)
{

    PRINTER_INFO_2* list;
    DWORD            cnt = 0;
    DWORD            sz = 0;
    DWORD Level = 2;
    int            i;
    int            sl;

    EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, Level, NULL, 0, &sz, &cnt);

    if ((list = (PRINTER_INFO_2*)malloc(sz)) == 0)    return;

    if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, Level, (LPBYTE)list, sz, &sz, &cnt))
    {
        free(list);
        return;
    }

    for (i = 0, sl = 0; i < (int)cnt; i++)
    {
        USES_CONVERSION;
        std::string szPrinterName(T2A(list[i].pPrinterName));

        if (szPrinterName.find(wantedPrinter) != std::string::npos) {
            SetDefaultPrinter(list[i].pPrinterName);
            return;

        }
    }
}
