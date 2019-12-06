#pragma once
#pragma once
#include "pch.h"
#include "global_variables.h"

string ask_for_ip();
void menu(json db, string ownIP);
vector<string> list_of_participants(int min);
bool check_ip(const string& ip);
// bool check_schedule(json schedule);
// ==================  Examples  ======================================
int test_pause_exit();


