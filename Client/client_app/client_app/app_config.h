#pragma once

#include "pch.h"
using std::string;


static const struct app_config
{
	// Debug IP address
	 // string DEBUG_HARDCODED_SERVER_IP_IN = "192.168.1.133"; 	
	 string DEBUG_HARDCODED_SERVER_IP_IN = "192.168.0.183"; 	
	// string DEBUG_HARDCODED_SERVER_IP_IN = "172.31.8.16";

	// Diretory paths
	string DIR_LOCAL_STORAGE = "local_storage";
	string SENT_RECEIVED_LOG_PATH = "local_storage/sent_received_log.json";
	string DB_PATH = "local_storage/db.json";
	string INVITATIONS_PATH = "local_storage/invitation.json";
	string EXAMPLE_DB_PATH = "local_storage/example_db.json";
} config;
