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

using std::thread;
using std::mutex;
using std::queue;
using std::ref;
using std::atomic;

// Example Function prototypes
int test_pause_exit();
void threadTester(int number);


// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
#define BUFLEN 32768		//Max length of buffer 
#define SERVER_PORT 8888   //The port on which to listen for incoming data
#define CLIENT_PORT 9999   //The port on the clients listen to, for the server to send the data

#define SERVER "192.168.0.115"
// #define CLIENT "192.168.0.106"
#define CLIENT "192.168.0.115"

// #define CLIENT_IP "192.168.0.106"   //Clinet IP
#define CLIENT_IP "192.168.0.115"


// void create_n_bind_server_socket(
// 	WSADATA win_socket_struct,
// 	SOCKET& s,
// 	sockaddr_in&
// 	server_struct);
//
// void send_message_to_client(
// 	SOCKET s,
// 	sockaddr_in server_struct,
// 	int server_struct_len,
// 	const string& messageJsonStr);
//
// void receive_message_from_client(
// 	SOCKET s,
// 	sockaddr_in server_struct,
// 	int server_struct_len,
// 	queue<json>& messages,
// 	string& clientIP
// );


// Mutexes 
// mutex received_message_queue_mutex;
// mutex send_message_mutex;
// mutex cout_mutex;
// mutex cin_mutex;

// Global variables in use
queue<json> messages_queue; // queues for messages from the clients
json confirmed_db;
json pending_db;
std::atomic<int> global_meet_id(0);
std::atomic<bool> exit_program(false);
string clientIP;


struct sockaddr_in client_struct;
int client_struct_len = sizeof(client_struct);


std::mutex socket_mutex;

bool gotSomething = false;


void send_to_client(SOCKET s, sockaddr_in clientAddrStr, char buf[32768])
{
	char buff[BUFLEN];
	string messageJsonStr = "Hello from desk Server";
	memset(buff, '\0', BUFLEN);
	messageJsonStr.copy(buff, messageJsonStr.size());

	while (true)
	{
		// if (socket_mutex.try_lock() && gotSomething)
		if (socket_mutex.try_lock())
		{
			//send the messageJsonStr
			if (sendto(s, buff, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&clientAddrStr),
			           sizeof(clientAddrStr)) == SOCKET_ERROR)
			{
				cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
				exit(EXIT_FAILURE);
			}
			socket_mutex.unlock();
		}
		// sleep for 5000 ms
		// sleepcp(5000);
	}
}

int main(void)
{
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.PENDING_DB_PATH, config.CONFIRMED_DB_PATH);

	// loading db from file to memory
	confirmed_db = db_helper::db_to_json(config.CONFIRMED_DB_PATH);
	pending_db = db_helper::db_to_json(config.PENDING_DB_PATH);


	WSADATA wsaData = {};
	SOCKET s = INVALID_SOCKET;

	sockaddr_in serverAddrStr, clientAddrStr;
	int serverAddrStr_len = sizeof(serverAddrStr);


	//Initialise winsock
	cout << "\nInitialising Winsock... " << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Initialised Winsock" << endl;


	int iType = SOCK_DGRAM;
	// int iFamily = AF_INET;
	// int iProtocol = IPPROTO_UDP;
	serverAddrStr.sin_family = AF_INET;
	serverAddrStr.sin_port = htons(SERVER_PORT);
	serverAddrStr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	//create socket
	if ((s = socket(serverAddrStr.sin_family, iType, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}

	//Bind Socket
	if (bind(s, (struct sockaddr *)&serverAddrStr, sizeof(serverAddrStr)) == SOCKET_ERROR)
	{
		cout << "Bind failed with error code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}

	char buf[BUFLEN];

	clientAddrStr.sin_family = AF_INET;
	clientAddrStr.sin_port = htons(CLIENT_PORT);
	// setup the variable: client_struct.sin_addr.S_un
	if (inet_pton(clientAddrStr.sin_family, CLIENT, &clientAddrStr.sin_addr.S_un.S_addr) != 1)
	{
		cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	};

	thread sendingThread(send_to_client, ref(s), ref(clientAddrStr), ref(buf));
	// send_to_client(s, clientAddrStr, buf);


	// cout_mutex.try_lock();
	cout << "Waiting for data..." << endl;
	fflush(stdout);

	//clear the buffer by filling null, it might have previously received data
	// memset(buf, '\0', BUFLEN + 1);
	memset(buf, '\0', BUFLEN);

	while (true)
	{
		gotSomething = false;

		if ((recvfrom(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&serverAddrStr),
		              &serverAddrStr_len)) ==
			SOCKET_ERROR)
		{
			cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		char printable_IP_add_buf[INET_ADDRSTRLEN];
		if ((inet_ntop(AF_INET, &serverAddrStr.sin_addr, printable_IP_add_buf, INET_ADDRSTRLEN)) != nullptr)
		{
			clientIP = string(printable_IP_add_buf);
		}

		cout << "clientIP: " << clientIP << endl;

		string buffer = std::string(buf);
		cout << "received data" << endl;
		cout << buffer << endl;
		cout << endl;

		gotSomething = true;
	}


	sendingThread.join();


	closesocket(s);
	WSACleanup();

	test_pause_exit();

	return 0;
}

// ==================  Examples  ======================================
int test_pause_exit()
{
	int pause = 0;
	cin >> pause;
	return 0;
}

void threadTester(int number)
{
	for (int i = 0; i < number; i++)
	{
		cout << i << endl;
	}
}
