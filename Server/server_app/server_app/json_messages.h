#pragma once
#include "pch.h"
#ifndef MSGS_STR_H
#define MSGS_STR_H

using json = nlohmann::json;
using std::string;

static const struct messageType
{
	string request = "REQUEST";
	string response = "RESPONSE";
	string invite = "INVITE";
	string accept = "ACCEPT";
	string reject = "REJECT";
	string confirm = "CONFIRM";
	string scheduled = "SCHEDULED";
	string cancelResponse = "CANCEL";
	string notScheduled = "NOT_SCHEDULED";
	string cancelRequest = "CANCEL";
	string withdraw = "WITHDRAW";
	string withdrawNotify = "WITHDRAW";
	string add = "ADD";
	string added = "ADDED";
	string roomChange = "ROOM_CHANGE";
} messageType;


struct messages
{
	static json request(
		string requestID,
		string meetingDay,
		string meetingTime,
		string minimumParticipants,
		vector<string> listofParticipants,
		string topic
		// string meetingStatus,
		// string roomNumber,
		// json& db
	)
	{
		json req;
		// json listOfPart(listofParticipants);
		// json room;

		req["message"] = messageType.request;
		req["requestID"] = requestID;
		req["meetingDay"] = meetingDay;
		req["meetingTime"] = meetingTime;
		req["invitedParticipantsIP"] = listofParticipants;
		req["minimumParticipants"] = minimumParticipants;
		req["topic"] = topic;
		// req["meetingStatus"] = meetingStatus;
		// req["roomNumber"] = roomNumber;
		// room[roomNumber] = req;
		return req;
	}

	static json response_unavail(string requestID)
	{
		json deny;
		deny["message"] = messageType.response;
		deny["requestID"] = requestID;
		deny["meetingStatus"] = "UNAVAILABLE";
		return deny;
	}

	static json invite(
		string meetingID,
		string meetingDay,
		string meetingTime,
		string topic,
		string requesterIP)
	{
		json invitation;
		invitation["message"] = messageType.invite;
		invitation["meetingID"] = meetingID;
		invitation["meetingDay"] = meetingDay;
		invitation["meetingTime"] = meetingTime;
		invitation["topic"] = topic;
		invitation["requesterIP"] = requesterIP;
		return invitation;
	}

	static json accept_inv(string meetingID)
	{
		json accept;
		accept["message"] = messageType.accept;
		accept["meetingID"] = meetingID;
		return accept;
	}

	static json reject_inv(string meetingID)
	{
		json deny;
		deny["message"] = messageType.reject;
		deny["meetingID"] = meetingID;
		return deny;
	}

	static json confirm_room(string meetingID, string roomNumber)
	{
		json confirmation;
		confirmation["message"] = messageType.confirm;
		confirmation["meetingID"] = meetingID;
		confirmation["roomNumber"] = roomNumber;
		return confirmation;
	}

	static json scheduled(
		string requestID,
		string meetingID,
		string roomNumber,
		vector<string> confirmedParticipantsIP
	)
	{
		json sched;
		sched["message"] = messageType.scheduled;
		sched["requestID"] = requestID;
		sched["meetingID"] = meetingID;
		sched["roomNumber"] = roomNumber;
		sched["confirmedParticipantsIP"] = confirmedParticipantsIP;
		return sched;
	}

	static json cancel(string meetingID, string reason)
	{
		json cancel;
		cancel["message"] = messageType.cancelResponse;
		cancel["meetingID"] = meetingID;
		cancel["reason"] = reason;

		return cancel;
	}

	static json not_sched(
		string requestID,
		string meetingDay,
		string meetingTime,
		string minimumParticipants,
		vector<string> confirmedParticipantsIP,
		string topic)
	{
		json notSched;
		notSched["message"] = messageType.notScheduled;
		notSched["requestID"] = requestID;
		notSched["meetingDay"] = meetingDay;
		notSched["meetingTime"] = meetingTime;
		notSched["minimumParticipants"] = minimumParticipants;
		notSched["confirmedParticipantsIP"] = confirmedParticipantsIP;
		notSched["topic"] = topic;

		return notSched;
	}

	static json cancelMeet(string meetingID)
	{
		json cancel;
		cancel["message"] = messageType.cancelRequest;
		cancel["meetingID"] = meetingID;
		return cancel;
	}

	static json withdraw(string meetingID)
	{
		json withdrawal;
		withdrawal["message"] = messageType.withdraw;
		withdrawal["meetingID"] = meetingID;
		return withdrawal;
	}

	static json withdrawIP(string meetingID, string IP)
	{
		json withdrawal;
		withdrawal["message"] = messageType.withdrawNotify;
		withdrawal["meetingID"] = meetingID;
		withdrawal["IP"] = IP;
		return withdrawal;
	}

	static json add(string meetingID)
	{
		json adder;
		adder["message"] = messageType.add;
		adder["meetingID"] = meetingID;
		return adder;
	}

	static json confirm(string meetingID, string roomNumber)
	{
		json confirmation;
		confirmation["message"] = messageType.confirm;
		confirmation["meetingID"] = meetingID;
		confirmation["roomNumber"] = roomNumber;
		return confirmation;
	}

	static json added(string meetingID, string IP)
	{
		json addedP;
		addedP["message"] = messageType.added;
		addedP["meetingID"] = meetingID;
		addedP["IP"] = IP;
		return addedP;
	}

	static json room_change(string meetingID, string newRoomNum)
	{
		json room;
		room["message"] = messageType.roomChange;
		room["meetingID"] = meetingID;
		room["newRoom"] = newRoomNum;
		return room;
	}
};
#endif // !MSGS_STR_H
