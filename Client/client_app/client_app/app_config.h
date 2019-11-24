#pragma once

#include "pch.h"
using std::string;


static const struct app_config
{
	// Debug IP address
	// string DEBUG_HARDCODED_SERVER_IP_IN = "192.168.1.133"; 	
	string DEBUG_HARDCODED_SERVER_IP_IN = "192.168.0.115"; 	

	// Diretory paths
	string DIR_LOCAL_STORAGE = "local_storage";
	string LOG_PATH = "local_storage/log.json";
	string DB_PATH = "local_storage/db.json";
	string EXAMPLE_DB_PATH = "local_storage/example_db.json";

}config;
