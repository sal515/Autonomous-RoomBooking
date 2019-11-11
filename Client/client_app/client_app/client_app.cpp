// client_app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Generate proto files from the proto file definition:
// ..\..\..\protoc - 3.10.1 - win32\bin\protoc.exe - I = ..\..\..\Proto\ --cpp_out =. ..\..\..\Proto\messages.proto


// File management: https://www.tutorialspoint.com/cplusplus/cpp_files_streams.htm

#include "pch.h"
#include <iostream>

#include <cstdio>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Linking Winsock Library
#include <ws2tcpip.h> // Used to convert IPv4 or IPv6 addressed to standard binary and vice versa

#include <string>
#include <algorithm>
#include "messages.pb.h"

#include "json.hpp"

using json = nlohmann::json;

#define SERVER "192.168.0.115"  //IP address of RBMS UDP server
// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
#define BUFLEN 32768		//Max length of buffer including 

#define PORT 8888   //The port on which to listen for incoming data

int main(void)
{
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;




	json json_var;
	json_var["i"] = 0;
	json_var["j"] = 10;
	json_var["l"] = (int(json_var.at("i"))+ int(json_var.at("j")))/2;

	std::string json_string = json_var.dump();


	std::cout << json_var.at("l") << std::endl;
	std::cout << json_string << std::endl;



	int pause = 0;
	std::cin >> pause;

	return 0;





	WSADATA win_socket_struct;
	SOCKET s;

	struct sockaddr_in client_struct;
	int client_struct_len = sizeof(client_struct);

	char* msg = new char[BUFLEN];
	char buf[BUFLEN];
	string buffer;

	//TODO : what would be the limit and based on the limit the msg send has to be restricted
	// char msg[BUFLEN];
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
		cout << "Enter message : ";
		cin >> message;

		messages::request request;
		request.set_topic(message);

		cout << message << endl;

		const int sizeOfRequest = request.ByteSize();
		cout << sizeOfRequest << endl;

		memset(msg, '\0', BUFLEN + 1);

		request.SerializeToArray(msg, sizeOfRequest);

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


		// request


		// buffer = string(buf);
		// cout << "Sent Data: " << buffer << endl;
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
