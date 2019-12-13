// client_app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// File management: https://www.tutorialspoint.com/cplusplus/cpp_files_streams.htm

#include "pch.h"
#include "main_functions.h"

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

#define BUFLEN 32768		//Max length of buffer including 
#define SERVER_LISTENING_PORT 8888   //The port on which to listen for incoming data
#define OWN_LISTENING_PORT 9999   //The port on which to listen for incoming data

// #define SERVER_LISTENING_PORT 9999   //DEBUG: Test to send it to the client from the client

// Example Function prototypes
int test_pause_exit();

// Mutexes 
// std::mutex db_mutex;
std::mutex socket_mutex;


// Global variables in use
std::queue<json> received_messages_queue; // queue for messages received from server
std::queue<json> sending_messages_queue; // queue for messages to be sent from the clients

// debug variables
bool debugResendToClientAfterReceive = false;
// bool debugResendToClientAfterReceive = true;
bool debugTestData = false;
// bool debugTestData = true;
// bool resetDatabases = false;
bool resetDatabases = true;

string SERVER_IP_IN;

// string CLIENT_LOCAL_IP;
std::atomic<int> REQUEST_COUNTER(0);
json confirmed_db;
json pending_db;
std::atomic<bool> exit_program(false);


// Function prototypes
bool get_client_local_ip(SOCKET s, string& client_local_ip);

// Please do not call this function - Its already threaded
void send_to_server(SOCKET s, sockaddr_in serverAddrStr);
void processMsg(json& db, vector<json>& invitations_db, std::queue<json>& received_messages_queue,
                std::queue<json>& sending_messages_queue);

int main(void)
{
	// ============= Initialization of database ==========================

	if (resetDatabases)
	{
		cout << "Reset databases: (y/n)" << endl;
		char resetDB = 'y';
		// TODO : Uncomment resetDB
		// cin >> resetDB;
		resetDB = std::tolower(resetDB);
		if (resetDB == 'y')
		{
			db_helper::removeDirectory(config.DIR_LOCAL_STORAGE);
		}
	}


	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.DB_PATH);
	db_helper::initialize_invitations_db(config.INVITATIONS_PATH);
	logger::initialize_log_file(config.SENT_RECEIVED_LOG_PATH);


	// loading db from file to memory
	json db = db_helper::db_to_json(config.DB_PATH);
	vector<json> invitation_db = db_helper::db_to_jsonArr(config.INVITATIONS_PATH);
	// ============= Initialization of database ==========================


	// TODO: Delete test =====================

	// json jsonMsg;
	// jsonMsg["message"] = "REQUEST";
	// jsonMsg["meetingDay"] = "monday";
	// jsonMsg["meetingTime"] = "10";
	// jsonMsg["requestID"] = "1";
	// jsonMsg["topic"] = "yomama";
	// jsonMsg["participantsIP"] = json::array({});
	// jsonMsg["participantsIP"].push_back("192.168.1.133");
	// jsonMsg["participantsIP"].push_back("192.168.0.188");
	//
	// logger::add_received_log(config.SENT_RECEIVED_LOG_PATH, jsonMsg);
	//
	// test_pause_exit();
	//
	// return 0;

	// TODO: Delete test =====================


	// --------------- Test codes below  -------------------------
	if (debugTestData)
	{
		json jsonMsg;
		jsonMsg["message"] = "REQUEST";
		jsonMsg["meetingDay"] = "monday";
		jsonMsg["meetingTime"] = "10";
		jsonMsg["requestID"] = "1";
		jsonMsg["topic"] = "yomama";
		jsonMsg["invitedParticipantsIP"] = json::array({});
		jsonMsg["invitedParticipantsIP"].push_back("192.168.1.133");
		jsonMsg["invitedParticipantsIP"].push_back("192.168.0.188");
		jsonMsg["minimumParticipants"] = "1";
		// sending_messages_queue.push(jsonMsg);
		// sending_messages_queue.push(jsonMsg);
		//sending_messages_queue.push(jsonMsg);
		//sending_messages_queue.push(jsonMsg);
		queueHelper::push_to_queue(sending_messages_queue, jsonMsg);
	}
	// --------------- Test codes above -------------------------


	// ==================== Socket setup and binding =====================
	WSADATA wsaData = {};
	SOCKET s = INVALID_SOCKET;
	sockaddr_in serverAddrStr, ownAddrStr;


	//Initialise winsock
	cout << "\nInitialising Winsock... " << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
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


	//==================== Retrieve local ip for client ===========================
	// Get the local client ip to attach in the messages when sending request
	if (get_client_local_ip(s, CLIENT_IP))
	{
		cout << "Client local ip is " << CLIENT_IP << endl;
	}
	else
	{
		cout << "Client local ip was not found." << endl;
	};
	//==================== Retrieve local ip for client ===========================


	// get server IP from user 
	SERVER_IP_IN = ask_for_ip();

	//Initialise winsock
	cout << "\nInitializing Winsock... " << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
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

	// setup server address structure 
	serverAddrStr.sin_family = AF_INET;
	serverAddrStr.sin_port = htons(SERVER_LISTENING_PORT);
	if (inet_pton(serverAddrStr.sin_family, SERVER_IP_IN.c_str(), &serverAddrStr.sin_addr.S_un.S_addr) != 1)
	{
		cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	};

	// setup own address structure 
	ownAddrStr.sin_family = AF_INET;
	ownAddrStr.sin_port = htons(OWN_LISTENING_PORT);
	ownAddrStr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//Bind Socket
	if (bind(s, (struct sockaddr*)&ownAddrStr, sizeof(ownAddrStr)) == SOCKET_ERROR)
	{
		cout << "Bind failed with error code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}


	//==================== Sending thread call ===========================
	thread sendingThread(
		send_to_server,
		ref(s),
		ref(serverAddrStr));

	
	thread processThread(
		processMsg,
		ref(db),
		ref(invitation_db),
		ref(received_messages_queue),
		ref(sending_messages_queue));
	//==================== Sending thread call  ===========================

	//==================== Free running UI thread call ===========================

	thread thread_UI(
		menu,
		ref(db),
		ref(socket_mutex),
		ref(sending_messages_queue),
		ref(received_messages_queue),
		ref(CLIENT_IP)
	);

	//==================== Free running UI thread call ===========================


	//==================== Receiving while loop ===========================
	char receive_buffer[BUFLEN];
	while (true)
	{
		memset(receive_buffer, '\0', BUFLEN);

		try
		{
			if (recvfrom(s, receive_buffer, (BUFLEN - 1), 0, NULL, NULL) == SOCKET_ERROR)
			{
				throw WSAGetLastError();
				// cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
				// exit(EXIT_FAILURE);
			};

			if ((receive_buffer[0] != '\0'))
			{
				string receivedStr = string(receive_buffer);

				// TODO: REMOVE BELOW TEST PRINT
				cout << "======= Test Print =======" << endl;
				cout << "Received Data" << endl;
				cout << receivedStr << endl;
				cout << "======= Test =======" << endl;
				// TODO: REMOVE ABOVE TEST PRINT


				json receivedMsg = json::parse(receivedStr);
				queueHelper::push_to_queue(received_messages_queue, receivedMsg);

				bool saved = false;
				while (!saved)
				{
					if (logFileMutex.try_lock())
					{
						logger::add_received_log(config.SENT_RECEIVED_LOG_PATH, receivedMsg);
						saved = true;
						logFileMutex.unlock();
					}
				}

				if (debugResendToClientAfterReceive)
				{
					// push_to_queue(sending_messages_queue, json::parse(receivedStr));
					queueHelper::push_to_queue(sending_messages_queue,
					                           queueHelper::get_queue_top(received_messages_queue));
				}
			}
		}
		catch (int e)
		{
			if (e == 10054)
			{
				// igonre this error code
			}
			else
			{
				cout << "recvfrom() failed with error code : " << e << endl;
			}
		}
	}
}

