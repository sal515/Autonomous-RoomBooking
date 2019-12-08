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
#define PORT 45678   //The port on which to listen for incoming data


void create_n_bind_server_socket(
	WSADATA win_socket_struct,
	SOCKET& s,
	sockaddr_in&
	server_struct);

void send_message_to_client(
	SOCKET s,
	sockaddr_in server_struct,
	int server_struct_len,
	const string& messageJsonStr);

void receive_message_from_client(
	SOCKET s,
	sockaddr_in server_struct,
	int server_struct_len,
	queue<json>& messages,
	string& clientIP
);


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


int main(void)
{
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.PENDING_DB_PATH, config.CONFIRMED_DB_PATH);

	// loading db from file to memory
	confirmed_db = db_helper::db_to_json(config.CONFIRMED_DB_PATH);
	pending_db = db_helper::db_to_json(config.PENDING_DB_PATH);


	string clientIp = "1.1.1.1";
	// json received_data;

	// // // Test: messageType.request
	// received_data.clear();
	// received_data["message"] = messageType.request;
	// received_data["requestID"] = "2";
	// received_data["meetingDay"] = "friday";
	// received_data["meetingTime"] = "10";
	// received_data["invitedParticipantsIP"] = vector<string> {"1.1.1.1", "2.2.2.2"};
	// received_data["minimumParticipants"] = "1";
	// received_data["topic"] = "Testing request message";
	//
	// // Process the request data 
	// processMessages(pending_db, received_data, clientIp);
	//

	// // Test: messageType.cancelRequest
	// received_data.clear();
	// received_data["message"] = messageType.cancelResponse;
	// received_data["meetingID"] = "1";
	// received_data["reason"] = "Testing is the reason";

	// Process the request data 
	//processMessages(db, received_data,"0.0.0.0");


	// thread readThread(sendMessageToClient, "This A");
	// thread uiThread(receiveMessageFromClient, "This B");

	// readThread.join();
	// uiThread.join();


	/*
	 * Socket Variables
	 */

	WSADATA win_socket_struct;
	SOCKET s;

	struct sockaddr_in server_struct;
	// struct sockaddr_in si_other;

	int server_struct_len = sizeof(server_struct);
	int recv_len;

	//Initializing winsocket to win_socket_struct variable
	cout << "\nInitialising Winsock... " << endl;
	if (WSAStartup(MAKEWORD(2, 2), &win_socket_struct) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Initialised Winsock" << endl;

	// creating the socket from the initialized socket struct variable
	create_n_bind_server_socket(win_socket_struct, s, server_struct);

	// keep listening for data
	thread thread_receive_message(receive_message_from_client,
	                              s,
	                              server_struct,
	                              server_struct_len,
	                              ref(messages_queue),
	                              ref(clientIP)
	);


	// while (!exit_program)
	// {
	// 	if (!messages_queue.empty())
	// 	{
	// 		json messageJsonObj = messages_queue.front();
	// 		messages_queue.pop();
	//
	//
	// 		// TODO : create separate threads to process each message depending on ?message type?
	// 		// thread process_received_message(
	// 		// processMessages,
	// 		// ref(pending_db), 
	// 		// ref(received_data), 
	// 		// ref(clientIP)
	// 		// );
	// 	}
	//
	// }
	// cout_mutex.lock();
	// cout << "exiting message queue processing" << endl;
	// cout_mutex.unlock();

	// send_message_to_client(
	// 	s,
	// 	server_struct,
	// 	server_struct_len,
	// 	buf);


		// Wait for all the threads to finish for safe exit of main 
	thread_receive_message.join();
	// process_received_message.join();


	closesocket(s);
	WSACleanup();

	// test_pause_exit();

	return 0;
}


void send_message_to_client(
	SOCKET s,
	sockaddr_in server_struct,
	int server_struct_len,
	const json& messageJsonObj)
{
	const string messageJsonStr = messageJsonObj.dump();
	char buf[BUFLEN];
	memset(buf, '\0', BUFLEN);
	messageJsonStr.copy(buf, messageJsonStr.size());

	// send_message_mutex.lock();
	// //now reply the client with the same data
	if (sendto(s, buf, (BUFLEN -1), 0, (struct sockaddr*)&server_struct, server_struct_len) == SOCKET_ERROR)
	{
		cout << "sendto() failed with error code : " << WSAGetLastError << endl;
		exit(EXIT_FAILURE);
	}
	// send_message_mutex.unlock();
}

void receive_message_from_client(
	SOCKET s,
	sockaddr_in server_struct,
	int server_struct_len,
	queue<json>& received_messages_queue,
	string& clientIP
)
{
	char buf[BUFLEN];

	while (!exit_program)
	{
		// cout_mutex.try_lock();
		cout << "Waiting for data..." << endl;
		fflush(stdout);
		// cout_mutex.unlock();

		//clear the buffer by filling null, it might have previously received data
		// memset(buf, '\0', BUFLEN + 1);
		memset(buf, '\0', BUFLEN);

		if ((recvfrom(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&server_struct),
		              &server_struct_len)) ==
			SOCKET_ERROR)
		{
			// cout_mutex.try_lock();
			cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
			// cout_mutex.unlock();
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		char printable_IP_add_buf[INET_ADDRSTRLEN];
		if ((inet_ntop(AF_INET, &server_struct.sin_addr, printable_IP_add_buf, INET_ADDRSTRLEN)) != nullptr)
		{
			clientIP = string(printable_IP_add_buf);
		}

		// TODO: DELETE Test Print when it is time
		// cout_mutex.try_lock();
		cout << "clientIP: " << clientIP << endl;
		// cout_mutex.unlock();

		string buffer = std::string(buf);
		// std::cout << "Data Recieved from client IP - " << clientIP << ": " << buffer << std::endl;

		json received_data = (json::parse(buffer));
		if (received_data.empty())
		{
			received_data = json({});
		}

		cout << received_data << endl;

		// Saving all the messages received in the global queue
		// received_message_queue_mutex.lock();
		received_messages_queue.push(received_data);
		// received_message_queue_mutex.unlock();

		if (!received_messages_queue.empty())
		{
			send_message_to_client(s, server_struct, server_struct_len, received_messages_queue.front());
		}
	}


	// cout_mutex.lock();
	cout << "exiting receiving thread" << endl;
	// cout_mutex.unlock();
}

void create_n_bind_server_socket(
	WSADATA win_socket_struct,
	SOCKET& s,
	sockaddr_in& server_struct)
{
	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		cout << "Could not create socket : " << WSAGetLastError() << endl;
	}
	cout << "Socket created" << endl;

	//Prepare the sockaddr_in structure
	server_struct.sin_family = AF_INET;
	server_struct.sin_addr.S_un.S_addr = INADDR_ANY;
	// server_struct.sin_addr.s_addr = INADDR_ANY;
	server_struct.sin_port = htons(PORT);

	//Bind Socket
	if (bind(s, (struct sockaddr *)&server_struct, sizeof(server_struct)) == SOCKET_ERROR)
	{
		cout << "Bind failed with error code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Bind done" << endl;
}


// Other variables (unrelated to sockets)
// std::string server_hostname;
// std::string client_hostname;

// char c_hostname[128];
// if ((gethostname(c_hostname, sizeof c_hostname) != 0))
// {
// 	std::cout << "Error while retrieving hostname.Error Code : " << WSAGetLastError() << std::endl;
// }

// if (c_hostname != nullptr)
// {
// 	server_hostname = std::string(c_hostname);
// 	std::cout << "Hostname of computer is: " << server_hostname << std::endl;
// }


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
