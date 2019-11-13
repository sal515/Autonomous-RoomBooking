#include "pch.h"
#include "meeting.h"

json meeting::meetingObj_to_json(const meeting& meetInfo)
{
	json meeting_json;
	meeting_json["minimumParticipants"] = meetInfo.minimumParticipants;
	meeting_json["rq"] = meetInfo.rq;
	meeting_json["mt"] = meetInfo.mt;

	json invitationParticipantArr = json::array();
	for (user element : meetInfo.invitedParticipants)
	{
		invitationParticipantArr.push_back
		({
				{"ip", element.ip},
				{"listeningPort", element.listeningPort},
				{"userName", element.userName}
			});
	}
	meeting_json["invitedParticipants"] = invitationParticipantArr;

	json confirmedParticipantArr = json::array();
	for (user element : meetInfo.confirmedParticipants)
	{
		confirmedParticipantArr.push_back
		({
				{"ip", element.ip},
				{"listeningPort", element.listeningPort},
				{"userName", element.userName}
			});
	}
	meeting_json["confirmedParticipants"] = confirmedParticipantArr;
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
