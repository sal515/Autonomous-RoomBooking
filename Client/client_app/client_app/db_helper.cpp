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

void db_helper::initialize_db(const string& dbPath, const bool& isClient)
{
	if (isClient)
	{
		//Server only
		// json room_list;

		json day;
		json time;


		map<string, string> time_map = time_day_room::time_map(6, 22);
		for (const auto &element : time_map)
		{
			time[element.first] = json({});
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
	else
	{
		//server db initialize
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
