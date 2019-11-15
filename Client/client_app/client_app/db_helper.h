#pragma once

#include <string>
#include "json.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <map>

#include "time_day_room.h"

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;
using std::vector;
using std::map;


struct db_helper
{
	static bool createDirectory(std::string dirName);
	static bool isDirectoryExist(string dirName);
	static string json_to_string(json ajson);
	static bool removeDirectory(string relativeDirName);
	/*
	 * Usage example: json db = db_helper::db_to_json(dbPath);
	 */
	static void initialize_db(const string& dbPath, const bool &isClient);
	static json db_to_json(const string& dbPath);
	static bool save_db(const string& dbPath, const json& db);
};
