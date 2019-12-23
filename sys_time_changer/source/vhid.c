#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"

u64 con_id = 0;


int vhidNewController()
{
    Result rc;
    rc = hiddbgInitialize();
    char logMsg[50];
    int init_flag = 1;
    
    HiddbgHdlsDeviceInfo device = {0};

    if(R_FAILED(rc))
    {
        strcpy(logMsg, "Fail to create virtual controller.\n");
        logInfo(LOGFILE, logMsg);
    }
    else
        init_flag = 0;

    // Set the controller type to Pro-Controller, and set the npadInterfaceType.
    device.deviceType = HidDeviceType_FullKey3;
    device.npadInterfaceType = NpadInterfaceType_Bluetooth;
    // Set the controller colors. The grip colors are for Pro-Controller on [9.0.0+].
    device.singleColorBody = RGBA8_MAXALPHA(255,255,255);
    device.singleColorButtons = RGBA8_MAXALPHA(0,0,0);
    device.colorLeftGrip = RGBA8_MAXALPHA(230,255,0);
    device.colorRightGrip = RGBA8_MAXALPHA(0,40,20);

    if (init_flag == 0) 
    {
        rc = hiddbgAttachHdlsWorkBuffer();
        if(R_SUCCEEDED(rc))
        {
            // Attach a new virtual controller.
            rc = hiddbgAttachHdlsVirtualDevice(&con_id, &device);
            sprintf(logMsg,"hiddbgAttachHdlsVirtualDevice(): 0x%x\n", rc);
            logInfo(LOGFILE, logMsg);
        }
        else
        {
            init_flag = 1;
            strcpy(logMsg, "Failed to attach a virtual controller.\n");
            logInfo(LOGFILE, logMsg);
        }
        
    }
    return init_flag;
}


vhidSetControllerState(HiddbgHdlsState *state, char button)
{

    rc2 = hiddbgSetHdlsState(con_id, state);
    if (R_FAILED(rc2)) printf("hiddbgSetHdlsState(): 0x%x\n", rc2);

}

void vhidDetachController()
{
    hiddbgDetachHdlsVirtualDevice(con_id);
    hiddbgReleaseHdlsWorkBuffer();
    hiddbgExit();
}