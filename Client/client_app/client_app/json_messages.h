#pragma once
#include "pch.h"
#ifndef MSGS_STR_H
#define MSGS_STR_H

using json = nlohmann::json;

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
	json request(
		std::string requestID,
		std::string day,
		std::string time,
		std::vector<std::string> list,
		std::string minimum,
		std::string topic,
		std::string meetingStatus,
		std::string roomID,
		json& db)
	{
		json req;
		json room;

		req["message"] = messageType.request;
		req["requestID"] = requestID;
		req["day"] = day;
		req["time"] = time;
		json listOfPart(list);
		req["participantsIP"] = listOfPart;
		req["minimum"] = minimum;
		req["topic"] = topic;
		req["meetingStatus"] = meetingStatus;

		req["roomID"] = roomID;
		room[roomID] = req;
		db.at(day).at(time).update(req);
		return req;
	}

	json response_unavail(std::string requestID)
	{
		json deny;
		deny["message"] = messageType.response;
		deny["requestID"] = requestID;
		deny["status"] = "UNAVAILABLE";
		return deny;
	}

	json invite(
		std::string meetingID,
		std::string day,
		std::string time,
		std::string topic,
		std::string requesterIP)
	{
		json invitation;
		invitation["message"] = messageType.invite;
		invitation["meetingID"] = meetingID;
		invitation["day"] = day;
		invitation["time"] = time;
		invitation["topic"] = topic;
		invitation["requesterIP"] = requesterIP;
		return invitation;
	}

	json accept_inv(std::string meetingID)
	{
		json accept;
		accept["message"] = messageType.accept;
		accept["meetingID"] = meetingID;
		return accept;
	}

	json reject_inv(std::string meetingID)
	{
		json deny;
		deny["message"] = messageType.reject;
		deny["meetingID"] = meetingID;
		return deny;
	}

	json confirm_room(std::string meetingID, std::string roomID)
	{
		json confirmation;
		confirmation["message"] = messageType.confirm;
		confirmation["meetingID"] = meetingID;
		confirmation["roomID"] = roomID;
		return confirmation;
	}

	json scheduled(
		std::string requestID,
		std::string meetingID,
		std::string roomID,
		std::vector<std::string> list,
		std::vector<std::string> unconfirmed)
	{
		json sched;
		sched["message"] = messageType.scheduled;
		sched["requestID"] = requestID;
		sched["meetingID"] = meetingID;
		sched["roomID"] = roomID;
		json listOfPart(list);
		sched["confirmedParticipantsIP"] = listOfPart;
		json listUnconfirmed(unconfirmed);
		sched["invitedParticipantsIP"] = listUnconfirmed;
		return sched;
	}

	json cancel(std::string meetingID, std::string reason)
	{
		json cancel;
		cancel["message"] = messageType.cancelResponse;
		cancel["meetingID"] = meetingID;
		cancel["reason"] = reason;
	}

	json not_sched(
		std::string requestID,
		std::string day,
		std::string time,
		std::string minimum,
		std::vector<std::string> list,
		std::string topic)
	{
		json notSched;
		notSched["message"] = messageType.notScheduled;
		notSched["requestID"] = requestID;
		notSched["day"] = day;
		notSched["time"] = time;
		notSched["minimum"] = minimum;
		json listOfPart(list);
		notSched["confirmedParticipantsIP"] = listOfPart;
		notSched["topic"] = topic;

		return notSched;
	}

	json cancelMeet(std::string meetingID)
	{
		json cancel;
		cancel["message"] = messageType.cancelRequest;
		cancel["meetingID"] = meetingID;
		return cancel;
	}

	json withdraw(std::string meetingID)
	{
		json withdrawal;
		withdrawal["message"] = messageType.withdraw;
		withdrawal["meetingID"] = meetingID;
		return withdrawal;
	}

	json withdrawIP(std::string meetingID, std::string IP)
	{
		json withdrawal;
		withdrawal["message"] = messageType.withdrawNotify;
		withdrawal["meetingID"] = meetingID;
		withdrawal["IP"] = IP;
		return withdrawal;
	}

	json add(std::string meetingID)
	{
		json adder;
		adder["message"] = messageType.add;
		adder["meetingID"] = meetingID;
		return adder;
	}

	json confirm(std::string meetingID, std::string roomID)
	{
		json confirmation;
		confirmation["message"] = messageType.confirm;
		confirmation["meetingID"] = meetingID;
		confirmation["roomID"] = roomID;
		return confirmation;
	}

	json added(std::string meetingID, std::string IP)
	{
		json addedP;
		addedP["message"] = messageType.added;
		addedP["meetingID"] = meetingID;
		addedP["IP"] = IP;
		return addedP;
	}

	json room_change(std::string meetingID, std::string newRoomNum)
	{
		json room;
		room["message"] = messageType.roomChange;
		room["meetingID"] = meetingID;
		room["newRoom"] = newRoomNum;
		return room;
	}
};
#endif // !MSGS_STR_H
