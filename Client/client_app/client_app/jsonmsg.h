#pragma once

#include "pch.h"

using json = nlohmann::json;


struct messages {
	json request(std::string reqNum, std::string date,
		std::string time, std::string minimum,
		std::vector<std::string> list, std::string topic) {
		json req;
		req["message"] = "REQUEST";
		req["reqNum"] = reqNum;
		req["date"] = date;
		req["time"] = time;
		req["minimum"] = minimum;
		json listOfPart(list);
		req["listOfParticipants"] = listOfPart;
		req["topic"] = topic;
		return req;
	}

	json response_unavail(std::string reqNum) {
		json deny;
		deny["message"] = "RESPONSE";
		deny["reqNum"] = reqNum;
		deny["status"] = "UNAVAILABLE";
		return deny;
	}

	json invite(std::string meetNum, std::string date, std::string time,
		std::string topic, std::string requester) {
		json invitation;
		invitation["message"] = "INVITE";
		invitation["meetNum"] = meetNum;
		invitation["date"] = date;
		invitation["time"] = time;
		invitation["topic"] = topic;
		invitation["requester"] = requester;
		return invitation;
	}

	json accept_inv(std::string meetNum) {
		json accept;
		accept["message"] = "ACCEPT";
		accept["meetNum"] = meetNum;
		return accept;
	}

	json reject_inv(std::string meetNum) {
		json deny;
		deny["message"] = "REJECT";
		deny["meetNum"] = meetNum;
		return deny;
	}

	json confirm_room(std::string meetNum, std::string roomNum) {
		json confirmation;
		confirmation["message"] = "CONFIRM";
		confirmation["meetNum"] = meetNum;
		confirmation["roomNum"] = roomNum;
		return confirmation;
	}

	json scheduled(std::string reqNum, std::string meetNum,
		std::string roomNum, std::vector<std::string> list) {
		json sched;
		sched["message"] = "SCHEDULED";
		sched["reqNum"] = reqNum;
		sched["meetNum"] = meetNum;
		sched["roomNum"] = roomNum;
		json listOfPart(list);
		sched["listOfParticipants"] = listOfPart;
		return sched;
	}

	json cancel(std::string meetNum, std::string reason) {
		json cancel;
		cancel["message"] = "CANCEL";
		cancel["meetNum"] = meetNum;
		cancel["reason"] = reason;
	}

	json not_sched(std::string reqNum, std::string date,
		std::string time, std::string minimum,
		std::vector<std::string> list, std::string topic) {
		json notSched;
		notSched["message"] = "NOT_SCHEDULED";
		notSched["reqNum"] = reqNum;
		notSched["date"] = date;
		notSched["time"] = time;
		notSched["minimum"] = minimum;
		json listOfPart(list);
		notSched["listOfParticipants"] = listOfPart;
		notSched["topic"] = topic;

		return notSched;
	}

	json cancelMeet(std::string meetNum) {
		json cancel;
		cancel["message"] = "CANCEL";
		cancel["meetNum"] = meetNum;
		return cancel;
	}

	json withdraw(std::string meetNum) {
		json withdrawal;
		withdrawal["message"] = "WITHDRAW";
		withdrawal["meetNum"] = meetNum;
		return withdrawal;
	}

	json withdrawIP(std::string meetNum, std::string IP) {
		json withdrawal;
		withdrawal["message"] = "WITHDRAW";
		withdrawal["meetNum"] = meetNum;
		withdrawal["IP"] = IP;
		return withdrawal;
	}

	json add(std::string meetNum) {
		json adder;
		adder["message"] = "ADD";
		adder["meetNum"] = meetNum;
		return adder;
	}

	json confirm(std::string meetNum, std::string roomNum) {
		json confirmation;
		confirmation["message"] = "CONFIRM";
		confirmation["meetNum"] = meetNum;
		confirmation["roomNum"] = roomNum;
		return confirmation;
	}

	json added(std::string meetNum, std::string IP) {
		json addedP;
		addedP["message"] = "ADDED";
		addedP["meetNum"] = meetNum;
		addedP["IP"] = IP;
		return addedP;
	}

	json room_change(std::string meetNum, std::string newRoomNum) {
		json room;
		room["message"] = "ROOM_CHANGE";
		room["meetNum"] = meetNum;
		room["newRoom"] = newRoomNum;
		return room;
	}

};