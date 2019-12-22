// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include "log.h"

char *LOGFILE = "/sys_time_changer.log";
TimeServiceType __nx_time_service_type = TimeServiceType_Menu;

Result setDateToNextDay(u64 cur_day, u64 *next_day)
{
	*next_day = cur_day + 24*60*60;
	return timeSetCurrentTime(TimeType_LocalSystemClock, *next_day);
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    // Other initialization goes here. As a demonstration, we print hello world.
    printf("Press A to move to next date.\n");
    u64 current_date = 1573776600;
    u64 next_date;
    Result rc;
    char logMsg[64];

    strcpy(logMsg,  "Write first log.\n");
    logInfo(LOGFILE, logMsg);

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A)
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
    			sprintf(logMsg, "Time is set to: %ld\n", next_date);
                logInfo(LOGFILE, logMsg);
    		} 

        }

        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }
    
    strcpy(logMsg,  "Exiting.\n");
    logInfo(LOGFILE, logMsg);
    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
