#pragma once

void frameForward(u64 *cur_day);
int frameGetNumber();
int frameReadConfig(int* frames, const char* prefix, bool single);
int frameSL(u64 *cur_day);
void frameSave();
Result frameSimpleRestart();
Result frameBackupRestart();
