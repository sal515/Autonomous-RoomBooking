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
#include <ctime>
#include <sstream>
#include <iomanip>
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
int test_pause_exit();
void menu();
bool check_schedule(struct tm *time);
bool extract_date(const std::string& s, int& d, int& m, int& y);


int main(void)
{
	/*dbHelper::createDirectory(dir_local_storage);

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
	client_struct.sin_port = htons(PORT);


	//start communication
	while (true)
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
	WSACleanup(); */

	cout << "This is supposed to output";

	return test_pause_exit();
}


void menu() 
{ 
	char choice;
	cout << "Please select from the following options:\n"
		 << "b. Book a Room\n"
		 << "c. Cancel a Meeting\n"
		 << "w. Withdraw from a Meeting\n"
		 << "r. Request to Participate\n"
		 << "e. Exit the Program\n";

	cin >> choice;

	switch (choice)
	{
	case 'b':
		string date_time;
		int mm, dd, yyyy, hh, min_participants;
		string topic;
		cout << "Please provide the following details:\n"
			<< "Date (DD/MM/YYYY): ";
		cin >> date_time;
		while (!extract_date(date_time, dd, mm, yyyy))
		{
			cout << "\nPlease input a valid date (DD/MM/YYYY): ";
			cin >> date_time;
		}
		cout << "\nTime of meeting (24hr): ";
		cin >> hh;
		while (hh > 24 || hh < 0)
		{
			cout << "\nPlease input a valid time 0-24: ";
			cin >> hh;
		}
		cout << "\nTopic of meeting: ";
		cin >> topic;
		cout << "\nMinimum number of Participants: ";
		cin >> min_participants;
		vector<string> participants = list_of_participants(min_participants);

		//Elie copy into json to send to server + implement request id?
		break;
	

	}



}

vector<string> list_of_participants(int min) {
	vector<string> participants;
	vector<string>::iterator it;
	string participants_menu = " a. add new participant\n v. view participants\n f. finish\n";
	string ip;
	char choice = 'm';
	while (choice != 'e') 
	{
		cin >> choice;
		switch (choice)
		{
		case 'm': //show menu
			cout << participants_menu;
			cin >> choice;
			break;
		case 'a': //add participants
			
			cout << "IP: ";
			cin >> ip;
			if (participants.empty())
			{
				if (check_ip(ip))
				{
					participants.push_back(ip);
				}
				else
				{
					cout << "IP address is not valid.\n";
					choice = 'm';
				}
			}
			else
			{
				it = find(participants.begin(), participants.end(), ip);
				if (it != participants.end())
				{
					cout << "Participant is already part of the list\n";
				}
				else
				{
					if (check_ip(ip))
					{
						participants.push_back(ip);
					}
					else
					{
						cout << "IP address is not valid.\n";
						choice = 'm';
					}
				}

			} 
			choice = 'm';
			break;
		case 'v': //output list of participants
			for (int i = 0; i < participants.size(); i++) 
			{
				cout << participants.at(i);
			}
			choice = 'm';
			break;
		case 'f': //check if list is complete
			if (min < participants.size()) 
			{
				choice = 'e';
			}
			else 
			{
				cout << "The total number of participants is smaller than the minimum required.\nYou need to add "
					<< min - participants.size() << " more participants.";
				choice = 'm';
			}
		default :
			cout << "Please input a valid choice.\n";
			choice = 'm';	
		}

	}

}



bool check_ip(const string &ip)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
	return result != 0;
}

// function expects the string in format dd/mm/yyyy:
bool extract_date(const std::string& s, int& d, int& m, int& y) 
{
	std::istringstream is(s);
	char delimiter;
	if (is >> d >> delimiter >> m >> delimiter >> y) 
	{
		struct tm t = { 0 };
		t.tm_mday = d;
		t.tm_mon = m - 1;
		t.tm_year = y - 1900;
		t.tm_isdst = -1;

		// normalize:
		time_t when = mktime(&t);
		const struct tm *norm = localtime(&when);
		// the actual date would be:
		// m = norm->tm_mon + 1;
		// d = norm->tm_mday;
		// y = norm->tm_year;
		// e.g. 29/02/2013 would become 01/03/2013

		// validate (is the normalized date still the same?):
		return (norm->tm_mday == d &&
			norm->tm_mon == m - 1 &&
			norm->tm_year == y - 1900);
	}
	return false;
}

// ==================  Examples  ======================================
int test_pause_exit()
{
	int pause = 0;
	cin >> pause;
	return 0;
}
