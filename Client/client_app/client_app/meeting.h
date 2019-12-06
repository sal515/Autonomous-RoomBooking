#pragma once

#include "pch.h"

using json = nlohmann::json;

struct meeting
{
	string message;
	string minimumParticipants;
	string requestID;
	string meetingID;
	vector<string> invitedParticipantsIP;
	vector<string> confirmedParticipantsIP;
	string roomNumber;
	string topic;
	string meetingDay;
	string meetingTime;
	string requesterIP;
	bool meetingStatus;
	bool server_confirmation;

	static json meetingObj_to_json(const meeting& meetInfo);
	static meeting json_to_meetingObj(const json& meeting_json);

	meeting();
	meeting(
		const string& message,
		const string& minimumParticipants,
		const string& requestID,
		const string& meetingID,
		const vector<string>& invitedParticipantsIP,
		const vector<string>& confirmedParticipantsIP,
		const string& roomNumber,
		const string& topic,
		const string& meetingDay,
		const string& meetingTime,
		const string& requesterIP,
		const bool& meetingStatus,
		const bool& server_confirmation
	);

	// client specific meeting manipulators

	static json get_meeting(json& db, const string& day, const string& time);
	static bool update_meeting(json& db, const string& day, const string& time, const json& meeting);
	static bool isMeeting(json& db, const string& day, const string& time);
	static bool print_meeting(meeting meet);
};
