#include "pch.h"

#include <iostream>

#include "json.hpp"
#include "db_helper.h"
#include "app_config.h"
#include "meeting.h"

using std::cout;
using std::cin;
using std::endl;

int Testing_dbHelper_meetingObj()
{
	/*Testing:
	 *Load db to memory and convert a meetingObj to JSON
	 *And store it in the db in memory.
	 *Then save to the file.
	*/

	nlohmann::json db = db_helper::db_to_json(config.DB_PATH);

	json meetInfo = meeting::client_get_meeting(
		db,
		"friday",
		"6");

	cout << "Checking if the database already exists: " <<
		meeting::client_isMeeting(db, "friday", "6")
		<< endl;

	// create a meeting object
	// meeting meetObj(
	// 	"1",
	// 	"2",
	// 	"2",
	// 	"2",
	// 	vector<string>{"192.165.54.1"},
	// 	vector<string>{"192.111.111.111"},
	// 	"EV005.251",
	// 	"First Test meeting object",
	// 	"friday",
	// 	"192.165.54.1",
	// 	false
	// );

	// cout << "meeting object updated: " <<
	// 	meeting::client_update_meeting(
	// 		db,
	// 		"friday",
	// 		"6",
	// 		meeting::meetingObj_to_json(meetObj)
	// 	)
	// 	<< endl;
	//
	//
	// cout << "database saved to file: " <<
	// 	db_helper::save_db(config.DB_PATH, db)
	// 	<< endl;
	//

	/*
	 *Testing:
	 *Load db to memory
	 *and parse json object loaded from file to meetingObject in memory
	 */

	json db_loaded_from_file_again = db_helper::db_to_json(config.DB_PATH);

	json meetInfo_loaded_from_file = meeting::client_get_meeting(
		db,
		"friday",
		"6");

	meeting extracted_meetingObj_from_file = meeting::json_to_meetingObj(meetInfo_loaded_from_file);

	return 0;
}
