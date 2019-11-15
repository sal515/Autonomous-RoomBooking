#pragma once
#include <iostream>
#include <string>
#include "json.hpp"
#include <vector>

using std::string;
using std::vector;
using json = nlohmann::json;
using std::cout;
using std::endl;


struct meeting
{
	int minimumParticipants;
	int rq;
	int mt;
	vector<string> invitedParticipantsIP;
	vector<string> confirmedParticipantsIP;
	string roomNumber;
	string topic;
	string requesterIP;

	static json meetingObj_to_json(const meeting& meetInfo);
	static meeting json_to_meetingObj(const json& meeting_json);
	
	meeting();
	meeting	(
		const int &minimumParticipants,
		const int &rq,
		const int &mt,
		const vector<string> &invitedParticipantsIP,
		const vector<string> &confirmedParticipantsIP,
		const string &roomNumber,
		const string &topic,
		const string &requesterIP
	);

	// client specific meeting manipulators
	static json client_get_meeting(json& db, const string& day, const string& time);
	static bool client_update_meeting(json& db, const string& day, const string& time, const json& meeting);
	static bool client_isMeeting(json& db, const string& day, const string& time);


	// --------------------------------------------

	// server specific meeting manipulators
	static json server_get_meeting(json& db, const string& day, const string& time, const string& room);
	static bool server_update_meeting(json& db, const string& day, const string& time, const string& room,                                  const json& meeting);
	static bool server_isMeeting(json& db, const string& day, const string& time, const string& room);
};
