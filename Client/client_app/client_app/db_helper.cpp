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
		exit(0);
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

		exit(0);
		return false;
	}
}

void db_helper::initialize_db(const string& dbPath)
{
	// json rooms;
	json day;
	json time;

	map<string, string> time_map = time_day_room::time_map(time_day_room::startTime, time_day_room::endTime);
	for (const auto& element : time_map)
	{
		time[element.first] = json({});
	}

	map<string, string> day_map = time_day_room::day_map();
	for (const auto& element : day_map)
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

void db_helper::initialize_invitations_db(const string& dbPath)
{
	// const json invitationsArray = json::array({{"meetingID", "meetID"}}) ;
	const json invitationsArray = json::array();

	try
	{
		const fs::path p(dbPath);
		if (!fs::exists(p))
		{
			std::ofstream writeFile(dbPath);
			writeFile << std::setw(4) << invitationsArray << std::endl;
			writeFile.close();

			cout << "Invitations agenda was created" << endl;
		}
		else
		{
			cout << "Invitations agenda already exits" << endl;
		}
	}
	catch (fstream::failure& e)
	{
		cout << "Exception: initialize_invitations_db method throws -> " << e.what() << endl;
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
			// readFile.close();
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
	if (!dbPath.compare(config.INVITATIONS_PATH) || 
		!dbPath.compare(config.SENT_RECEIVED_LOG_PATH))
	{
		json arr = json::array();
		arr = db;

		try
		{
			fs::path p(dbPath);
			if (fs::exists(p))
			{
				std::ofstream writeFile(dbPath);
				writeFile << std::setw(4) << arr << std::endl;
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
			cout << "Exception: save invitation database method throws -> " << e.what() << endl;

			return false;
		}
	}


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
		cout << "Exception: save meeting database method throws -> " << e.what() << endl;
		return false;
	}
}

json db_helper::getMeetingByID(const string& meetingID, const json& db)
{
	json foundMeeting = json({});
	// looping through days
	for (const auto& day : db)
	{
		// looping through time
		for (const auto& time : day)
		{
			if (!meetingID.compare(time.at("meetingID")))
			{
				foundMeeting = (time);
				return foundMeeting;
			}
		}
	}
	return foundMeeting;
}

void db_helper::print_invitations(vector<json>& invitationDB)
{
	if (invitationDB.empty())
	{
		cout << "No invitations at this time" << endl;
		return;
	}

	int counter = 0;
	for (json i : invitationDB)
	{
		cout << "Invitation # " << ++counter << endl;
		cout << "Meeting ID" << i.at("meetingID") << endl;
		cout << "Meeting Day" << i.at("meetingDay") << endl;
		cout << "Meeting Time" << i.at("meetingTime") << endl;
		cout << "Meeting Topic" << i.at("topic") << endl;
		cout << "Requester IP" << i.at("requesterIP") << endl;
		cout << endl;
	}
}

bool db_helper::find_invitation(string meetingID, meeting& meetObj, vector<json>& invitationDB)
{
	if (invitationDB.empty())
	{
		cout << "Invitation database was empty" << endl;
		return false;
	}

	for (json i : invitationDB)
	{
		if (!meetingID.compare(i.at("meetingID")))
		{
			meetObj = meeting::json_to_meetingObj(i);
			return true;
		}
	}
	return false;
}

void db_helper::remove_invitation(string meetingID, vector<json>& invitationDB)
{
	if (invitationDB.empty())
	{
		cout << "Invitation database was empty" << endl;
		return;
	}

	int counter = -1;

	for (json i : invitationDB)
	{
		counter += 1;
		if (!meetingID.compare(i.at("meetingID")))
		{
			invitationDB.erase(invitationDB.begin() + (counter));
			break;
		}
	}
}

void db_helper::add_invitation(meeting meetingObj, vector<json>& invitationDB)
{
	invitationDB.push_back(meeting::meetingObj_to_json(meetingObj));
}
