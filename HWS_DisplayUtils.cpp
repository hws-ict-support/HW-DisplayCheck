
#include "HWS_DisplayPrintCheck.h"


DISPLAYCONFIG_TOPOLOGY_ID GetDisplayConf()
{
    UINT32 PathArraySize = 0;
    UINT32 ModeArraySize = 0;
    DISPLAYCONFIG_PATH_INFO* PathArray;
    DISPLAYCONFIG_MODE_INFO* ModeArray;
    DISPLAYCONFIG_TOPOLOGY_ID CurrentTopology;

    GetDisplayConfigBufferSizes(QDC_ALL_PATHS, &PathArraySize, &ModeArraySize);

    PathArray = (DISPLAYCONFIG_PATH_INFO*)malloc(PathArraySize * sizeof(DISPLAYCONFIG_PATH_INFO));
    memset(PathArray, 0, PathArraySize * sizeof(DISPLAYCONFIG_PATH_INFO));

    ModeArray = (DISPLAYCONFIG_MODE_INFO*)malloc(ModeArraySize * sizeof(DISPLAYCONFIG_MODE_INFO));
    memset(ModeArray, 0, ModeArraySize * sizeof(DISPLAYCONFIG_MODE_INFO));

    LONG ret = QueryDisplayConfig(QDC_DATABASE_CURRENT, &PathArraySize, PathArray, &ModeArraySize, ModeArray, &CurrentTopology);
    // Above CurrentTopology variable will aquire the current display setting (ie Extend, Duplicate etc)

    free(PathArray);
    free(ModeArray);

    return CurrentTopology;
}