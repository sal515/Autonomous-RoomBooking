﻿#include "pch.h"
#include "process_messages.h"
#define BUFLEN 32768		//Max length of buffer 
#define SERVER_PORT 45678   //The port on which to listen for incoming data
std::mutex sendmessage_mutex;



void processMessages(json& db,json& pendingdb, const json& req_data, const string& requesterIP, std::atomic <int> &global_meet_id,
	SOCKET s, sockaddr_in server_struct, int server_struct_len, std::queue<socket_messages>& sending_messages_queue)
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
				json meetingInv = meeting::meetingObj_to_json(meeting("invite",
					" ",
					" ",
					meetingID,
					vector<string>{},
					vector<string>{},
					" ",
					req_data.at("topic"),
					day,
					time,
					requesterIP,
					false
				)
				);
				string toSend = meetingInv.dump();
				for (string ip : req_data.at("invitedParticipantsIP")) {
					sockaddr_in client = clientMaker(ip);
					send_message_client(ip,sending_messages_queue, toSend);
				}
				break;
			}
			else
			{
				// rooms not available - build unavailable reponse for the client
				// db will not be updated
				json unavailable = messages::response_unavail(req_data.at("requestID"));
				sockaddr_in client = clientMaker(requesterIP);
				string toSend = unavailable.dump();
				send_message_client(requesterIP,sending_messages_queue, toSend);
				// TODO: Send response to client 
			}
		}
	}
	else if (!(messageType.accept.compare(req_data.at("message"))) || !(messageType.add.compare(req_data.at("message"))))
	{
		//check if meeting minimum has been achieved. If yes, send confirmation with room. Else, wait.
		string meetingID = req_data.at("meetingID");
		bool found = false;
		json lookForMeeting = db_helper::getMeetingByID(db, meetingID);
		// check if meeting with ID exists.
		if (lookForMeeting != NULL) {
			vector<string> acceptedParticipants = lookForMeeting.at("confirmedParticipantsIP");
			vector<string> invitedParticipants = lookForMeeting.at("invitedParticipantsIP");
			string minNum = lookForMeeting.at("minimumParticipants");
			int min = stoi(minNum, nullptr, 10);
			//check if requester was invited.
			for (string confirms : invitedParticipants) {
				if (!confirms.compare(requesterIP)) {
					found = true;
				}
			}
			if (found == false) {
				return;
			}
			//check if requester had already accepted.
			for (string confirms : acceptedParticipants) {
				if (!confirms.compare(requesterIP)) {
					return;
				}
			}
			//check if acceptedparticipants number is larger than min. if yes, just add the accepting person to accepted array and send them confirmation
			if ((min-1) < acceptedParticipants.size()) {
				json accepting = messages::confirm_room(
					lookForMeeting.at("meetingID"),
					lookForMeeting.at("roomNumber"));
				acceptedParticipants.push_back(requesterIP);
				sockaddr_in client = clientMaker(requesterIP);
				string toSend = accepting.dump();
				send_message_client(requesterIP,sending_messages_queue, toSend);
			}
			// if just reached size of required people to accept, send confirmation to all
			else if ((min-1) == acceptedParticipants.size()) {
				acceptedParticipants.push_back(requesterIP);
				json accepting = messages::confirm_room(
					lookForMeeting.at("meetingID"),
					lookForMeeting.at("roomNumber"));
				for (string confirms : acceptedParticipants) {

					sockaddr_in client = clientMaker(confirms);
					string toSend = accepting.dump();
					send_message_client(confirms,sending_messages_queue, toSend);
				}
				meeting::update_meeting(pendingdb, req_data.at("day"), req_data.at("time"), req_data.at("roomNumber"), json({}));
			}
			else {
				//just add to accepted.
				acceptedParticipants.push_back(requesterIP);
			}
			lookForMeeting.at("confirmedParticipantsIP") = acceptedParticipants;
			meeting::update_meeting(db, req_data.at("day"), req_data.at("time"), req_data.at("roomNumber"), lookForMeeting);
			while (!sendmessage_mutex.try_lock()) {
				db_helper::save_db(config.CONFIRMED_DB_PATH, db);
				db_helper::save_db(config.PENDING_DB_PATH, pendingdb);
				sendmessage_mutex.unlock();
			}
		}
	}
	else if (!(messageType.reject.compare(req_data.at("message"))) || !(messageType.withdraw.compare(req_data.at("message"))))
	{
	//check if meeting minimum has been achieved. If yes, send confirmation with room. Else, wait.
	string meetingID = req_data.at("meetingID");
	bool found = false;
	json lookForMeeting = db_helper::getMeetingByID(db, meetingID);
	// check if meeting with ID exists.
		if (lookForMeeting != NULL) {
			vector<string> acceptedParticipants = lookForMeeting.at("confirmedParticipantsIP");
			vector<string> invitedParticipants = lookForMeeting.at("invitedParticipantsIP");
			string minNum = lookForMeeting.at("minimumParticipants");
			int min = stoi(minNum, nullptr, 10);
			//check if requester was invited.
			for (string confirms : invitedParticipants) {
				if (!confirms.compare(requesterIP)) {
					found = true;
				}
			}
			if (found == false) {
				return;
			}
			//check if requester had already accepted.
			for (string confirms : acceptedParticipants) {
				if (!confirms.compare(requesterIP)) {
					// if number dips below minimum, send cancel message.
					if (min == acceptedParticipants.size()) {
						vector<int>::iterator it;
						for (auto it = acceptedParticipants.begin(); it != acceptedParticipants.end(); ++it) {
							if (!requesterIP.compare(*it)) {
								acceptedParticipants.erase(it);
								break;
							}
						}
						json cancel = messages::cancel(lookForMeeting.at("meetingID"), "Not enough members");
						for (string confirms : acceptedParticipants) {
							sockaddr_in client = clientMaker(confirms);
							string toSend = cancel.dump();
							send_message_client(confirms,sending_messages_queue, toSend);
						}
					}
					//if number below or above minimum, don't send any messages. just remove participant from confirmed.
					else {
						vector<int>::iterator it;
						for (auto it = acceptedParticipants.begin(); it != acceptedParticipants.end(); ++it) {
							if (!requesterIP.compare(*it)) {
								acceptedParticipants.erase(it);
								break;
							}
						}
					}
				}
			}
			lookForMeeting.at("confirmedParticipantsIP") = acceptedParticipants;
			meeting::update_meeting(db, req_data.at("day"), req_data.at("time"), req_data.at("roomNumber"), lookForMeeting);
			while (!sendmessage_mutex.try_lock()) {
				db_helper::save_db(config.CONFIRMED_DB_PATH, db);
				db_helper::save_db(config.PENDING_DB_PATH, pendingdb);
				sendmessage_mutex.unlock();
			}
		}
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
			json cancelled = messages::cancel(
				meetObj.meetingID,
				"cancelled"
			);
			vector<string> invited = meetObj.invitedParticipantsIP;
			for (const string& participant : meetObj.invitedParticipantsIP)
			{
				// TODO: notify all participants of meeting cancellation
				for (string ip : invited) {
					sockaddr_in client = clientMaker(ip);
					string toSend = cancelled.dump();
					send_message_client(participant, sending_messages_queue, toSend);
				}
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
			while (!sendmessage_mutex.try_lock()) {
				db_helper::save_db(config.CONFIRMED_DB_PATH, db);
				sendmessage_mutex.unlock();
			}
		}
	}
}

void send_message_client(
	string ip,
	std::queue<socket_messages>& sending_messages_queue,
	string& msg)
{
	// char buf[BUFLEN];
	// memset(buf, '\0', BUFLEN + 1);
	socket_messages newmsg;
	newmsg.ip_for_message = ip;
	newmsg.message = msg;

	queueHelper::push_to_queue(sending_messages_queue, newmsg);

}

sockaddr_in clientMaker(string requesterIP) {
	sockaddr_in client;
	client.sin_family = AF_INET;
	if (client.sin_addr.s_addr = inet_pton(AF_INET, requesterIP.c_str(), &client.sin_addr.s_addr) == NULL) {
		printf("Invalid address: %s\n", requesterIP.c_str());
		exit(EXIT_FAILURE);
	}
	else if (client.sin_addr.s_addr == -1) {
		perror("inet_pton");
		exit(EXIT_FAILURE);
	}
	client.sin_port = htons(SERVER_PORT);
	return client;
}