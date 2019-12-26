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
    u64 RUNNING_TID;
    Result rc;
    consoleInit(NULL);

    // Other initialization goes here. As a demonstration, we print hello world.
    printf("Press A to open program.\n");
    printf("Press B to terminate programs.\n");

    NsLaunchProperties nsp = {0};
    nsp.program_id = PMSW_TID;
    nsp.storageID = NcmStorageId_SdCard;
    nsp.is_application = 1;
    nsp.version = 1;
    nsp.index;
    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        nsdevInitialize();
        if (kDown & KEY_A)
        {
            rc = nsdevLaunchProgram(&RUNNING_TID, &nsp, 16);
            if(R_FAILED(rc))
                printf("Failed to open program.\n");
        }
        if (kDown & KEY_B)
        {
            rc = nsdevTerminateProgram(PMSW_TID);
            if(R_FAILED(rc))
                printf("Failed to terminate program.\n");
        }
        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }
    
    nsdevExit();
    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
