#include "pch.h"
#include <string>
#include "json.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;
using std::vector;

// ================ db_helper functions ================ 

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
	// 		cout << "ERROR: Couldn't retrieve the executable file client_path" << endl;
	// 	}
	// }
	// catch (...)
	// {
	// 	cout << "ERROR: Couldn't retrieve the executable file client_path" << endl;
	// }


	const fs::path pathToShow{fs::current_path()};

	int i = 0;
	cout << "Displaying client_path info for: " << pathToShow << "\n";
	for (const auto& part : pathToShow)
	{
		cout << "client_path part: " << i++ << " = " << part << "\n";
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


	cout << "client_path concat/append:\n";
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


void example_db_creator(string example_db_path)
{
	//Server only
	// json room_list;

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

	json meeting =
	{
		{"minimumParticipants", "min Participants"},
		{"rq", "request number"},
		{"mt", "meeting number"},
		{"invitedParticipantsVec", invitedParticipantsVec},
		{"confirmedParticipantsVec", confirmedParticipantsVec},
		{"topic", "Some topic for the meeting I guess"},
		{"bookingDate", "some Date"},
		{"requester", "requester IP"}
	};


	time["6"] = meeting;
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
	// room_list["EV005.251"] = all_days;
	// room_list["EV02.301"] = all_days;


	// JSON Array to Vector Example code: 
	// =====================================
	// string testEvent = event.dump();
	// cout << test << endl;
	// json str2Json = json::parse(test);
	// vector<string> jsonArr2Vector = (str2Json.at("invitedParticipantsVec"));
	// cout << jsonArr2Vector.at(0) << endl;
	// return  test_pause_exit();
}

// ================ db_helper functions ================ 
