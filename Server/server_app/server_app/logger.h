#pragma once
#include "pch.h"


struct logger
{
	static void initialize_log_file(std::string logPath);
	// static void is_Log_File_Exist(std::string logPath);
	// static void remove_Log_File(std::string logPath);
	static void add_received_log(std::string logPath, const socket_messages& msg);
	static void add_sent_log(std::string logPath, const socket_messages& msg);

	static string get_time();

};
