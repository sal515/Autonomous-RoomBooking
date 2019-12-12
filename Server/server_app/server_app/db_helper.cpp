#include "pch.h"
#include "db_helper.h"


bool db_helper::createDirectory(string relativeDirName)
{
	try
	{
		const fs::path p(relativeDirName);

		if (fs::create_directory(p))
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
		const fs::path p(relativeDirName);

		if (fs::exists(p))
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
		const fs::path p(relativeDirName);

		if (db_helper::isDirectoryExist(relativeDirName))
		{
			// fs::remove(p);
			fs::remove_all(p);
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

void db_helper::initialize_db(const string& pending_db, const string& confirmed_db)
{
		// ============ Server ===============
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
			const fs::path p(pending_db);
			if (!fs::exists(p))
			{
				std::ofstream writeFile(pending_db);
				writeFile << std::setw(4) << day << std::endl;
				writeFile.close();

				cout << "Server pending database was created" << endl;
			}
			else
			{
				cout << "Server pending database already exits" << endl;
			}
		}
		catch (fstream::failure& e)
		{
			cout << "Exception: \"pending_db\" initialize_db method throws -> " << e.what() << endl;
		}
	
	try
		{
			const fs::path p(confirmed_db);
			if (!fs::exists(p))
			{
				std::ofstream writeFile(confirmed_db);
				writeFile << std::setw(4) << day << std::endl;
				writeFile.close();

				cout << "Server confirmed database was created" << endl;
			}
			else
			{
				cout << "Server confirmed database already exits" << endl;
			}
		}
		catch (fstream::failure& e)
		{
			cout << "Exception: \"confirmed_db\" initialize_db method throws -> " << e.what() << endl;
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

json db_helper::db_to_jsonArr(const string& dbPath)
{
	try
	{
		const fs::path p(dbPath);
		if (fs::exists(p))
		{
			std::ifstream readFile(dbPath);
			json db = json::array();
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
		cout << "Exception: read_db_jsonArr method throws -> " << e.what() << endl;
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

json db_helper::getMeetingByID(const json& db, const string meetingID)
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
}
