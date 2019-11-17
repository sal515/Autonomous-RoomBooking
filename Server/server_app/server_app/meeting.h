﻿#pragma once

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
	string requesterIP;
	bool meetingStatus;

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
		const string& requesterIP,
		const bool& meetingStatus
	);

	// client specific meeting manipulators
	static json client_get_meeting(json& db, const string& day, const string& time);
	static bool client_update_meeting(json& db, const string& day, const string& time, const json& meeting);
	static bool client_isMeeting(json& db, const string& day, const string& time);


	// --------------------------------------------

	// server specific meeting manipulators
	static json server_get_meeting(json& db, const string& day, const string& time, const string& room);
	static bool server_update_meeting(json& db, const string& day, const string& time, const string& room,
		const json& meeting);
	static bool server_isMeeting(json& db, const string& day, const string& time, const string& room);
};
