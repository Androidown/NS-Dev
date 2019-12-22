// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"

// Sysmodules should not use applet*.
u32 __nx_applet_type = AppletType_None;
TimeServiceType __nx_time_service_type = TimeServiceType_Menu;
bool ChangeDateTrigger = false;
bool Exit = false;
char *LOGFILE = "/sys_time_changer.log";

// Adjust size as needed.
#define INNER_HEAP_SIZE 0x000320000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char   nx_inner_heap[INNER_HEAP_SIZE];

void __libnx_initheap(void)
{
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	// Newlib
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

// Init/exit services, update as needed.
void __attribute__((weak)) __appInit(void)
{
    Result rc;
    svcSleepThread(10000000000L);

    // Initialize default services.
    rc = smInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    // Enable this if you want to use HID.
    rc = hidInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

    //Enable this if you want to use time.
    rc = timeInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_Time));

    __libnx_init_time();

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));
    
    // setting hos version because apparently it changes some functions
    rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) 
    {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
    }

    rc = nsInitialize();
    if (R_FAILED(rc))
        fatalSimple(rc);

    fsdevMountSdmc();
}


void __attribute__((weak)) __appExit(void)
{
    // Cleanup default services.
    fsdevUnmountAll();
    fsExit();
    timeExit();//Enable this if you want to use time.
    hidExit();// Enable this if you want to use HID.
    smExit();
    nsExit();
}


void inputPoller()
{
    hidScanInput();
    u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
    char logMsg[32];

    if ((kDown & KEY_MINUS || kDown & KEY_Y) && (kHeld & KEY_MINUS && kHeld & KEY_Y))
    {
        strcpy(logMsg,  "Hold MINUS & Y detected.\n");
        logInfo(LOGFILE, logMsg);
        ChangeDateTrigger = true;
    }
    if ((kDown & KEY_MINUS || kDown & KEY_B) && (kHeld & KEY_MINUS && kHeld & KEY_B))
    {
        strcpy(logMsg,  "Hold MINUS & B detected.\n");
        logInfo(LOGFILE, logMsg);
        ChangeDateTrigger = false;
    }
    if ((kDown & KEY_MINUS || kDown & KEY_X) && (kHeld & KEY_MINUS && kHeld & KEY_X))
    {
        strcpy(logMsg,  "Hold MINUS & X detected.\n");
        logInfo(LOGFILE, logMsg);
        ChangeDateTrigger = false;
        Exit = true;
    }
}

Result setDateToNextDay(u64 cur_day, u64 *next_day)
{
    *next_day = cur_day + 24*60*60;
    return timeSetCurrentTime(TimeType_LocalSystemClock, *next_day);
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Other initialization goes here. As a demonstration, we print hello world.
    u64 current_date = 1573776600;
    u64 next_date;
    Result rc;
    int i;
    char logMsg[32];

    strcpy(logMsg,  "Write first log.\n");
    logInfo(LOGFILE, logMsg);

    // Main loop
    while (appletMainLoop() && (!Exit))
    {
        inputPoller();

        if (ChangeDateTrigger)
        {
            strcpy(logMsg,  "Change date enabled.\n");
            logInfo(LOGFILE, logMsg);
            for(i=0; i<10; i++)
            {
                rc = setDateToNextDay(current_date, &next_date);
                if (R_FAILED(rc))
                {
                    strcpy(logMsg, "Unable to set local time.\n");
                    logInfo(LOGFILE, logMsg);
                } 
                else
                {
                    current_date = next_date;
                    timeGetCurrentTime (TimeType_LocalSystemClock, &next_date);

                    strcpy(logMsg, "Time is set to next_date.\n");
                    logInfo(LOGFILE, logMsg);
                } 

            }
        }
    }
    strcpy(logMsg,  "Exiting.\n");
    logInfo(LOGFILE, logMsg);
    // Deinitialize and clean up resources used by the console (important!)
    return 0;
}
