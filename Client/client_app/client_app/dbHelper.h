#pragma once
#include <string>

#include <string>
#include "json.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;

class dbHelper
{
public:
	dbHelper();
	~dbHelper();
	static bool createDirectory(std::string dirName);
	static bool isDirectoryExist(string dirName);
	static string json_to_string(json ajson);
	static bool removeDirectory(string relativeDirName);
	static string read_db(string dbPath);
	static bool write_db(string dbPath, string data);
};

