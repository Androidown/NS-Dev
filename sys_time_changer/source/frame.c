// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"
#include "vhid.h"

void frameForward(u64 *cur_day)
{
    Result rc;

    vhidPressButtonAndWait('A', 2E+9L); // start local connection
    *cur_day += 86400;  // one day equals to 86400 seconds.
    rc = timeSetCurrentTime(TimeType_LocalSystemClock, *cur_day);
    if (R_FAILED(rc))
        logInfo(LOGFILE, "Unable to set local time.\n");
    vhidPressButtonAndWait('B', 1E+9L); // cancel local connection
    vhidPressButtonAndWait('A', 2E+9L); // confirm cancel and return to wild area
    vhidPressButtonAndWait('A', 2E+9L); // comunicate with raid
    vhidPressButtonAndWait('A', 1E+9L); // confirm get watt
    vhidPressButtonAndWait('A', 1E+9L); // show raid detail
}

int frameGetNumber()
{
    u64 kDown;
    int sum_up = 0;
    int cur_digit = 0;
    while(appletMainLoop())
    {
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if(kDown & KEY_DUP)
            cur_digit = (cur_digit + 1) % 10;
        else if(kDown & KEY_DDOWN)
            cur_digit = (9 + cur_digit) % 10;
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
        else if(kDown & KEY_A)
        {
            sum_up = cur_digit + sum_up*10;
            break;
        }
        svcSleepThread(1E+7L);
    }
    
    return sum_up;
}


int frameGetNumberFromConfig()
{
    char* conf_path = "/config/frame_walker";
    int frame_num = 0;
    char logMsg[32];

    DIR* files = opendir(conf_path);
    struct dirent* hFile;

    if (files) 
    {
        while ((hFile = readdir(files)) != NULL) 
        {
            if(hFile->d_name[0] == '.') continue;

            if(strstr( hFile->d_name, "frame."))
            {
                char* raw_num = hFile->d_name + 6;
                frame_num = strtol(raw_num, NULL, 10);      
                snprintf(logMsg, 32, "Get frame number %d.\n", frame_num);
                logInfo(LOGFILE, logMsg);
                break;
            }
        }
        closedir( files );
    }

    if(!frame_num) logInfo(LOGFILE, "Failed to get frame number.\n");

    return frame_num;
}


bool isWanted()
{
    u64 kDown;
    HidControllerID con_id = hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1;

    while(appletMainLoop())
    {
        hidScanInput();
        kDown = hidKeysDown(con_id);
        
        if(kDown & KEY_DUP)
            return true;
        else if(kDown & KEY_DDOWN)
            return false;

        svcSleepThread(1E+7L);
    }
}

int frameSL(u64 *cur_day)
{
    int i;
    int cnt=0;

    while(appletMainLoop())
    {
        cnt ++;
        for(i=0; i<3; i++)
            frameForward(cur_day);
        if(isWanted())
            break;
        else // restart game
        {
            vhidPressButtonAndWait('H', 1E+9L);
            vhidPressButtonAndWait('X', 1E+9L); //close game
            vhidPressButtonAndWait('A', 1E+9L); //confirm close
            vhidPressButtonAndWait('A', 1E+9L); //open game
            vhidPressButtonAndWait('A', 15E+9L); //confirm user and wait to title scene
            vhidPressButtonAndWait('A', 6E+9L); //start and wait for loading save data
            vhidPressButtonAndWait('A', 2E+9L); //comunicate with raid
            vhidPressButtonAndWait('A', 1E+9L); //confirm get watt
            vhidPressButtonAndWait('A', 1E+9L); //show raid detail
        }
        
        svcSleepThread(1E+8L);
    }
    return cnt;
}


void frameSave()
{
    vhidPressButtonAndWait('B', 1E+9L); // cancel local connection
    vhidPressButtonAndWait('A', 2E+9L); // confirm cancel and return to wild area
    vhidPressButtonAndWait('X', 2E+9L); // open menu
    vhidPressButtonAndWait('R', 1E+9L); // shortcut to save
    vhidPressButtonAndWait('A', 2E+9L); // confirm save
    vhidPressButtonAndWait('B', 2E+9L); // exit from save scene
    vhidPressButtonAndWait('B', 2E+9L); // close menu
    vhidPressButtonAndWait('A', 2E+9L); // comunicate with raid
    vhidPressButtonAndWait('A', 1E+9L); // confirm get watt
    vhidPressButtonAndWait('A', 1E+9L); // show raid detail
}
