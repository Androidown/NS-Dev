#pragma once
/*API for virtual human input devices, aka virtual controller*/

int vhidNewController();
void vhidPressButtonAndWait(char button, s64 timeout);
void vhidDetachController();
