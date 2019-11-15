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

#include "meeting.h"
#include "db_helper.h"
#include "app_config.h"

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

// Example Function prototypes
int test_pause_exit();


// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
#define BUFLEN 32768		//Max length of buffer including 
#define LISTENING_PORT 8888   //The port on which to listen for incoming data

string SERVER_IP_IN;

// Function prototypes
string ask_for_ip();



int main(void)
{
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.DB_PATH, TRUE);

	// loading db from file to memory
	json db = db_helper::db_to_json(config.DB_PATH);



	// return Testing_dbHelper_meetingObj();



	SERVER_IP_IN = ask_for_ip();

	// Converting IP: string to char[]
	// The character array should be the size of the string+1 to accomodate the '\0' - null character at the end
	int const server_ip_in_size = SERVER_IP_IN.length() + 1;
	char* SERVER = new char[server_ip_in_size];
	memset(SERVER, '\0', server_ip_in_size);
	strcpy_s(SERVER, server_ip_in_size, SERVER_IP_IN.c_str());

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
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&client_struct, 0, sizeof(client_struct));
	client_struct.sin_family = AF_INET;
	if (inet_pton(AF_INET, SERVER, &client_struct.sin_addr.S_un.S_addr) != 1)
	{
		cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	};
	client_struct.sin_port = htons(LISTENING_PORT);

	// delete[] SERVER;

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
			cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
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


string ask_for_ip()
{
	string SERVER_IP_IN;
	cout << "Please input the server IP" << endl;
	cin >> SERVER_IP_IN;

	if (SERVER_IP_IN == "d")
	{
		SERVER_IP_IN = config.DEBUG_HARDCODED_SERVER_IP_IN;
	}
	return SERVER_IP_IN;
}


// ==================  Examples  ======================================
int test_pause_exit()
{
	int pause = 0;
	cin >> pause;
	return 0;
}
