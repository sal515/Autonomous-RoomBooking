#pragma once
#include <string>

#include <string>
#include "json.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;
using std::vector;

class dbHelper
{
public:
	dbHelper();
	~dbHelper();
	static bool createDirectory(std::string dirName);
	static bool isDirectoryExist(string dirName);
	static string json_to_string(json ajson);
	static bool removeDirectory(string relativeDirName);
	/*
	 * Usage example: json db = dbHelper::db_to_json(dbPath);
	 */
	static json db_to_json(const string &dbPath); 
	static bool update_event(json &db, const string &day, const string &time, const json &event);
	static bool update_db(const string &dbPath, const json &db);



	// Example methods
	static void example_db_creator(string example_db_path);
};

