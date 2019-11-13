// server_app.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include <iostream>

#include <cstdio>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Linking Winsock Library
#include <ws2tcpip.h>	// Used to convert IPv4 or IPv6 addressed to standard binary and vice versa

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


// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
#define BUFLEN 32768		//Max length of buffer 
#define PORT 8888   //The port on which to listen for incoming data


// Path for directories and files
const auto dir_local_storage = "local_storage";
const auto log_path = "local_storage/log.json";
const auto db_path = "local_storage/db.json";
const auto example_db_path = "local_storage/example_db.json";

int main(void)
{
	dbHelper::createDirectory(dir_local_storage);
	json db = dbHelper::db_to_json(db_path);


	/*
	 * Socket Variables
	 */

	WSADATA win_socket_struct;
	SOCKET s;

	struct sockaddr_in server_struct, si_other;

	int server_struct_len, recv_len;
	server_struct_len = sizeof(server_struct);


	char buf[BUFLEN];
	std::string buffer;


	// /*
	//  * Other variables (unrelated to sockets)
	//  */
	// std::string server_hostname;
	std::string clientIP;
	// std::string client_hostname;
	//
	// /*
	//  */

	//Initialise winsock
	std::cout << "\nInitialising Winsock... " << std::endl;

	if (WSAStartup(MAKEWORD(2, 2), &win_socket_struct) != 0)
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Initialised Winsock" << std::endl;


	// char c_hostname[128];
	// if ((gethostname(c_hostname, sizeof c_hostname) != 0))
	// {
	// 	std::cout << "Error while retrieving hostname.Error Code : " << WSAGetLastError() << std::endl;
	// }
	//
	// if (c_hostname != nullptr)
	// {
	// 	server_hostname = std::string(c_hostname);
	// 	std::cout << "Hostname of computer is: " << server_hostname << std::endl;
	// }

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Could not create socket : " << WSAGetLastError() << std::endl;
	}

	std::cout << "Socket created" << std::endl;

	//Prepare the sockaddr_in structure
	server_struct.sin_family = AF_INET;
	server_struct.sin_addr.s_addr = INADDR_ANY;
	server_struct.sin_port = htons(PORT);

	//Bind Socket
	if (bind(s, (struct sockaddr *)&server_struct, sizeof(server_struct)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Bind done" << std::endl;

	//keep listening for data
	while (1)
	{
		std::cout << "Waiting for data..." << std::endl;
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN + 1);

		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&server_struct, &server_struct_len)) ==
			SOCKET_ERROR)
		{
			std::cout << "recvfrom() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		char printable_IP_add_buf[INET_ADDRSTRLEN];
		if ((inet_ntop(AF_INET, &server_struct.sin_addr, printable_IP_add_buf, INET_ADDRSTRLEN)) != nullptr)
		{
			clientIP = std::string(printable_IP_add_buf);
		}

		cout << "clientIP: " << clientIP << endl;
		buffer = std::string(buf);
		json received_data = json::parse(buffer);
		cout << received_data.at("message") << std::endl;


		//
		// std::cout << "Data Recieved from client IP - " << clientIP << ": " << buffer << std::endl;

		// //now reply the client with the same data
		// if (sendto(s, buf, recv_len, 0, (struct sockaddr*)&client_struct, client_struct_len) == SOCKET_ERROR)
		// {
		// 	std::cout << "sendto() failed with error code : " << WSAGetLastError << std::endl;
		// 	exit(EXIT_FAILURE);
		// }
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
