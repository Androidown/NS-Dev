#pragma once
/*API for virtual human input devices, aka virtual controller*/

int vhidNewController();
void vhidPressButtonAndWait(u64 button, s64 timeout);
void vhidDetachController();
void vhidMoveTo(char direction, int move_time);
