#pragma once

#include "json.hpp"

struct socket_messages
{
	std::string ip_for_message;
	nlohmann::json message;
};