void processMsg(json& db, vector<json>& invitations_db, std::queue<json>& received_messages_queue,
                std::queue<json>& sending_messages_queue)
{
	while (true)
	{
		if (!received_messages_queue.empty())
		{
			processMessages(db, invitations_db, received_messages_queue, sending_messages_queue);

			queueHelper::pop_from_queue(received_messages_queue);
			// received_messages_queue.pop();
		}
	}
}

void send_to_server(SOCKET s, sockaddr_in serverAddrStr)
{
	bool debug = false;
	// bool debug = true;
	char buf[BUFLEN];

	while (true)
	{
		if (!(sending_messages_queue.empty()))
		{
			// getting top of queue
			json messageJsonObj = queueHelper::get_queue_top(sending_messages_queue);

			try
			{
				// serializing it to string from json to send
				string messageJsonStr = messageJsonObj.dump();

				if (debug)
				{
					json testJson;
					testJson["Test param 1"] = "Test string to send from client";
					testJson["Test param 2"] = "This should work";
					messageJsonStr = testJson.dump();
				}

				memset(buf, '\0', BUFLEN);
				messageJsonStr.copy(buf, messageJsonStr.size());

				if (socket_mutex.try_lock())
				{
					// sending the serialized string
					if (sendto(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr*>(&serverAddrStr),
					           sizeof(serverAddrStr)) == SOCKET_ERROR)
					{
						cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
						exit(EXIT_FAILURE);
					}
					socket_mutex.unlock();
				}
				json sentMessageToPop = queueHelper::get_queue_top(sending_messages_queue);
				queueHelper::pop_from_queue(sending_messages_queue);

				bool saved = false;
				while (!saved)
				{
					if (logFileMutex.try_lock())
					{
						logger::add_sent_log(config.SENT_RECEIVED_LOG_PATH, sentMessageToPop);
						saved = true;
						logFileMutex.unlock();
					}
				}
			}
			catch (int e)
			{
				cout << "sendto() failed with error code : " << e << endl;
			}
		}
	}
}


bool get_client_local_ip(SOCKET s, string& client_local_ip)
{
	// --- Reference --- for the code below: https://stackoverflow.com/questions/49335001/get-local-ip-address-in-c/49336660

	sockaddr_in loopback;

	std::memset(&loopback, 0, sizeof(loopback));
	loopback.sin_family = AF_INET;
	loopback.sin_addr.s_addr = INADDR_LOOPBACK; // using loopback ip address
	loopback.sin_port = htons(9); // using debug port

	if (connect(s, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == -1)
	{
		std::cerr << "Could not connect\n";
		// exit(EXIT_FAILURE);
		return false;
	}

	socklen_t addrlen = sizeof(loopback);
	if (getsockname(s, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == -1)
	{
		std::cerr << "Could not getsockname\n";
		// exit(EXIT_FAILURE);
		return false;
	}


	char buf[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0)
	{
		std::cerr << "Could not inet_ntop\n";
		// exit(EXIT_FAILURE);
		return false;
	}
	else
	{
		client_local_ip = string(buf);
		// std::cout << "Client Local IP address: " << client_local_ip << "\n";
	}

	return true;

	// --- Reference --- for the code above: https://stackoverflow.com/questions/49335001/get-local-ip-address-in-c/49336660
}
