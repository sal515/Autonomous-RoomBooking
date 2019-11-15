#include "pch.h"
#include "time_day_room.h"


const map<string, string> time_day_room::day_map()
{
	map<string, string> m;
	m.emplace("monday", "monday");
	m.emplace("tuesday", "tuesday");
	m.emplace("wednesday", "wednesday");
	m.emplace("thursday", "thursday");
	m.emplace("friday", "friday");
	// m.emplace("saturday", "saturday");
	// m.emplace("sunday", "sunday");
	return m;
}

const map<string, string> time_day_room::time_map(int startHr, int endHr)
{
	map<string, string> m;
	for (int i = startHr; i <= endHr; i++)
	{
		m.emplace(std::to_string(i), std::to_string(i));
	}
	return m;
}

const vector<string> time_day_room::room_vec()
{
	vector<string> rooms =
	{
		"EV005.251",
		"EV02.301"
	};
	return rooms;
}
