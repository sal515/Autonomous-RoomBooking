#pragma once
#include <string>
#include "json.hpp"
#include <vector>
#include <Bits.h>


using std::string;
using std::vector;
using json = nlohmann::json;

struct meeting
{
	int minimumParticipants = -1;
	int rq = -1;
	int mt= -1;
	vector<string> invitedParticipantsIP;
	vector<string> confirmedParticipantsIP;
	string roomNumber = "";
	string topic = "";
	// Date bookingDate;
	string requesterIP = "";

	// Model Only required for auto registration
	// vector<user> invitedParticipantsIP;
	// vector<user> confirmedParticipantsIP;


	static json meetingObj_to_json(const meeting &meetInfo);
	static meeting json_to_meetingObj(const json meeting_json);

};
