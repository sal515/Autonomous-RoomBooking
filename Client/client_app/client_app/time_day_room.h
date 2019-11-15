#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::vector;


struct time_day_room
{
	static const map<string, string> day_map();
	static const map<string, string> time_map(int startHr, int endHr);
	static const vector<string> room_vec();
};
