#pragma once
#include <string>
#include "json.hpp"
#include <vector>
#include "user.h"

using std::string;
using std::vector;
using json = nlohmann::json;

struct meeting
{
	int minimumParticipants;
	int rq;
	int mt;
	vector<user> invitedParticipants;
	vector<user> confirmedParticipants;
	string roomNumber;
	string topic;
	// Date bookingDate;
	user requester;

	static json meetingObj_to_json(const meeting &meetInfo);

};
