#pragma once
#include "main_headers.h"

namespace fs = std::filesystem;
using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;
using std::vector;
bool checkSchedule(std::string date, std::string time, std::string roomID, json db);