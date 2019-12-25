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
    int init_flag = 1;
    
    HiddbgHdlsDeviceInfo device = {0};

    if(R_FAILED(rc))
        logInfo(LOGFILE, "Fail to create virtual controller.\n");
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
            hiddbgAttachHdlsVirtualDevice(&con_id, &device);
            logInfo(LOGFILE, "Attached a new v-controller.\n");
        }
        else
        {
            init_flag = 1;
            logInfo(LOGFILE, "Failed to attach a virtual controller.\n");
        }
        
    }
    return init_flag;
}


void vhidPressButtonAndWait(char button, s64 timeout)
{
    HiddbgHdlsStateV7 state = {0};
    state.batteryCharge = 4; // Set battery charge to full.

    switch (button)
    {
    case 'A':
        state.buttons = KEY_A;
        break;
    case 'B':
        state.buttons = KEY_B;
        break;
    case 'X':
        state.buttons = KEY_X;
        break;
    case 'Y':
        state.buttons = KEY_Y;
        break;
    case 'H':
        state.buttons = KEY_HOME;
        break;
    case 'R':
        state.buttons = KEY_R;
        break;
    default:
        break;
    }

    Result rc2 = hiddbgSetHdlsState(con_id, &state);
    if (R_FAILED(rc2))
        logInfo(LOGFILE, "Failed to set state for vitual controller.\n");
    else
    {
        svcSleepThread(2E+8L);
        state.buttons = 0;
        hiddbgSetHdlsState(con_id, &state);
    }
    svcSleepThread(timeout);
}


void vhidDetachController()
{
    hiddbgDetachHdlsVirtualDevice(con_id);
    hiddbgReleaseHdlsWorkBuffer();
    hiddbgExit();
}
