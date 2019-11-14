#pragma once
#include <string>
#include "json.hpp"
#include <vector>

using std::string;
using std::vector;
using json = nlohmann::json;

struct meeting
{
	int minimumParticipants;
	int rq;
	int mt;
	vector<string> invitedParticipantsIP;
	vector<string> confirmedParticipantsIP;
	string roomNumber;
	string topic;
	// Date bookingDate;
	string requesterIP;

	// Model Only required for auto registration
	// vector<user> invitedParticipantsIP;
	// vector<user> confirmedParticipantsIP;


	static json meetingObj_to_json(const meeting &meetInfo);

};
