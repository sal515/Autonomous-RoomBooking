#pragma once
#include "pch.h"

string ask_for_ip();
void menu();
vector<string> list_of_participants(int min);
bool getMeetID(const string& meetID);
bool check_ip(const string &ip);
bool check_schedule(json schedule);
bool extract_date(const string& s, int& d, int& m, int& y);

// ==================  Examples  ======================================
int test_pause_exit();
