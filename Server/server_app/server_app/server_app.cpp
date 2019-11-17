// server_app.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"

#include "process_messages.h"

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
#define BUFLEN 32768		//Max length of buffer 
#define PORT 8888   //The port on which to listen for incoming data

// struct messages message;

int main(void)
{
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.DB_PATH, false);

	// loading db from file to memory
	json db = db_helper::db_to_json(config.DB_PATH);


	json received_data;

	// Test: messageType.request
	received_data.clear();
	received_data["message"] = messageType.request;
	received_data["requestID"] = "2";
	received_data["meetingDay"] = "friday";
	received_data["meetingTime"] = "10";
	received_data["invitedParticipantsIP"] = vector<string> {"1.1.1.1", "2.2.2.2"};
	received_data["minimumParticipants"] = "1";
	received_data["topic"] = "Testing request message";

	// Process the request data 
	processMessages(db, received_data);


	return test_pause_exit();




	/*
	 * Socket Variables
	 */

	WSADATA win_socket_struct;
	SOCKET s;

	struct sockaddr_in server_struct;
	int server_struct_len = sizeof(server_struct);
	int recv_len;

	// struct sockaddr_in si_other;

	char msg[BUFLEN];
	string message;

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

		if ((recvfrom(s, buf, BUFLEN, 0, reinterpret_cast<struct sockaddr *>(&server_struct), &server_struct_len)) ==
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
		json received_data;
		received_data.update(json::parse(buffer));

		// Process the request data 
		processMessages(db, received_data);


		// std::cout << "Data Recieved from client IP - " << clientIP << ": " << buffer << std::endl;

		// //now reply the client with the same data
		if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*)&server_struct, server_struct_len) == SOCKET_ERROR)
		{
			std::cout << "sendto() failed with error code : " << WSAGetLastError << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}


// ==================  Examples  ======================================
int test_pause_exit()
{
	int pause = 0;
	cin >> pause;
	return 0;
}
