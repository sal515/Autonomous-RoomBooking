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

// Example Function prototypes
int test_pause_exit();


// Note: If I want to send x characters my buff has to be x+1 for '\0' character at the end
// Defined in PCH
// #define BUFLEN 32768		//Max length of buffer including 
// #define SERVER_LISTENING_PORT 8888   //The port on which to listen for incoming data


// Function prototypes
string ask_for_ip();


void create_client_socket(
	char* SERVER,
	SOCKET& s,
	sockaddr_in& client_struct);

void send_message_to_server(
	SOCKET s,
	sockaddr_in client_struct,
	int client_struct_len,
	queue<json>& sending_messages_queue);

void receive_message_from_server(
	SOCKET s,
	sockaddr_in client_struct,
	int client_struct_len,
	queue<json>& received_messages_queue);


// Mutexes 
std::mutex db_mutex;
std::mutex socket_mutex;
std::mutex queue_mutex;


// Global variables in use
std::queue<json> received_messages_queue; // queue for messages received from server
std::queue<json> sending_messages_queue; // queue for messages to be sent from the clients

string SERVER_IP_IN;
string CLIENT_LOCAL_IP;
std::atomic<int> REQUEST_COUNTER(0);

json confirmed_db;
json pending_db;
// std::atomic<int> REQUEST_COUNTER(0);
std::atomic<bool> exit_program(false);
std::string clientIP;


// Function prototypes
bool get_client_local_ip(SOCKET s, string& client_local_ip);


struct sockaddr_in server_struct;
int server_struct_len = sizeof(server_struct);


int main(void)
{
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.DB_PATH);

	// loading db from file to memory
	json db = db_helper::db_to_json(config.DB_PATH);


	// --------------- Test codes below  -------------------------
	json jsonMsg;

	jsonMsg["message"] = "REQUEST";
	jsonMsg["day"] = "monday";
	jsonMsg["time"] = "10";
	jsonMsg["requestID"] = "1";
	jsonMsg["topic"] = "yomama";
	jsonMsg["participantsIP"] = json::array({});
	jsonMsg["participantsIP"].push_back("192.168.1.133");
	jsonMsg["participantsIP"].push_back("192.168.0.188");

	sending_messages_queue.push(jsonMsg);

	// --------------- Test codes above -------------------------


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
	// struct sockaddr_in server_struct;
	int client_struct_len = sizeof(client_struct);
	// int server_struct_len = sizeof(server_struct);

	//Initialise winsock
	cout << "\nInitialising Winsock... " << endl;
	if (WSAStartup(MAKEWORD(2, 2), &win_socket_struct) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Initialised Winsock" << endl;

	// creating socket for the client to communicate with the server
	create_client_socket(SERVER, s, client_struct);

	// Get the local client ip to attach in the messages when sending request
	if (get_client_local_ip(s, CLIENT_LOCAL_IP))
	{
		cout << "Client local ip is " << CLIENT_LOCAL_IP << endl;
	}
	else
	{
		cout << "Client local ip was not found." << endl;
	};

	// free running thread for UI
	thread thread_UI(
		menu,
		db,
		ref(socket_mutex),
		ref(sending_messages_queue),
		ref(received_messages_queue),
		ref(CLIENT_LOCAL_IP)
	);

	thread thread_send_message(
		send_message_to_server,
		s,
		client_struct,
		client_struct_len,
		ref(sending_messages_queue)
	);

	thread thread_receive_message(
		receive_message_from_server,
		s,
		client_struct,
		client_struct_len,
		ref(received_messages_queue));

	// TODO: Run process_messages thread

	// Wait for all the threads to finish for safe exit of main 
	thread_receive_message.join();
	thread_send_message.join();

	closesocket(s);
	WSACleanup();

	return 0;
}

void send_message_to_server(
	SOCKET s,
	sockaddr_in client_struct,
	int client_struct_len,
	queue<json>& sending_messages_queue)
{
	while (true)
	{
		if (!sending_messages_queue.empty())
		{
			// const send_receive sndOrRecv;
			json messageJsonObj = get_front_of_queue(sending_messages_queue);


			string messageJsonStr = messageJsonObj.dump();
			char buf[BUFLEN];
			// memset(buf, '\0', BUFLEN + 1);
			memset(buf, '\0', BUFLEN);
			messageJsonStr.copy(buf, messageJsonStr.size());

			//send the messageJsonStr
			if (sendto(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&client_struct),
			           client_struct_len) == SOCKET_ERROR)
			{
				cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
				exit(EXIT_FAILURE);
			}
			else
			{
				pop_from_queue(sending_messages_queue);
			}


			// use_socket_with_lock(
			// 	sndOrRecv.send,
			// 	messageJsonObj,
			// 	sending_messages_queue,
			// 	s,
			// 	client_struct,
			// 	client_struct_len
			// );
		}
	}
}


void receive_message_from_server(
	SOCKET s,
	sockaddr_in client_struct,
	int client_struct_len,
	queue<json>& received_messages_queue)
{
	while (true)
	{
		// const send_receive sndOrRecv;
		json received_data = json({});


		// server_struct.sin_family = AF_INET;
		// server_struct.sin_port = htons(OWN_LISTENING_PORT);
		//
		// if (inet_pton(server_struct.sin_family, SERVER_IP_IN.c_str(), &server_struct.sin_addr.S_un.S_addr) != 1)
		// {
		// 	cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
		// 	exit(EXIT_FAILURE);
		// };


		char buf[BUFLEN];
		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		// //try to receive some data, this is a blocking call
		// if (recvfrom(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr *>(&server_struct),
		//              &server_struct_len) ==
		// 	SOCKET_ERROR)
		// {
		// 	cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
		// 	exit(EXIT_FAILURE);
		// }

		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, (BUFLEN - 1), 0, NULL,
		             NULL) ==
			SOCKET_ERROR)
		{
			cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
			exit(EXIT_FAILURE);
		}


		string buffer = string(buf);

		cout << buffer << endl;

		received_data = json::parse(buffer);


		// use_socket_with_lock(
		// 	sndOrRecv.receive,
		// 	received_data,
		// 	received_messages_queue,
		// 	s,
		// 	client_struct,
		// 	client_struct_len
		// );


		push_to_queue(received_messages_queue, received_data);
	}
}

void create_client_socket(char* SERVER, SOCKET& s, sockaddr_in& client_struct)
{
	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset(reinterpret_cast<char *>(&client_struct), 0, sizeof(client_struct));

	client_struct.sin_family = AF_INET;
	client_struct.sin_port = htons(SERVER_LISTENING_PORT);
	// setup the variable: client_struct.sin_addr.S_un
	if (inet_pton(AF_INET, SERVER, &client_struct.sin_addr.S_un.S_addr) != 1)
	{
		cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	};
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
