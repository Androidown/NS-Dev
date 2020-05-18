#include "vhid.h"
#include "log.h"
#include "cfg.h"

#include <switch.h>
#include <string.h>

#define INIFILE "/item_drop/config.ini"


void **getDropSet(char **dropset)
{
   
    int total_set = cfgGetIniKeyInt("general", "total_set", INIFILE);

    for (int i=1; i<=total_set; i++)
    {
        
    }
    cfgGetIniKeyString
}


void actMoveToNextCenter(char direction)
{
    int move_time = cfgGetIniKeyInt("general", "move_time_ms", INIFILE);
    vhidMoveTo(direction, move_time * 1E+6L);
}


void actDropItemSingle(int choice_pos)
{
    vhidPressButtonAndWait(KEY_A, 5E+8L);
    for(int i=0; i<choice_pos; i++)
    {
        vhidPressButtonAndWait(KEY_DDOWN, 5E+8L);
    }
    vhidPressButtonAndWait(KEY_A, 5E+8L);
}


void actDropItemsOneSet(int has_item[40], int choice_pos[40])
{
    vhidPressButtonAndWait(KEY_X, 5E+8L); // open bag
    int item_dropped = 0;
    for (int i=0; i<40; i++)
    {
        if (has_item[i])
        {
            actDropItemSingle(choice_pos[i]);
            item_dropped ++;
        }
        vhidPressButtonAndWait(KEY_DRIGHT, 5E+8L); // move to right side item
        if (i % 10 == 9)
        {
            vhidPressButtonAndWait(KEY_DDOWN, 5E+8L);  // move to next line
        }
        if (item_dropped % 9 == 0)
        {
            vhidPressButtonAndWait(KEY_B, 5E+8L); // close bag
            actMoveToNextCenter('R');
            vhidPressButtonAndWait(KEY_X, 5E+8L); // open bag
        }
    }
    vhidPressButtonAndWait(KEY_B, 5E+8L); // close bag

}

void actDropAllItems()
{
    
}