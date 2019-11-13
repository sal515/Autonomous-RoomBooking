// client_app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// File management: https://www.tutorialspoint.com/cplusplus/cpp_files_streams.htm

#include "pch.h"
#include <iostream>

// socket library
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Linking Winsock Library
#include <ws2tcpip.h> // Used to convert IPv4 or IPv6 addressed to standard binary and vice versa

// Other libraries
#include <string>
#include "json.hpp"
#include <vector>

#include "dbHelper.h"

#include <windows.h>
// #include <fileapi.h>


// namespaces
using json = nlohmann::json;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::fstream;
using std::vector;



#define SERVER "192.168.0.115"  //IP address of RBMS UDP server
// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
#define BUFLEN 32768		//Max length of buffer including 
#define PORT 8888   //The port on which to listen for incoming data

// Path for directories and files
const auto dir_local_storage = "local_storage";
const auto log_path = "local_storage/log.json";
const auto db_path = "local_storage/db.json";
const auto example_db_path = "local_storage/example_db.json";


// Example Function prototypes
void example_create_remove_directories();
int test_pause_exit();


// Function prototypes
void createClientDirectories();
void removeClientDirectories();


int main(void)
{

	dbHelper::createDirectory(dir_local_storage);

	json db = dbHelper::db_to_json(db_path);
	
	// dbHelper::update_event(db, "friday", "10", json({}));
	// if(dbHelper::update_db(db_path, db)) { cout << "db updated" << endl; }

	// return test_pause_exit();


	WSADATA win_socket_struct;
	SOCKET s;

	struct sockaddr_in client_struct;
	int client_struct_len = sizeof(client_struct);

	// char* msg = new char[BUFLEN];
	char msg[BUFLEN];
	char buf[BUFLEN];
	string buffer;
	string message;

	//Initialise winsock
	cout << "\nInitialising Winsock... " << endl;
	if (WSAStartup(MAKEWORD(2, 2), &win_socket_struct) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Initialised Winsock" << endl;

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		cout << "Could not create socket : " << WSAGetLastError << endl;
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&client_struct, 0, sizeof(client_struct));
	client_struct.sin_family = AF_INET;
	if (inet_pton(AF_INET, SERVER, &client_struct.sin_addr.S_un.S_addr) != 1)
	{
		cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError << endl;
		exit(EXIT_FAILURE);
	};
	client_struct.sin_port = htons(PORT);


	//start communication
	while (1)
	{
		// cout << "Enter message : ";
		// cin >> message;

		json jsonMsg;
		jsonMsg["message"] = "msg";
		message = jsonMsg.dump();

		cout << message << endl;

		memset(msg, '\0', BUFLEN + 1);
		message.copy(msg, message.size());

		//send the message
		if (sendto(s, msg, strlen(msg), 0, (struct sockaddr *)&client_struct, client_struct_len) == SOCKET_ERROR)
		{
			cout << "sendto() failed with error code : " << WSAGetLastError << endl;
			exit(EXIT_FAILURE);
		}

		// FIXME :: DOESN'T Work ?? clean up dynamic char* array[]
		// memset(msg, '\0', BUFLEN);
		// delete[] msg;

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		// memset(buf, '\0', BUFLEN);
		// //try to receive some data, this is a blocking call
		// if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&client_struct, &client_struct_len) == SOCKET_ERROR)
		// {
		// 	cout << "recvfrom() failed with error code : " << WSAGetLastError << endl;
		// 	exit(EXIT_FAILURE);
		// }


		// buffer = string(buf);
		// cout << "Sent Data: " << buffer << endl;
	}

	closesocket(s);
	WSACleanup();

	return 0;
}


// ==================  Examples  ======================================
void example_create_remove_directories()
{
	// Removing and Creating directory order matters
	// Cannot delete directories with existing subdirectories
	// Cannot create subdirectories if the outer directory does not exist
	dbHelper::createDirectory(dir_local_storage);
	// dbHelper::createDirectory(dir_log);
	// dbHelper::createDirectory(dir_json_db);
	// dbHelper::createDirectory("delete");
	// dbHelper::removeDirectory("delete");
	// dbHelper::removeDirectory("local_storage/log");
}

int test_pause_exit()
{
	int pause = 0;
	cin >> pause;
	return 0;
}
