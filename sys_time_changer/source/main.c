// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"
#include "frame.h"
#include "vhid.h"

// Sysmodules should not use applet*.
u32 __nx_applet_type = AppletType_None;
TimeServiceType __nx_time_service_type = TimeServiceType_Menu;
bool ChangeDateTrigger = false;
bool Exit = false;

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
    svcSleepThread(15E+9L);

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


int inputPoller()
{
    hidScanInput();
    u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

    if ((kDown & KEY_MINUS || kDown & KEY_Y) && (kHeld & KEY_MINUS && kHeld & KEY_Y))
    {
        logInfo(LOGFILE, "Hold MINUS & Y detected.\n");
        return 1;
    }
    if ((kDown & KEY_MINUS || kDown & KEY_B) && (kHeld & KEY_MINUS && kHeld & KEY_B))
    {
        logInfo(LOGFILE, "Hold MINUS & B detected.\n");
        return 2;
    }
    if ((kDown & KEY_MINUS || kDown & KEY_X) && (kHeld & KEY_MINUS && kHeld & KEY_X))
    {
        logInfo(LOGFILE, "Hold MINUS & X detected.\n");
        return 0;
    }
    
    return 255;
}


// Main program entrypoint
int main(int argc, char* argv[])
{
    u64 current_date = 1573776600;
    // Result rc;
    int i;
    int ope_flag, frame_number, sl_cnt;
    char logMsg[40];

    logInfo(LOGFILE, "Application started.\n");

    // Main loop
    while(appletMainLoop() && (!Exit))
    {
        
        ope_flag = inputPoller();
        switch(ope_flag)
        {
            case 1: // move frame forward by input number.
            {
                logInfo(LOGFILE, "MODE: specific frame number.\n");
                frame_number = frameGetNumber();
                logInfo(LOGFILE, logMsg);
                for(i=0; i<frame_number; i++)
                    frameForward(&current_date);
                snprintf(logMsg, 40,  "Move frame forward by: %d.\n", frame_number);
                break;
            }

            case 2: // S&L to get wanted PokeMon.
            {
                logInfo(LOGFILE, "MODE: Frame forward by 3 and confirm.\n");
                vhidNewController();
                sl_cnt = frameSL(&current_date);
                snprintf(logMsg, 40, "Break after %d times S&L.\n", sl_cnt);
                logInfo(LOGFILE, logMsg);
                vhidDetachController();
                break;
            }

            case 0:
            {
                Exit = true;
                break;
            }

        }
        svcSleepThread(1E+8L);
        ope_flag = 255;        
    }
    logInfo(LOGFILE, "Application exited.\n");

    return 0;
}
