#include "pch.h"
#include "meeting.h"

json meeting::meetingObj_to_json(const meeting& meetInfo)
{
	json meeting_json;
	meeting_json["minimumParticipants"] = meetInfo.minimumParticipants;
	meeting_json["rq"] = meetInfo.rq;
	meeting_json["mt"] = meetInfo.mt;
	meeting_json["invitedParticipants"] = meetInfo.invitedParticipants;
	meeting_json["confirmedParticipants"] = meetInfo.confirmedParticipants;
	meeting_json["roomNumber"] = meetInfo.roomNumber;
	meeting_json["topic"] = meetInfo.topic;
	meeting_json["requester"] = 
	{ 
		{"ip", meetInfo.requester.ip},
		{"listeningPort", meetInfo.requester.listeningPort},
		{"userName", meetInfo.requester.userName}
	};
	return meeting_json;
}


// {"meetingDate", meetInfo.},
