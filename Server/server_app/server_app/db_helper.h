#pragma once

#include "pch.h"
#include "json.hpp"

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
	static void initialize_db(const string& dbPath, const bool &is
	);
	static json db_to_json(const string& dbPath);
	static bool save_db(const string& dbPath, const json& db);

	static json getMeetingByID(const json &db, const string meetingID);
};
