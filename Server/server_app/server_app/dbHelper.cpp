#include "pch.h"
#include "dbHelper.h"


void test_path()
{
	// TCHAR executable_path_tchar[MAX_PATH] = { 0 };
	// string executable_path_str;
	//
	// try
	// {
	// 	if(GetModuleFileName(NULL, executable_path_tchar, MAX_PATH) || GetLastError() != 0 )
	// 	{
	// 		std::wstring executable_path_wstring(&executable_path_tchar[0]);
	// 		executable_path_str = string(executable_path_wstring.begin(), executable_path_wstring.end());
	// 		// cout << executable_path_str << endl;
	// 	}
	// 	else
	// 	{
	// 		cout << "ERROR: Couldn't retrieve the executable file path" << endl;
	// 	}
	// }
	// catch (...)
	// {
	// 	cout << "ERROR: Couldn't retrieve the executable file path" << endl;
	// }


	const fs::path pathToShow{ fs::current_path() };

	int i = 0;
	cout << "Displaying path info for: " << pathToShow << "\n";
	for (const auto& part : pathToShow)
	{
		cout << "path part: " << i++ << " = " << part << "\n";
	}

	cout << "exists() = " << fs::exists(pathToShow) << "\n"
		<< "root_name() = " << pathToShow.root_name() << "\n"
		<< "root_path() = " << pathToShow.root_path() << "\n"
		<< "relative_path() = " << pathToShow.relative_path() << "\n"
		<< "parent_path() = " << pathToShow.parent_path() << "\n"
		<< "filename() = " << pathToShow.filename() << "\n"
		<< "stem() = " << pathToShow.stem() << "\n"
		<< "extension() = " << pathToShow.extension() << "\n";


	// bool dirCreated = fs::create_directory("test");
	// cout << dirCreated << endl;

	try
	{
		cout << "canonical() = " << fs::canonical(pathToShow) << "\n";
	}
	catch (fs::filesystem_error err)
	{
		cout << "exception: " << err.what() << "\n";
	}


	cout << "path concat/append:\n";
	fs::path p1("C:\\temp");
	p1 /= "user";
	p1 /= "data";
	cout << p1 << "\n";

	fs::path p2("C:\\temp\\");
	p2 += "user";
	p2 += "data";
	cout << p2 << "\n";


	// string testDirName = "\\test";
	// string testDirPath = executable_path_str + testDirName;
	// cout << testDirPath << endl;


	// // create a new directory if it doesn't exit
	// if (CreateDirectory(executable_path_tchar, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	// {
	// 	// new directory created		
	// 	cout << "New directory created" << endl;
	// }
	// else
	// {
	// 	// new directory not created
	// 	cout << "New directory could not be created" << endl;
	// }

	//
	// fstream file;
	// file.open("test\test_file.txt", std::ios::out | std::ios::in | std::ios::trunc);
	// // file.open(".\test_file.txt");
	// file << json_string << endl;
	//
	// file.close();
}

int test_json()
{
	json json_var;
	json_var["i"] = 0;
	json_var["j"] = 10;
	json_var["l"] = (int(json_var.at("i")) + int(json_var.at("j"))) / 2;

	string json_string = json_var.dump();


	cout << json_var.at("l") << endl;
	cout << json_string << endl;


	test_path();

	int pause = 0;
	cin >> pause;

	return 0;
}


void dbHelper::example_db_creator(string example_db_path)
{
	//Server only
	// json room;

	json all_days;
	json time;
	json request;
	json userId;


	json userA = {
		{"ip", "168.204.654.152"},
		{"listeningPort", "8000"},
		{"userName", "UserA"}
	};

	json userB = {
		{"ip", "162.344.367.132"},
		{"listeningPort", "5000"},
		{"userName", "UserB"}
	};

	json userC = {
		{"ip", "122.111.333.555"},
		{"listeningPort", "2222"},
		{"userName", "UserC"}
	};

	std::vector<json> invitedParticipantsVec;
	vector<json> confirmedParticipantsVec;

	invitedParticipantsVec.push_back(userA);
	invitedParticipantsVec.push_back(userB);
	invitedParticipantsVec.push_back(userC);

	confirmedParticipantsVec.push_back(userA);
	confirmedParticipantsVec.push_back(userB);

	json event =
	{
		{"minimumParticipants", "min Participants"},
		{"rq", "request number"},
		{"mt", "meeting number"},
		{"invitedParticipantsVec", invitedParticipantsVec},
		{"confirmedParticipantsVec", confirmedParticipantsVec},
		{"topic", "Some topic for the event I guess"},
		{"bookingDate", "some Date"},
		{"requester", "requester IP"}
	};


	time["6"] = event;
	time["7"] = json({});
	time["8"] = json({});
	time["9"] = json({});
	time["10"] = json({});
	time["11"] = json({});
	time["12"] = json({});
	time["13"] = json({});
	time["14"] = json({});
	time["15"] = json({});
	time["16"] = json({});
	time["17"] = json({});
	time["18"] = json({});
	time["19"] = json({});
	time["20"] = json({});

	all_days["monday"] = time;
	all_days["tuesday"] = time;
	all_days["wednesday"] = time;
	all_days["thursday"] = time;
	all_days["friday"] = time;

	std::ofstream writeFile(example_db_path);
	writeFile << std::setw(4) << all_days << std::endl;
	writeFile.close();

	// server only
	// room["EV005.251"] = all_days;
	// room["EV02.301"] = all_days;


	// JSON Array to Vector Example code: 
	// =====================================
	// string testEvent = event.dump();
	// cout << testEvent << endl;
	// json str2Json = json::parse(testEvent);
	// vector<string> jsonArr2Vector = (str2Json.at("invitedParticipantsVec"));
	// cout << jsonArr2Vector.at(0) << endl;
	// return  test_pause_exit();



}


dbHelper::dbHelper()
{
}


dbHelper::~dbHelper()
{
}


bool dbHelper::createDirectory(string relativeDirName)
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
			if (dbHelper::isDirectoryExist(relativeDirName))
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

bool dbHelper::isDirectoryExist(string relativeDirName)
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


string dbHelper::json_to_string(json ajson)
{
	return ajson.dump();
}


bool dbHelper::removeDirectory(string relativeDirName)
{
	try
	{
		if (dbHelper::isDirectoryExist(relativeDirName))
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


json dbHelper::db_to_json(const string& dbPath)
{
	// usage example
	// json db = dbHelper::db_to_json(dbPath);

	try
	{
		// example db path: local_storage/client_json_db/db.json 
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


bool dbHelper::update_event(json& db, const string& day, const string& time, const json& event)
{
	try
	{
		db.at(day).at(time) = event;
		return true;
	}
	catch (nlohmann::json::exception& e)
	{
		cout << "Exception: update_event method throws -> " << e.what() << endl;
		return false;
	}
}


bool dbHelper::update_db(const string& dbPath, const json& db)
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
		cout << "Exception: update_event method throws -> " << e.what() << endl;
		return false;
	}
}
