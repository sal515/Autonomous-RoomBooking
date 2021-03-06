﻿#include "pch.h"
#include "logger.h"


void logger::initialize_log_file(std::string logPath)
{
	const json logArray = json::array();

	try
	{
		const fs::path p(logPath);
		if (!fs::exists(p))
		{
			std::ofstream writeFile(logPath);
			writeFile << std::setw(4) << logArray << std::endl;
			writeFile.close();

			cout << "Client log file was created " << logPath << endl;
		}
		else
		{
			cout << "Client log file already exits " << logPath << endl;
		}
	}
	catch (fstream::failure& e)
	{
		cout << "Exception: initialize_log_file method throws -> " << e.what() << endl;
	}
}

void logger::add_received_log(std::string logPath, const socket_messages& msg)
{
	json log_db = db_helper::db_to_jsonArr (logPath);

	json newLogObj = json({});
	newLogObj["received at time"] = get_time();
	newLogObj["received from Client IP"] = msg.ip_for_message;
	newLogObj["received message details"] = msg.message;

	log_db.push_back(newLogObj);

	try
	{
		while (!db_helper::save_db(logPath, log_db));
	}
	catch (...)
	{
		cout << "Failed to received logs" << endl;
	}
}

void logger::add_sent_log(std::string logPath, const socket_messages& msg)
{
	json log_db = db_helper::db_to_jsonArr (logPath);

	json newLogObj = json({});
	newLogObj["sent at time"] = get_time();
	newLogObj["sent to Client IP"] = msg.ip_for_message;
	newLogObj["sent message details"] = msg.message;

	log_db.push_back(newLogObj);

	try
	{
		while (!db_helper::save_db(logPath, log_db));
	}
	catch (...)
	{
		cout << "Failed to sent logs" << endl;
	}
}

string logger::get_time()
{
	try
	{
		time_t result = time(NULL);
		char str[100];
		memset(str, '\0', 100);
		ctime_s(str, sizeof(str), &result);
		// string dateTime = string(str);
		// cout << dateTime << endl;
		return string(str);
	}
	catch (...)
	{
		cout << "Error occured to get Current Time" << endl;
		return "";
	}
}
