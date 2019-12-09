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


struct socket_messages
{
	string ip_for_message;
	json message;
};


// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
#define BUFLEN 32768		//Max length of buffer 
#define SERVER_PORT 8888    //The port on which to listen for incoming data

// Mutexes 
std::mutex socket_mutex;
std::mutex queue_mutex;

// Global variables in use
queue<json> messages_queue; // queues for messages from the clients
json confirmed_db;
json pending_db;
std::atomic<int> global_meet_id(0);
std::atomic<bool> exit_program(false);

// string CLIENT_IP;
string CLIENT_IP = "192.168.0.115";  // TODO: Delete this debug one
USHORT CLIENT_PORT = 9999;			
// USHORT CLIENT_PORT = 8888; // DEBUG ONLY - send it to the server itself to test data

struct sockaddr_in client_struct;
int client_struct_len = sizeof(client_struct);


// Global variables in use
std::queue<socket_messages> received_messages_queue; // queue for messages received from server
std::queue<socket_messages> sending_messages_queue; // queue for messages to be sent from the clients

// bool debugResendToClientAfterReceive = true;
bool debugResendToClientAfterReceive = false;
bool debugTestData = true;
// bool debugTestData = false;


// Function prototype
socket_messages get_queue_top(std::queue<socket_messages>& queue);
void pop_from_queue(std::queue<socket_messages>& queue);
void push_to_queue(std::queue<socket_messages>& queue, const socket_messages& data);
void send_to_client(SOCKET s, sockaddr_in clientAddrStr);


