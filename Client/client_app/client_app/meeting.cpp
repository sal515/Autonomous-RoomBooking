#include "pch.h"
#include "meeting.h"

json meeting::meetingObj_to_json(const meeting& meetInfo)
{
	json meeting_json;
	meeting_json["minimumParticipants"] = meetInfo.minimumParticipants;
	meeting_json["rq"] = meetInfo.rq;
	meeting_json["mt"] = meetInfo.mt;

	for (const string &element : meetInfo.invitedParticipantsIP)
	{
		meeting_json["invitedParticipantsIP"].push_back(element);
	}

	for (const string &element : meetInfo.confirmedParticipantsIP)
	{
		meeting_json["confirmedParticipantsIP"].push_back(element);
	}
	meeting_json["roomNumber"] = meetInfo.roomNumber;
	meeting_json["topic"] = meetInfo.topic;
	meeting_json["requesterIP"] = meetInfo.requesterIP;
	return meeting_json;
}




// {"meetingDate", meetInfo.},
