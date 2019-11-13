#pragma once
#include <string>
#include "json.hpp"

using std::string;
using json = nlohmann::json;

struct user
{
	string ip;
	string listeningPort;
	string userName;

	static json userObj_to_json(const user &usr);
};