int main(void)
{
	// ============= Initialization of database ==========================
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.PENDING_DB_PATH, config.CONFIRMED_DB_PATH);

	// loading db from file to memory
	confirmed_db = db_helper::db_to_json(config.CONFIRMED_DB_PATH);
	pending_db = db_helper::db_to_json(config.PENDING_DB_PATH);
	// ============= Initialization of database ==========================


	// --------------- Test codes below  -------------------------
	if (debugTestData)
	{
		json jsonMsg;
		jsonMsg["message"] = "REQUEST";
		jsonMsg["day"] = "monday";
		jsonMsg["time"] = "10";
		jsonMsg["requestID"] = "1";
		jsonMsg["topic"] = "yomama";
		jsonMsg["participantsIP"] = json::array({});
		jsonMsg["participantsIP"].push_back("192.168.1.133");
		jsonMsg["participantsIP"].push_back("192.168.0.188");
		string iptemp = "111.111.111.111";

		socket_messages sockMsg;
		sockMsg.message = jsonMsg;
		sockMsg.ip_for_message = CLIENT_IP;
		push_to_queue(sending_messages_queue, sockMsg);
		
		jsonMsg["message"] = "REQUEST";
		jsonMsg["day"] = "monday";
		jsonMsg["time"] = "10";
		jsonMsg["requestID"] = "1";
		jsonMsg["topic"] = "checking second multiple client";
		jsonMsg["participantsIP"] = json::array({});
		jsonMsg["participantsIP"].push_back("192.168.1.133");
		jsonMsg["participantsIP"].push_back("192.168.0.188");
		iptemp = "111.111.111.111";

		sockMsg.message = jsonMsg;
		sockMsg.ip_for_message = "192.168.0.106";
		push_to_queue(sending_messages_queue, sockMsg);
	}
	// --------------- Test codes above -------------------------


	// ==================== Socket setup and binding =====================
	// Declare the socket varaibles
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

	// setup server address structure 
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

	// ==================== Socket setup and binding =====================


	//==================== Sending thread call ===========================
	char received_buffer[BUFLEN];
	thread sendingThread(send_to_client, ref(s), ref(clientAddrStr));
	//==================== Sending thread call  ===========================


	//==================== Receiving while loop ===========================
	cout << "Waiting for data..." << endl;
	fflush(stdout);

	//clear the buffer by filling null, it might have previously received data
	memset(received_buffer, '\0', BUFLEN);


	while (true)
	{
		try
		{
			if ((recvfrom(s, received_buffer, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&serverAddrStr),
			              &serverAddrStr_len)) ==
				SOCKET_ERROR)
			{
				throw WSAGetLastError();
				// cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
				// exit(EXIT_FAILURE);
			}


			//print details of the client/peer and the data received
			char printable_IP_add_buf[INET_ADDRSTRLEN];
			if ((inet_ntop(AF_INET, &serverAddrStr.sin_addr, printable_IP_add_buf, INET_ADDRSTRLEN)) != nullptr)
			{
				CLIENT_IP = string(printable_IP_add_buf);
			}

			// Debug print the client ip where data was received from if needed
			cout << "Received from CLIENT_IP: " << CLIENT_IP << endl;

			// while (!(buf[0] == '\0'))
			if ((received_buffer[0] != '\0'))
			{
				string receivedStr = std::string(received_buffer);

				// TODO: REMOVE BELOW TEST PRINT
				cout << "======= Test Print =======" << endl;
				cout << "Received Data" << endl;
				cout << receivedStr << endl;
				cout << "======= Test =======" << endl;
				// TODO: REMOVE ABOVE TEST PRINT

				// storing received message to the received queue
				socket_messages socketMsgToPush;
				socketMsgToPush.message = json::parse(receivedStr);
				socketMsgToPush.ip_for_message = CLIENT_IP;
				push_to_queue(received_messages_queue, socketMsgToPush);


				if (debugResendToClientAfterReceive)
				{
					// push_to_queue(sending_messages_queue, socketMsgToPush);
					push_to_queue(sending_messages_queue, get_queue_top(received_messages_queue));
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

	//==================== Receiving while loop ===========================


	//==================== Program clean up code below ===========================

	sendingThread.join();
	closesocket(s);
	WSACleanup();
	return 0;
}


void send_to_client(SOCKET s, sockaddr_in clientAddrStr)
{
	bool debug1 = false;
	// bool debug = true;
	char buf[BUFLEN];

	while (true)
	{
		if (!(sending_messages_queue.empty()))
		{
			// getting top of queue
			socket_messages socket_message = get_queue_top(sending_messages_queue);

			json messageJsonObj = socket_message.message;
			string client_IP_to_send = socket_message.ip_for_message;

			try
			{
				clientAddrStr.sin_family = AF_INET;
				clientAddrStr.sin_port = htons(CLIENT_PORT);
				if (socket_mutex.try_lock())
				{
					// setup the variable: client_struct.sin_addr.S_un
					if (inet_pton(clientAddrStr.sin_family, client_IP_to_send.c_str(),
					              &clientAddrStr.sin_addr) != 1)
					{
						throw WSAGetLastError();
						// cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
						// exit(EXIT_FAILURE);
					};
					socket_mutex.unlock();
				}
			}
			catch (int e)
			{
				cout << "Failed to convert IPv4 or IPv6 to standard binary format " << e << endl;
			}


			try
			{
				// serializing it to string from json to send
				string messageJsonStr = messageJsonObj.dump();

				if (debug1)
				{
					json testJson;
					testJson["Test param 1"] = "Test string to send from server";
					testJson["Test param 2"] = "This should work";
					messageJsonStr = testJson.dump();
				}

				memset(buf, '\0', BUFLEN);
				messageJsonStr.copy(buf, messageJsonStr.size());


				if (socket_mutex.try_lock())
				{
					//send the messageJsonStr
					if (sendto(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&clientAddrStr),
					           sizeof(clientAddrStr)) == SOCKET_ERROR)
					{
						// cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
						// exit(EXIT_FAILURE);
						throw WSAGetLastError();
					}
					socket_mutex.unlock();
				}
				pop_from_queue(sending_messages_queue);
			}
			catch (int e)
			{
				cout << "sendto() failed with error code : " << e << endl;
			}
			// sleep for 5000 ms
			// sleepcp(5000);
		}
	}
}


bool is_string_a_number(string choiceStr)
{
	for (char x : choiceStr)
	{
		if (!isdigit(x))
		{
			return false;
			// break;
		}
	}
	return true;
}

void pop_from_queue(std::queue<socket_messages>& queue)
{
	bool deleted = false;
	while (!deleted)
	{
		if (queue_mutex.try_lock())
		{
			queue.pop();
			queue_mutex.unlock();
			deleted = true;
			break;
		}
	}
}

void push_to_queue(std::queue<socket_messages>& queue, const socket_messages& data)
{
	bool saved = false;
	while (!saved)
	{
		if (queue_mutex.try_lock())
		{
			queue.push(data);
			queue_mutex.unlock();
			saved = true;
			break;
		}
	}
}


socket_messages get_queue_top(std::queue<socket_messages>& queue)
{
	socket_messages msg = queue.front();
	return msg;
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
