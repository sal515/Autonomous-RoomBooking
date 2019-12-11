#pragma once
#include "pch.h"

#include "json.hpp"

using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;
using std::vector;
using std::map;

struct logger
{
	static void initialize_log_file(std::string logPath);
	// static void is_Log_File_Exist(std::string logPath);
	// static void remove_Log_File(std::string logPath);
	static void add_received_log(std::string logPath, const json& msg);
	static void add_sent_log(std::string logPath, const json& msg);

	static string get_time();

};