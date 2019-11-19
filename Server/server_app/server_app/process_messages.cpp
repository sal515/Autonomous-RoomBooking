#include "pch.h"
#include "process_messages.h"


void processMessages(json& db, const json& req_data, const string& requesterIP, std::atomic <int> &global_meet_id)
{
	// Process message type : Request 
	if (!(messageType.request.compare(req_data.at("message"))))
	{
		string day = req_data.at("meetingDay");
		string time = req_data.at("meetingTime");
		vector<string> all_room = time_day_room::room_vec();
		bool available = false;
		string meetingID = std::to_string(++global_meet_id);
		for (string room : all_room)
		{
			if (!meeting::isMeeting(db, day, time, room))
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
					meetingID,
					req_data.at("invitedParticipantsIP"),
					vector<string>{},
					room,
					req_data.at("topic"),
					day,
					time,
					requesterIP,
					false
				));
				meeting::update_meeting(db, day, time, room, meetingObj);
				// db_helper::save_db(config.CONFIRMED_DB_PATH, db);
				db_helper::save_db(config.PENDING_DB_PATH, db);
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
	else if (!(messageType.accept.compare(req_data.at("message"))))
	{
	}
	else if (!(messageType.reject.compare(req_data.at("message"))))
	{
	}
	else if (!(messageType.cancelRequest.compare(req_data.at("message"))))
	{
		bool valid = false;
		meeting temp_meetObj;
		const json jsonObj = db_helper::getMeetingByID(db, req_data.at("meetingID"));
		
		if (!jsonObj.empty())
		{
			temp_meetObj = meeting::json_to_meetingObj(jsonObj);
		}

		if (!temp_meetObj.requesterIP.compare(requesterIP))
		{
			valid = true;
		}

		const meeting meetObj = temp_meetObj;

		if (valid)
		{
			// TODO: Build the message.notScheduled
			json notScheduled = messages::not_sched(
				meetObj.requestID,
				meetObj.meetingDay,
				meetObj.meetingTime,
				meetObj.minimumParticipants,
				meetObj.confirmedParticipantsIP,
				meetObj.topic
			);

			for (const string& participant : meetObj.invitedParticipantsIP)
			{
				// TODO: notify all participants of meeting cancellation
				// sendMessageToClients(notScheduled, participant);
			}

			// TODO: delete from second database too 
			// deleting the meeting object in the database 
			meeting::update_meeting(
				db,
				meetObj.meetingDay,
				meetObj.meetingTime,
				meetObj.roomNumber,
				json({}));
			db_helper::save_db(config.CONFIRMED_DB_PATH, db);
		}
	}
	else if (!(messageType.withdraw.compare(req_data.at("message"))))
	{
	}
	else if (!(messageType.add.compare(req_data.at("message"))))
	{
	}
}
