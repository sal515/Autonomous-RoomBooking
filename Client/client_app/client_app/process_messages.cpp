#include "pch.h"
#include "process_messages.h"

// void (json& db, const json& req_data) {

void processMessages(json& db, vector<json> invitations_db, const json& req_data) {
	// scheduled that meeting is happening
	// not_scheduled
	// response unavailable
	// confirmed x
	// invitation

	bool exit = false;
	//meeting req_meeting = meeting::json_to_meetingObj(req_data);

	//invite
	if (!(messageType.invite.compare(req_data.at("message")))) {

		meeting myMeeting = meeting(req_data.at("message"), "", "", req_data.at("meetingID"), {}, {}, "", req_data.at("topic"),
			req_data.at("meetingDay"), req_data.at("meetingTime"), req_data.at("requesterIP"), false, false);
		db_helper::add_invitation(myMeeting, invitations_db);

	}
	//confirm
	else if (!(messageType.confirm.compare(req_data.at("message")))) {

		meeting myMeeting = meeting();

		if (db_helper::find_invitation(req_data.at("meetingID"), myMeeting, invitations_db)) {
			db_helper::remove_invitation(req_data.at("meetingID"), invitations_db);

			myMeeting.server_confirmation = true;
			string roomNumber = req_data.at("roomNumber");
			myMeeting.roomNumber = roomNumber;

			meeting::update_meeting(db, myMeeting.meetingDay, myMeeting.meetingTime, meeting::meetingObj_to_json(myMeeting));
			db_helper::save_db(config.DB_PATH, db);
		}
	}
	//cancel
	else if (!(messageType.cancelResponse.compare(req_data.at("message")))) {
		meeting myMeeting = meeting();
		if (db_helper::find_invitation(req_data.at("meetingID"), myMeeting, invitations_db)) {
			db_helper::remove_invitation(req_data.at("meetingID"), invitations_db);
			//cout << endl << "----------" << "Meeting " << req_data.at("meetingID") << " cancelled." << endl <<
				//"Number of participants is lower than minimum required." << endl;
			//TODO log Meeting Cancelled
		}
	}
	//response unavailable
	else if (!(messageType.response.compare(req_data.at("message")))) {
		string requestId = req_data.at("requestID");
		for (auto day : db) {
			for (auto time : day) {
				if (!requestId.compare(time.at("requestID"))) {
					meeting::update_meeting(db, time.at("meetingDay"), time.at("meetingTime"), json({}));
					db_helper::save_db(config.DB_PATH, db);
					//TODO  log Request denied
					//cout << endl << "----------" << "Request " << requestId << " denied. Room is unavailable." << endl;
					exit = true;
					break;
				}
			}
			if (exit) {
				break;
			}
		}
	}
	//Meeting Not Scheduled
	else if (!(messageType.notScheduled.compare(req_data.at("message")))) {
		string requestId = req_data.at("requestID");
		for (auto day : db) {
			for (auto time : day) {
				if (!requestId.compare(time.at("requestID"))) {
					meeting db_meeting = meeting::json_to_meetingObj(time);

					for (const string element : req_data["confirmedParticipantsIP"])
					{
						db_meeting.confirmedParticipantsIP.push_back(element);
					}

					//TODO log meeting NOT scheduled
					meeting::update_meeting(db, time.at("meetingDay"), time.at("meetingTime"), json({}));
					db_helper::save_db(config.DB_PATH, db);
					exit = true;
					break;
				}
			}
			if (exit) {
				break;
			}
		}
	}
	//Meeting scheduled successful
	else if (!messageType.scheduled.compare(req_data.at("message"))) {
		string requestId = req_data.at("requestID");
		for (auto day : db) {
			for (auto time : day) {
				if (!requestId.compare(time.at("requestID"))) {
					meeting thisMeeting = meeting::json_to_meetingObj(time);
					string meetingId = req_data.at("meetingID");
					string roomNumbr = req_data.at("roomNumber");
					thisMeeting.meetingID = meetingId;
					thisMeeting.roomNumber = roomNumbr;

					for (const string element : req_data["confirmedParticipantsIP"])
					{
						thisMeeting.confirmedParticipantsIP.push_back(element);
					}

					//TODO log schedule successful

					meeting::update_meeting(db, thisMeeting.meetingDay, thisMeeting.meetingTime, meeting::meetingObj_to_json(thisMeeting));
					db_helper::save_db(config.DB_PATH, db);

				}
			}
		}
	}
	//Added Participant to Meeting
	else if (!messageType.added.compare(req_data.at("message"))) {
		string meetingId = req_data.at("meetingID");
		for (auto day : db) {
			for (auto time : day) {
				if (!meetingId.compare(time.at("meetingID"))) {
					meeting thisMeeting = meeting::json_to_meetingObj(time);

					thisMeeting.confirmedParticipantsIP.push_back(req_data.at("IP"));
					meeting::update_meeting(db, thisMeeting.meetingDay, thisMeeting.meetingTime, meeting::meetingObj_to_json(thisMeeting));
					db_helper::save_db(config.DB_PATH, db);
					exit = true;

				}
				if (exit) {
					break;
				}
			}
			if (exit) {
				break;
			}
		}
	}
	//Withdraw from Meeting (Requester side)
	else if (!messageType.withdrawNotify.compare(req_data.at("message"))) {
		string meetingId = req_data.at("meetingID");
		for (auto day : db) {
			for (auto time : day) {
				if (!meetingId.compare(time.at("meetingID"))) {
					meeting thisMeeting = meeting::json_to_meetingObj(time);

					int counter = -1;

					for (const string element : thisMeeting.confirmedParticipantsIP)
					{
						counter += 1;
						if (!element.compare(req_data.at("IP"))) {

							thisMeeting.confirmedParticipantsIP.erase(thisMeeting.confirmedParticipantsIP.begin + counter);
							meeting::update_meeting(db, thisMeeting.meetingDay, thisMeeting.meetingTime, meeting::meetingObj_to_json(thisMeeting));
							db_helper::save_db(config.DB_PATH, db);
							exit = true;
						}
						if (exit) {
							break;
						}
					}
				}
				if (exit) {
					break;
				}
			}
			if (exit) {
				break;
			}
		}
	}
	//Room_Change
	else if (!messageType.roomChange.compare(req_data.at("message"))) {
		string meetingId = req_data.at("meetingID");
		for (auto day : db) {
			for (auto time : day) {
				if (!meetingId.compare(time.at("meetingID"))) {
					meeting thisMeeting = meeting::json_to_meetingObj(time);

					string newRoom = req_data.at("newRoom");
					thisMeeting.roomNumber = newRoom;

					meeting::update_meeting(db, thisMeeting.meetingDay, thisMeeting.meetingTime, meeting::meetingObj_to_json(thisMeeting));
					db_helper::save_db(config.DB_PATH, db);
					exit = true;
				}
				if (exit) {
					break;
				}
			}
			if (exit) {
				break;
			}
		}
	}


}