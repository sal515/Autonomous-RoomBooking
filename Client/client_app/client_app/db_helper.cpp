#include "pch.h"
#include "db_helper.h"


bool db_helper::createDirectory(string relativeDirName)
{
	try
	{
		if (fs::create_directory(relativeDirName))
		{
			cout << "Local storage directory was created" << endl;
			return true;
		}
		else
		{
			if (db_helper::isDirectoryExist(relativeDirName))
			{
				cout << "Directory \"" << relativeDirName << "\" already exists" << endl;
				return true;
			}

			cout << "Error: Creating file \"" << relativeDirName << "\"" << endl;
			return false;
		}
	}
	catch (fs::filesystem_error e)
	{
		cout << "Exception: createDirectory method throws -> " << e.what() << endl;
		return false;
	}
}

bool db_helper::isDirectoryExist(string relativeDirName)
{
	try
	{
		if (fs::exists(relativeDirName))
		{
			return true;
		}
		return false;
	}
	catch (fs::filesystem_error e)
	{
		cout << "Exception: isDirectoryExist method throws -> " << e.what() << endl;
		return false;
	}
}


string db_helper::json_to_string(json ajson)
{
	return ajson.dump();
}


bool db_helper::removeDirectory(string relativeDirName)
{
	try
	{
		if (db_helper::isDirectoryExist(relativeDirName))
		{
			fs::remove(relativeDirName);
			cout << "The directory \"" << relativeDirName << "\" was removed" << endl;
			return true;
		};
		return false;
	}
	catch (fs::filesystem_error e)
	{
		cout << "Exception: removeDirectory method throws -> " << e.what() << endl;
		return false;
	}
}

void db_helper::initialize_db(const string& dbPath)
{
		json rooms;
		json day;
		json time;

		vector<string> rooms_vec = time_day_room::room_vec();
		for (string room : rooms_vec)
		{
			rooms[room] = json({});
		}

		map<string, string> time_map = time_day_room::time_map(time_day_room::startTime, time_day_room::endTime);
		for (const auto &element : time_map)
		{
			time[element.first] = rooms;
		}

		map<string, string> day_map = time_day_room::day_map();
		for (const auto &element : day_map)
		{
			day[element.first] = time;
		}

		try
		{
			const fs::path p(dbPath);
			if (!fs::exists(p))
			{
				std::ofstream writeFile(dbPath);
				writeFile << std::setw(4) << day << std::endl;
				writeFile.close();
				
				cout << "Client agenda was created" << endl;
			}
			else
			{
				cout << "Client agenda already exits" << endl;
			}
		}
		catch (fstream::failure& e)
		{
			cout << "Exception: initialize_db method throws -> " << e.what() << endl;
		}
}

json db_helper::db_to_json(const string& dbPath)
{
	try
	{
		// example db client_path: local_storage/client_json_db/db.json 
		const fs::path p(dbPath);
		if (fs::exists(p))
		{
			std::ifstream readFile(dbPath);
			json db;
			readFile >> db;
			readFile.close();
			return db;

			// example below below (tested)
			// std::ifstream readFile("local_storage/client_json_db/storage.json");
			// json db;
			// readFile >> db;
			// readFile.close();
		}
		else
		{
			// if the db.json file was not created send empty json object back
			return json({});
		}
	}
	catch (fstream::failure& e)
	{
		cout << "Exception: read_db_json method throws -> " << e.what() << endl;
		return json({});
	}
}

bool db_helper::save_db(const string& dbPath, const json& db)
{
	try
	{
		fs::path p(dbPath);
		if (fs::exists(p))
		{
			std::ofstream writeFile(dbPath);
			writeFile << std::setw(4) << db << std::endl;
			writeFile.close();

			return true;
		}
		else
		{
			return false;
		}
	}
	catch (std::ofstream::failure& e)
	{
		cout << "Exception: update_meeting method throws -> " << e.what() << endl;
		return false;
	}
}

json db_helper::getMeetingByID(const string& meetingID, const json& db)
{
	json foundMeeting = json({});
	bool foundit = false;
	// looping through days
	for (auto day : db)
	{
		if (foundit) { break; }
		// looping through time
		for (auto time : day)
		{
			if (foundit) { break; }
			// looping through rooms
			for (auto room : time)
			{
				// if (foundit) { break; }
				if (!meetingID.compare(room.at("meetingID")))
				{
					foundMeeting = (room);
					foundit = true;
					break;
					// cout << foundMeeting<< endl;
				}
			}
		}
	}
	return foundMeeting;

	// //add funciton to look in agenda
	// json day;
	// json time;
	// map<string, string> day_map = time_day_room::day_map();
	// map<string, string> time_map = time_day_room::time_map(time_day_room::startTime, time_day_room::endTime);
	// for (const auto& element : day_map)
	// {
	// 	for (const auto& element1 : time_map)
	// 	{
	// 		if (!db.at(element.first).at(element1.first).empty()) {
	// 			if (!messageType.invite.compare(db.at(element.first).at(element1.first).at("message"))) {
	// 				return db.at(element.first).at(element1.first);
	// 			}
	// 		}
	// 	}
	// }
	// json emptyJ;
	// return emptyJ;
}
