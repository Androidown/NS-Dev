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
    int ope_code = 255;

    if ((kDown & KEY_L || kDown & KEY_DRIGHT) && (kHeld & KEY_L && kHeld & KEY_DRIGHT))
    {
        logInfo(LOGFILE, "Hold L & DRIGHT detected.\n");
        ope_code = 1;
    }
    if ((kDown & KEY_L || kDown & KEY_DLEFT) && (kHeld & KEY_L && kHeld & KEY_DLEFT))
    {
        logInfo(LOGFILE, "Hold L & DLEFT detected.\n");
        ope_code = 2;
    }
    if ((kDown & KEY_L || kDown & KEY_DDOWN) && (kHeld & KEY_L && kHeld & KEY_DDOWN))
    {
        logInfo(LOGFILE, "Hold L & DDOWN detected.\n");
        ope_code = 254;
    }
    if ((kDown & KEY_L || kDown & KEY_X) && (kHeld & KEY_L && kHeld & KEY_X))
    {
        logInfo(LOGFILE, "Hold L & X detected.\n");
        ope_code = 0;
    }
    
    return ope_code;
}


// Main program entrypoint
int main(int argc, char* argv[])
{
    u64 current_date = 1573776600;
    // Result rc;
    int i, cnt = 0;
    int ope_flag, sl_cnt, frame_number;
    int frame_move[1] = {0};
    int frame_backup[20] = {0};
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
                timeSetCurrentTime(TimeType_LocalSystemClock, current_date);
                logInfo(LOGFILE, "MODE: specific frame number.\n");
                frameReadConfig(frame_move, "frame.", true);
                cnt = frameReadConfig(frame_backup, "backup.", false);
                frame_number = frame_move[0];
                vhidNewController();
                for(i=0; i<frame_number; i++)
                {
                    frameForward(&current_date);
                    if(cnt && (i == frame_backup[cnt-1]))
                    {
                        snprintf(logMsg, 40,  "Save at %d frame.\n", i);
                        logInfo(LOGFILE, logMsg);
                        frameSave();
                        cnt-- ;
                    }
                }

                snprintf(logMsg, 40,  "Move frame forward by: %d.\n", frame_number);
                logInfo(LOGFILE, logMsg);
                vhidDetachController();
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

            case 254: // peserved for testing stuff.
            {
                logInfo(LOGFILE, "MODE: Test for fucntion.\n");
                vhidNewController();
                for(i=0; i<7; i++)
                {
                    frameForward(&current_date);
                    if(!(i % 3)) frameSave();
                }
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
