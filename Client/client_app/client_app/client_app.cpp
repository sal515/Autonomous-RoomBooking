// client_app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// File management: https://www.tutorialspoint.com/cplusplus/cpp_files_streams.htm

#include "pch.h"

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

// Example Function prototypes
int test_pause_exit();
vector<string> list_of_participants(int min);
void menu();
bool getMeetID(const string& meetID);
bool check_ip(const string &ip);
bool check_schedule(json schedule);
bool extract_date(const std::string& s, int& d, int& m, int& y);


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
	WSACleanup(); 


	return test_pause_exit();
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


void menu() 
{ 
	int reqCounter = 1;
	char choice;
	cout << "Please select from the following options:\n"
		 << "b. Book a Room\n"
		 << "c. Cancel a Meeting\n"
		 << "w. Withdraw from a Meeting\n"
		 << "r. Request to Participate\n"
		 << "e. Exit the Program\n";
	bool goodInput = false;
	while (1) {
		while (!goodInput) {
			cin >> choice;

			switch (choice)
			{
				case 'b':
				{
					string date;
					int mm, dd, yyyy, hh, min_participants;
					string topic;
					cout << "Please provide the following details:\n"
						<< "Date (DD/MM/YYYY): ";
					cin >> date;
					while (!extract_date(date, dd, mm, yyyy))
					{
						cout << "\nPlease input a valid date (DD/MM/YYYY): ";
						cin >> date;
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

					//Copy into json to send to server + implement request id with incremental?
					//variables are date, time, topic, min_participants and participants(list)
					goodInput = true;
					string requestID = std::to_string(reqCounter);
					string timeH = std::to_string(hh);
					string minParts = std::to_string(min_participants);
					messages message;
					json reqz = message.request(requestID, date, timeH, minParts, participants, topic);

					//put in json obj.
					break;
				}
				case 'c':
				{
					string meet_ID;
					while (!getMeetID(meet_ID)) {
						cout << "\nPlease enter a meeting ID: ";
						cin >> meet_ID;
					}
					//check if meeting in agenda.
					//check if meeting creator is same IP.
					//if it is, send cancellation.
					goodInput = true;
					messages message;
					json cancel = message.cancelMeet(meet_ID);
					break;
				}
				case 'w':
				{
					string meet_ID;
					while (!getMeetID(meet_ID)) {
						cout << "\nPlease enter a meeting ID: ";
						cin >> meet_ID;
					}
					//check if meeting in agenda
					// request withdrawal if in local agenda.
					//store in json
					goodInput = true;
					break;
				}
				case 'e':
				{
					cout << "exiting program";
					return;

					cout << "Please enter a valid input";
				}
			}
		}
		goodInput = false;
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
	return participants;

}

bool getMeetID(const string& meetID) {
	//add funciton to look in agenda
	return true;
}


bool check_ip(const string &ip)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
	return result != 0;
}

bool check_schedule(json schedule)
{
	//look into json file to see if available or not
	return false;
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
