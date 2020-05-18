// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"
#include "vhid.h"
#include "frame.h"

void frameForward(u64 *cur_day)
{
    Result rc;

    vhidPressButtonAndWait('A', 30E+8L); // start local connection
    *cur_day += 86400;  // one day equals to 86400 seconds.
    rc = timeSetCurrentTime(TimeType_LocalSystemClock, *cur_day);
    if (R_FAILED(rc))
        logInfo(LOGFILE, "Unable to set local time.\n");
    vhidPressButtonAndWait('B', 5E+8L); // cancel local connection
    vhidPressButtonAndWait('A', 40E+8L); // confirm cancel and return to wild area
    vhidPressButtonAndWait('A', 4E+8L); // comunicate with raid
    vhidPressButtonAndWait('A', 4E+8L); //confirm msg
    vhidPressButtonAndWait('A', 10E+8L); //confirm get watt and show raid detail
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


int _cmp(const void* a, const void* b)
{
    return *(int*)b - *(int*)a;
}


int frameReadConfig(int* frames, const char* prefix, bool single)
{
    char* conf_path = "/config/frame_walker";
    int read_num = 0, len_frames = 0;
    char logMsg[32];
    
    DIR* files = opendir(conf_path);
    struct dirent* hFile;

    if(files) 
    {
        while ((hFile = readdir(files)) != NULL) 
        {
            if(hFile->d_name[0] == '.') continue;

            if(strstr( hFile->d_name, prefix))
            {
                char* raw_num = hFile->d_name + 6;
                read_num = strtol(raw_num, NULL, 10);      
                snprintf(logMsg, 32, "Get config %s%d.\n", prefix, read_num);
                logInfo(LOGFILE, logMsg);
                frames[len_frames++] = read_num;
                if(single) break;
            }
        }
        closedir(files);
    }

    if(!read_num) 
        logInfo(LOGFILE, "Failed to read config.\n");
    else
        qsort(frames, len_frames, sizeof(int), _cmp);
    
    return len_frames;
}


bool _isWantedByHand()
{
    u64 kDown;
    bool is_wanted = false;
    HidControllerID con_id = hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1;

    while(appletMainLoop())
    {
        hidScanInput();
        kDown = hidKeysDown(con_id);
        
        if(kDown & KEY_DUP)
        {
            is_wanted = true;
            logInfo(LOGFILE, "Frame accepted.\n");
            break;
        }
        else if(kDown & KEY_DDOWN)
        {
            is_wanted = false;
            logInfo(LOGFILE, "Frame rejected, continue...\n");
            break;
        }
        else
            svcSleepThread(1E+7L);
    }
    return is_wanted;
}


int frameSL(u64 *cur_day)
{
    int i, cnt=0;

    while(appletMainLoop())
    {
        cnt ++;
        for(i=0; i<3; i++)
            frameForward(cur_day);
        if(_isWantedByHand()) break;
        else if(R_FAILED(frameSimpleRestart())) break;    
        svcSleepThread(1E+8L);
    }
    return cnt;
}


void frameSave()
{
    vhidPressButtonAndWait('B', 2E+9L); // cancel and return to wild area
    vhidPressButtonAndWait('X', 2E+9L); // open menu
    vhidPressButtonAndWait('R', 1E+9L); // shortcut to save
    vhidPressButtonAndWait('A', 3E+9L); // confirm save
    vhidPressButtonAndWait('A', 2E+9L); // show raid detail
}


Result _frameRestartImpl(void (*func)(void))
{
    Result rc;
    u64 PMSW_TID = 0x0100abf008968000;
    nsdevInitialize();
    rc = nsdevTerminateProgram(PMSW_TID);
    if(R_FAILED(rc))
        logInfo(LOGFILE, "Failed to terminate program.\n");
    else
    {
        vhidPressButtonAndWait('A', 1E+9L); // confirm process closing error
        if(func != NULL) func();
        vhidPressButtonAndWait('A', 1E+9L); // open game
        vhidPressButtonAndWait('A', 12E+9L); // confirm user and wait to title scene
        vhidPressButtonAndWait('A', 6E+9L); // start and wait for loading save data
        vhidPressButtonAndWait('A', 1E+9L); // show raid detail
    }
    nsdevExit();
    return rc;
}


Result frameSimpleRestart()
{
    return _frameRestartImpl(NULL);
}


Result frameBackupRestart()
{

}
