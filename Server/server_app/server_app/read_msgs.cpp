#include "pch.h"

bool checkSchedule(std::string date, std::string time, std::string roomID, json db) {
	try {
		if (db.at(date).at(time).at(roomID).empty()) {
			return true;
		}
		else {
			return false;
		}
	}
	catch (json::out_of_range) {
		return true;
	}
}