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
	meeting_json["meetingDay"] = meetInfo.meetingDay;
	meeting_json["meetingTime"] = meetInfo.meetingTime;
	meeting_json["requesterIP"] = meetInfo.requesterIP;
	meeting_json["meetingStatus"] = meetInfo.meetingStatus;
	return meeting_json;
}

meeting meeting::json_to_meetingObj(const json& meeting_json)
{
	meeting meetInfo;

	const string minimumParticipants = meeting_json.at("minimumParticipants");
	meetInfo.minimumParticipants = minimumParticipants;

	const string requestID = meeting_json.at("requestID");
	meetInfo.requestID = requestID;
	
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
	
	const string meetingDay = meeting_json.at("meetingDay");
	meetInfo.meetingDay = meetingDay;
	
	const string meetingTime = meeting_json.at("meetingTime");
	meetInfo.meetingTime = meetingTime;

	const string tempRequestIP = meeting_json.at("requesterIP");
	meetInfo.requesterIP = tempRequestIP;
	
	const bool meetingStatus = meeting_json.at("meetingStatus");
	meetInfo.meetingStatus = meetingStatus;

	return meetInfo;
}

meeting::meeting()
{
	minimumParticipants = "-1";
	requestID = "-1";
	meetingID = "-1";
	invitedParticipantsIP = vector<string>();
	confirmedParticipantsIP = vector<string>();
	roomNumber = "-1";
	topic = "-1";
	meetingDay = "-1";
	meetingTime = "-1";
	requesterIP = "-1";
	meetingStatus = false;
}

meeting::meeting(const string& minimumParticipants, const string& requestID, const string& meetingID,
	const vector<string>& invitedParticipantsIP, const vector<string>& confirmedParticipantsIP,
	const string& roomNumber, const string& topic, const string& meetingDay, const string& meetingTime, const string& requesterIP,
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
	this->meetingTime = meetingTime;
	this->requesterIP = requesterIP;
	this->meetingStatus = meetingStatus;
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

// is meetting already booked at the given time and day
bool meeting::client_isMeeting(json& db, const string& day, const string& time)
{
	return !meeting::client_get_meeting(db, day, time).empty();
}

bool meeting::print_meeting(meeting meet)
{
	cout << "\nMinimum # of Participants: " << meet.minimumParticipants;
	cout << "\nRequest ID: " << meet.requestID;
	cout << "\nMeeting ID: " << meet.meetingID;
	if (meet.invitedParticipantsIP.size() != 0) {
		cout << "\nInvited Participants: ";
		for (string var : meet.invitedParticipantsIP)
		{
			cout << "\n" << var;
		}
	}
	if (meet.confirmedParticipantsIP.size() != 0) {
		cout << "\Confirmed Participants";
		for (string var : meet.confirmedParticipantsIP)
		{
			cout << "\n" << var;
		}
	}
	cout << "\nRoom Number: " << meet.roomNumber;
	cout << "\nTopic: " << meet.topic;
	cout << "\nDay: " << meet.meetingDay;
	cout << "\nTime: " << meet.meetingTime;
	
	cout << "\nMeeting Status: " << meet.meetingStatus << endl;
	return true;
}

