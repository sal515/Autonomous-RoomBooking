#include "pch.h"
#include "process_messages.h"

// void (json& db, const json& req_data) {





void processMessages(json& db, const json& req_data) {
	// scheduled that meeting is happening
	// not_scheduled
	// response unavailable
	//	confirmed
	// invitation
	bool checkTrue = true;
	meeting req_meeting = meeting::json_to_meetingObj(req_data);
	if (!(messageType.confirm.compare(req_data.at("message")))) {
		for (auto day : db) {
			for (auto time : day) {
				if (!req_meeting.meetingID.compare(time.at("meetingID"))) {
					meeting thisMeeting = meeting::json_to_meetingObj(time);
					thisMeeting.server_confirmation = true;
					// meeting::update_meeting(db, thisMeeting.meetingDay, 
							// thisMeeting.meetingTime, meeting::meetingObj_to_json(thisMeeting));
					checkTrue = false;
					break;
				}
			}
			if (!checkTrue) {
				break;
			}
		}
	}
	//response unavail
	else if (!(messageType.response.compare(req_data.at("message")))) {
		for (auto day : db) {
			for (auto time : day) {
				if (!req_meeting.requestID.compare(time.at("requestID"))) {
					// meeting::update_meeting(db, time.at("day"),
						// time.at("time"), json({}));
					checkTrue = false;
					break;
				}
			}
			if (!checkTrue) {
				break;
			}
		}
	}
	else if (!(messageType.notScheduled.compare(req_data.at("message")))) {
		for (auto day : db) {
			for (auto time : day) {

				if (!req_meeting.requestID.compare(time.at("requestID"))) {
					// meeting::update_meeting(db, time.at("day"),
						// time.at("time"), json({}));
					checkTrue = false;
					break;
				}
				
			}
			if (!checkTrue) {
				break;
			}
		}
	}
	else if (!messageType.scheduled.compare(req_data.at("message"))) {
		for (auto day : db) {
			for (auto time : day) {
				if (!req_meeting.requestID.compare(time.at("requestID"))) {
					meeting thisMeeting = meeting::json_to_meetingObj(time);
					//thisMeeting.
					//meeting::get_meeting(db, time.at("day"), time.at("time"),)
				}
			}
		}
	}

}