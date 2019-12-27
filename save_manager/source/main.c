#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <switch.h>

int main(int argc, char **argv)
{
    Result rc=0;

    DIR* dir;
    struct dirent* ent;

    AccountUid uid={0};
    u64 PMSW_TID = 0x0100abf008968000;

    consoleInit(NULL);

    rc = accountInitialize(AccountServiceType_Application);
    if (R_FAILED(rc)) {
        printf("accountInitialize() failed: 0x%x\n", rc);
    }

    if (R_SUCCEEDED(rc)) {
        rc = accountGetPreselectedUser(&uid);
        accountExit();

        if (R_FAILED(rc)) {
            printf("accountGetPreselectedUser() failed: 0x%x\n", rc);
        }
    }

    if (R_SUCCEEDED(rc)) {
        printf("Using application_id=0x%016lx uid: 0x%lx 0x%lx\n", PMSW_TID, uid.uid[1], uid.uid[0]);
    }

    //You can use any device-name. If you want multiple saves mounted at the same time, you must use different device-names for each one.
    if (R_SUCCEEDED(rc)) {
        rc = fsdevMountSaveData("save", PMSW_TID, uid);//See also libnx fs.h/fs_dev.h
        if (R_FAILED(rc)) {
            printf("fsdevMountSaveData() failed: 0x%x\n", rc);
        }
    }

    //At this point you can use the mounted device with standard stdio.
    //After modifying savedata, in order for the changes to take affect you must use: rc = fsdevCommitDevice("save");
    //See also libnx fs_dev.h for fsdevCommitDevice.

    if (R_SUCCEEDED(rc)) {
        dir = opendir("save:/");//Open the "save:/" directory.
        if(dir==NULL)
        {
            printf("Failed to open dir.\n");
        }
        else
        {
            printf("Dir-listing for 'save:/':\n");
            while ((ent = readdir(dir)))
            {
                printf("d_name: %s\n", ent->d_name);
            }
            closedir(dir);
            printf("Done.\n");
        }

        //When you are done with savedata, you can use the below.
        //Any devices still mounted at app exit are automatically unmounted.
        fsdevUnmountDevice("save");
    }

    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
