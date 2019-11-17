#include "pch.h"
#include "meeting.h"

json meeting::meetingObj_to_json(const meeting& meetInfo)
{
	json meeting_json;
	meeting_json["minimumParticipants"] = meetInfo.minimumParticipants;
	meeting_json["requestID"] = meetInfo.requestID;
	meeting_json["meetingID"] = meetInfo.meetingID;

	for (const string& element : meetInfo.invitedParticipantsIP)
	{
		meeting_json["invitedParticipantsIP"].push_back(element);
	}

	for (const string& element : meetInfo.confirmedParticipantsIP)
	{
		meeting_json["confirmedParticipantsIP"].push_back(element);
	}
	meeting_json["roomNumber"] = meetInfo.roomNumber;
	meeting_json["topic"] = meetInfo.topic;
	meeting_json["requesterIP"] = meetInfo.requesterIP;
	return meeting_json;
}

meeting meeting::json_to_meetingObj(const json& meeting_json)
{
	meeting meetInfo;

	const string minimumParticipants = meeting_json.at("minimumParticipants");
	meetInfo.minimumParticipants = minimumParticipants;

	const string meetingID = meeting_json.at("meetingID");
	meetInfo.meetingID = meetingID;

	for (const string element : meeting_json["invitedParticipantsIP"])
	{
		meetInfo.invitedParticipantsIP.push_back(element);
	}

	for (const string element : meeting_json["confirmedParticipantsIP"])
	{
		meetInfo.confirmedParticipantsIP.push_back(element);
	}

	const string tempRoomNum = meeting_json.at("roomNumber");
	meetInfo.roomNumber = tempRoomNum;

	const string tempTopic = meeting_json.at("topic");
	meetInfo.topic = tempTopic;

	const string tempRequestIP = meeting_json.at("requesterIP");
	meetInfo.requesterIP = tempRequestIP;

	return meetInfo;
}

meeting::meeting()
{
	int minimumParticipants = -1;
	int rq = -1;
	int mt = -1;
	vector<string> invitedParticipantsIP;
	vector<string> confirmedParticipantsIP;
	string roomNumber = "";
	string topic = "";
	// Date bookingDate;
	string requesterIP = "";
}

meeting::meeting(const string& minimumParticipants, const string& requestID, const string& meetingID,
	const vector<string>& invitedParticipantsIP, const vector<string>& confirmedParticipantsIP,
	const string& roomNumber, const string& topic, const string& meetingDay, const string& requesterIP,
	const bool& meetingStatus)
{
	this->minimumParticipants = minimumParticipants;
	this->requestID = requestID;
	this->meetingID = meetingID;
	this->invitedParticipantsIP = invitedParticipantsIP;
	this->confirmedParticipantsIP = confirmedParticipantsIP;
	this->roomNumber = roomNumber;
	this->topic = topic;
	this->meetingDay = meetingDay;
	this->requesterIP = requesterIP;
	this->requesterIP = requesterIP;
	this->meetingStatus= meetingStatus;
}

json meeting::client_get_meeting(json& db, const string& day, const string& time)
{
	try
	{
		return db.at(day).at(time);
	}
	catch (nlohmann::json::exception& e)
	{
		cout << "Exception: client_get_meeting method throws -> " << e.what() << endl;
		return json({});
	}
}

bool meeting::client_update_meeting(json& db, const string& day, const string& time, const json& meeting)
{
	try
	{
		db.at(day).at(time).update(meeting);
		// db.at(days).at(time) = meeting;
		return true;
	}
	catch (nlohmann::json::exception& e)
	{
		cout << "Exception: client_update_meeting method throws -> " << e.what() << endl;
		return false;
	}
}

bool meeting::client_isMeeting(json& db, const string& day, const string& time)
{
	return !meeting::client_get_meeting(db, day, time).empty();
}


// server specific meeting manipulators

json meeting::server_get_meeting(json& db, const string& day, const string& time, const string& room)
{
	try
	{
		return db.at(day).at(time).at(room);
	}
	catch (nlohmann::json::exception& e)
	{
		cout << "Exception: server_get_meeting method throws -> " << e.what() << endl;
		return json({});
	}
}

bool meeting::server_update_meeting(json& db, const string& day, const string& time, const string& room,
                                    const json& meeting)
{
	try
	{
		db.at(day).at(time).at(room).update(meeting);
		// db.at(days).at(time).at(room) = meeting;
		return true;
	}
	catch (nlohmann::json::exception& e)
	{
		cout << "Exception: server_update_meeting method throws -> " << e.what() << endl;
		return false;
	}
}

bool meeting::server_isMeeting(json& db, const string& day, const string& time, const string& room)
{
	return !meeting::server_get_meeting(db, day, time, room).empty();
}
