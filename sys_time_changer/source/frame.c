// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"


void frameForward(u64 *cur_day)
{
    char logMsg[32];
    Result rc;

    *cur_day += 86400;  // one day equals to 86400 seconds.
    rc = timeSetCurrentTime(TimeType_LocalSystemClock, *cur_day);
    if (R_FAILED(rc))
    {
        strcpy(logMsg, "Unable to set local time.\n");
        logInfo(LOGFILE, logMsg);
    }
}

int frameGetNumber()
{
    u64 kDown;
    int sum_up = 0;
    int cur_digit = 0;
    do
    {
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if(kDown & KEY_DUP)
            cur_digit = ++cur_digit % 10;
        else if(kDown & KEY_DDOWN)
            cur_digit = (10 + --cur_digit) % 10;
        else if(kDown & KEY_DRIGHT)
        {
            sum_up = cur_digit + sum_up*10;
            cur_digit = 0;
        }
        else if(kDown & KEY_DLEFT)
        {
            sum_up /= 10;
            cur_digit = 0;
        }
    } while (!(kDown & KEY_A));
    
    return sum_up;
}

bool isWanted(HidControllerID con_id)
{
    u64 kDown;

    while(1)
    {
        hidScanInput();
        kDown = hidKeysDown(con_id);
        
        if(kDown & KEY_DUP)
            return true;
        else if(kDown & KEY_DDOWN)
            return false;
    }
}

int frameSL(u64 *cur_day)
{
    int i;
    int cnt=0;
    HiddbgHdlsState state={0};
    HidControllerID con_id = hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1;
    
    // Setup controller state.
    state.batteryCharge = 4; // Set battery charge to full.

    while(1)
    {
        cnt ++;
        for(i=0; i<3; i++)
            frameForward(cur_day);
        if(isWanted(con_id))
            break;
    }
    return cnt;
}
