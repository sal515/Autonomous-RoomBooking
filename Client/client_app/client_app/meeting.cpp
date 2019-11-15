#include "pch.h"
#include "meeting.h"

json meeting::meetingObj_to_json(const meeting& meetInfo)
{
	json meeting_json;
	meeting_json["minimumParticipants"] = meetInfo.minimumParticipants;
	meeting_json["rq"] = meetInfo.rq;
	meeting_json["mt"] = meetInfo.mt;

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

meeting meeting::json_to_meetingObj(const json meeting_json)
{
	meeting meetInfo;

	meetInfo.minimumParticipants = meeting_json.at("minimumParticipants");

	meetInfo.rq = meeting_json.at("rq");
	meetInfo.mt = meeting_json.at("mt");

	for (const string element : meeting_json["invitedParticipantsIP"])
	{
		meetInfo.invitedParticipantsIP.push_back(element);
	}

	for (const string element : meeting_json["confirmedParticipantsIP"])
	{
		meetInfo.confirmedParticipantsIP.push_back(element);
	}

	string tempRoomNum = meeting_json.at("roomNumber");
	meetInfo.roomNumber = tempRoomNum;

	string tempTopic = meeting_json.at("topic");
	meetInfo.topic = tempTopic;

	string tempRequestIP = meeting_json.at("requesterIP");
	meetInfo.requesterIP = tempRequestIP;

	return meetInfo;
}


// {"meetingDate", meetInfo.},
