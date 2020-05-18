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


void vhidPressButtonAndWait(u64 button, s64 timeout)
{
    HiddbgHdlsState state = {0};
    state.batteryCharge = 4; // Set battery charge to full.
    state.buttons = button;

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


void vhidMoveTo(char direction, int move_time)
{
    HiddbgHdlsState state = {0};

    switch (direction)
    {
        case 'U':
            state.joysticks[JOYSTICK_LEFT].dy = JOYSTICK_MAX;
            break;
        case 'D':
            state.joysticks[JOYSTICK_LEFT].dy = JOYSTICK_MIN;
            break;
        case 'L':
            state.joysticks[JOYSTICK_LEFT].dx = JOYSTICK_MIN;
            break;
        case 'R':
            state.joysticks[JOYSTICK_LEFT].dx = JOYSTICK_MAX;
            break;
    }
    Result rc = hiddbgSetHdlsState(con_id, &state);

    if (R_FAILED(rc))
        logInfo(LOGFILE, "Failed to set state for vitual controller.\n");
    else
    {
        svcSleepThread(move_time);
        state.joysticks[JOYSTICK_LEFT].dx = 0;
        state.joysticks[JOYSTICK_LEFT].dy = 0;
        hiddbgSetHdlsState(con_id, &state);
    }
}

