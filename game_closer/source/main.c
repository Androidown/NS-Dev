// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Include the main libnx system header, for Switch development
#include <switch.h>


// Main program entrypoint
int main(int argc, char* argv[])
{
    // 0x0100abf008968000
    u64 PMSW_TID = 72246641462509568L;
    Result rc;
    consoleInit(NULL);

    // Other initialization goes here. As a demonstration, we print hello world.
    printf("Press A to terminate program.\n");

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
            rc = nsdevTerminateProgram(PMSW_TID);
            if(R_FAILED(rc))
                printf("Failed to terminate program.");
        }
        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }
    
    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
