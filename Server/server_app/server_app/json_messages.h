#pragma once
#include "pch.h"
#ifndef MSGS_STR_H
#define MSGS_STR_H

using json = nlohmann::json;


struct messages {
	json request(std::string requestID,
		std::string day, std::string time,
		std::vector<std::string> list, std::vector<std::string> confirmed,
		std::string minimum, std::string topic,
		std::string meetingStatus, std::string roomNumber, std::string requesterIP,
		json &db) {
		json req;
		json listOfPart(list);
		json listOfConf(confirmed);
		req["message"] = "REQUEST";
		req["requestID"] = requestID;
		req["day"] = day;
		req["time"] = time;
		req["invitedParticipantsIP"] = listOfPart;
		req["confirmedParticipantsIP"] = confirmed;
		req["minimumParticipants"] = minimum;
		req["topic"] = topic;
		req["meetingStatus"] = meetingStatus;
		req["roomNumber"] = roomNumber;
		req["requesterIP"] = requesterIP;
		db.at(day).at(time).update(req);
		return req;
	}

	json response_unavail(std::string requestID) {
		json deny;
		deny["message"] = "RESPONSE";
		deny["requestID"] = requestID;
		deny["status"] = "UNAVAILABLE";
		return deny;
	}

	json invite(std::string meetingID, std::string day, std::string time,
		std::string topic, std::string requesterIP) {
		json invitation;
		invitation["message"] = "INVITE";
		invitation["meetingID"] = meetingID;
		invitation["day"] = day;
		invitation["time"] = time;
		invitation["topic"] = topic;
		invitation["requesterIP"] = requesterIP;
		return invitation;
	}

	json accept_inv(std::string meetingID) {
		json accept;
		accept["message"] = "ACCEPT";
		accept["meetingID"] = meetingID;
		return accept;
	}

	json reject_inv(std::string meetingID) {
		json deny;
		deny["message"] = "REJECT";
		deny["meetingID"] = meetingID;
		return deny;
	}

	json confirm_room(std::string meetingID, std::string roomNumber) {
		json confirmation;
		confirmation["message"] = "CONFIRM";
		confirmation["meetingID"] = meetingID;
		confirmation["roomNumber"] = roomNumber;
		return confirmation;
	}

	json scheduled(std::string requestID, std::string meetingID,
		std::string roomNumber, std::vector<std::string> list, std::vector<std::string> unconfirmed) {
		json sched;
		sched["message"] = "SCHEDULED";
		sched["requestID"] = requestID;
		sched["meetingID"] = meetingID;
		sched["roomNumber"] = roomNumber;
		json listOfPart(list);
		sched["confirmedParticipantsIP"] = listOfPart;
		json listUnconfirmed(unconfirmed);
		sched["invitedParticipantsIP"] = listUnconfirmed;
		return sched;
	}

	json cancel(std::string meetingID, std::string reason) {
		json cancel;
		cancel["message"] = "CANCEL";
		cancel["meetingID"] = meetingID;
		cancel["reason"] = reason;
	}

	json not_sched(std::string requestID, std::string day,
		std::string time, std::string minimum,
		std::vector<std::string> list, std::string topic) {
		json notSched;
		notSched["message"] = "NOT_SCHEDULED";
		notSched["requestID"] = requestID;
		notSched["day"] = day;
		notSched["time"] = time;
		notSched["minimum"] = minimum;
		json listOfPart(list);
		notSched["confirmedParticipantsIP"] = listOfPart;
		notSched["topic"] = topic;

		return notSched;
	}

	json cancelMeet(std::string meetingID) {
		json cancel;
		cancel["message"] = "CANCEL";
		cancel["meetingID"] = meetingID;
		return cancel;
	}

	json withdraw(std::string meetingID) {
		json withdrawal;
		withdrawal["message"] = "WITHDRAW";
		withdrawal["meetingID"] = meetingID;
		return withdrawal;
	}

	json withdrawIP(std::string meetingID, std::string IP) {
		json withdrawal;
		withdrawal["message"] = "WITHDRAW";
		withdrawal["meetingID"] = meetingID;
		withdrawal["IP"] = IP;
		return withdrawal;
	}

	json add(std::string meetingID) {
		json adder;
		adder["message"] = "ADD";
		adder["meetingID"] = meetingID;
		return adder;
	}

	json confirm(std::string meetingID, std::string roomNumber) {
		json confirmation;
		confirmation["message"] = "CONFIRM";
		confirmation["meetingID"] = meetingID;
		confirmation["roomNumber"] = roomNumber;
		return confirmation;
	}

	json added(std::string meetingID, std::string IP) {
		json addedP;
		addedP["message"] = "ADDED";
		addedP["meetingID"] = meetingID;
		addedP["IP"] = IP;
		return addedP;
	}

	json room_change(std::string meetingID, std::string newRoomNum) {
		json room;
		room["message"] = "ROOM_CHANGE";
		room["meetingID"] = meetingID;
		room["newRoom"] = newRoomNum;
		return room;
	}

};
#endif // !MSGS_STR_H