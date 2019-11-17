#include "pch.h"
#include "process_messages.h"

void processMessages(json& db, const json& req_data)
{
	if (!(messageType.request.compare(req_data.at("message"))))
	{
		string day = req_data.at("meetingDay");
		string time = req_data.at("meetingTime");
		vector<string> all_room = time_day_room::room_vec();
		bool available = false;

		for (string room : all_room)
		{
			if (!meeting::server_isMeeting(db, day, time, room))
			{
				// room available and can be booked - first come first server for rooms
				// update the db

				//TODO : DO I need to save the messagetype to the db ? ? ? 
				//TODO : FIXME -> How to generate meeting id - what is the logic
				//TODO : FIXME -> How to get ip for the requester - what is the logic
				json meetingObj = meeting::meetingObj_to_json(meeting(
					req_data.at("message"),
					req_data.at("minimumParticipants"),
					req_data.at("requestID"),
					"1",
					req_data.at("invitedParticipantsIP"),
					vector<string>(),
					room,
					req_data.at("topic"),
					day,
					time,
					"1.1.1.1",
					false
				));
				meeting::server_update_meeting(db, day, time, room, meetingObj);
				db_helper::save_db(config.DB_PATH, db);
				break;
			}
			else
			{
				// rooms not available - build unavailable reponse for the client
				// db will not be updated
				json unavailable = messages::response_unavail(req_data.at("requestID"));
				// TODO: Send response to client 
			}
		}
	}
}
