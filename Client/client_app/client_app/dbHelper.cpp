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


	const fs::path pathToShow{fs::current_path()};

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


string dbHelper::read_db(string dbPath)
{
	try
	{
		fs::path p(dbPath);
		if (fs::exists(p))
		{
			string data;
			fstream file;
			file.open(dbPath, std::ios::out | std::ios::in);
			file >> data;
			file.close();
			return data;

		}
		else
		{
			fstream file;
			file.open(dbPath, std::ios::out | std::ios::in | std::ios::trunc);
			file.close();
			return string();
		}
	}
	catch (fstream::failure e)
	{
		cout << "Exception: read_db method throws -> " << e.what() << endl;
		return string();
	}
}


bool dbHelper::write_db(string dbPath, string data)
{
	try
	{
		fs::path p(dbPath);
		if (fs::exists(p))
		{
			// string data;
			fstream file;
			file.open(dbPath, std::ios::out | std::ios::in);
			file << data;
			file.close();
			return true;
		}
		else
		{
			// string data;
			fstream file;
			file.open(dbPath, std::ios::out | std::ios::in | std::ios::trunc);
			file << data;
			file.close();
			return true;
		}
	}
	catch (fstream::failure e)
	{
		cout << "Exception: read_db method throws -> " << e.what() << endl;
		return false;
	}
}
