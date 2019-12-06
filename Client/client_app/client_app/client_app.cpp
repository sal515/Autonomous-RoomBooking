// client_app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// File management: https://www.tutorialspoint.com/cplusplus/cpp_files_streams.htm

#include "pch.h"
#include "main_functions.h"
#include "global_variables.h"


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
#define BUFLEN 32768		//Max length of buffer including 
#define LISTENING_PORT 8888   //The port on which to listen for incoming data

string SERVER_IP_IN;

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
// std::mutex received_message_queue_mutex;
// std::mutex send_message_mutex;

std::mutex socket_mutex;
std::mutex cout_mutex;
std::mutex cin_mutex;

std::mutex db_mutex;

// Global variables in use
std::queue<json> received_messages_queue; // queue for messages received from server
std::queue<json> sending_messages_queue; // queue for messages to be sent from the clients

json confirmed_db;
json pending_db;
std::atomic<int> global_meet_id(0);
std::atomic<bool> exit_program(false);
std::string clientIP;















// Testing getIP function 

// Returns hostname for the local computer 
void checkHostName(int hostname)
{
	if (hostname == -1)
	{
		perror("gethostname");
		exit(1);
	}
}

// Returns host information corresponding to host name 
void checkHostEntry(struct hostent * hostentry)
{
	if (hostentry == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
}

// Converts space-delimited IPv4 addresses 
// to dotted-decimal format 
void checkIPbuffer(char *IPbuffer)
{
	if (NULL == IPbuffer)
	{
		perror("inet_ntoa");
		exit(1);
	}
}


// Testing getIP function 






















int main(void)
{
	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.DB_PATH);

	// loading db from file to memory
	json db = db_helper::db_to_json(config.DB_PATH);



	// --------------- Test codes below  -------------------------
	char hostbuffer[256];
	char *IPbuffer;
	struct hostent *host_entry;
	int hostname;

	addrinfo * addInfo;
	struct addrinfo hints;
	struct addrinfo *result = NULL;


	// To retrieve hostname 
	hostname = gethostname(hostbuffer, sizeof(hostbuffer));
	checkHostName(hostname);

	// To retrieve host information

	getaddrinfo(hostbuffer, "8888", &hints, &result);

	// result->ai_addr->sa_data;
	// GetAddrInfoW()

	// host_entry = gethostbyname(hostbuffer));
	// checkHostEntry(host_entry);

	// To convert an Internet network 
	// address into ASCII string 
	IPbuffer = inet_ntoa(*((struct in_addr*)
		result->ai_addr->sa_data));

	printf("Hostname: %s\n", hostbuffer);
	printf("Host IP: %s", IPbuffer);

	return 0;




	// return Testing_dbHelper_meetingObj();
	//menu(db);

	// delete[] SERVER;

	json jsonMsg;

	jsonMsg["message"] = "REQUEST";
	jsonMsg["day"] = "monday";
	jsonMsg["time"] = "10";
	jsonMsg["requestID"] = "1";
	jsonMsg["topic"] = "yomama";
	jsonMsg["participantsIP"] = json::array({});
	jsonMsg["participantsIP"].push_back("192.168.1.133");
	jsonMsg["participantsIP"].push_back("192.168.0.188");


	// sending_messages_queue.push(jsonMsg);

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
	int client_struct_len = sizeof(client_struct);

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

	// free running thread for UI
	thread thread_UI(
		menu,
		db,
		SERVER_IP_IN
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
	// thread_UI.join();


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
	char buf[BUFLEN];
	while (!exit_program)
	{
		if (!sending_messages_queue.empty())
		{
			socket_mutex.lock();


			// send_message_mutex.lock();
			json messageJsonObj = sending_messages_queue.front();
			sending_messages_queue.pop();
			// send_message_mutex.unlock();

			string messageJsonStr = messageJsonObj.dump();

			memset(buf, '\0', BUFLEN + 1);
			messageJsonStr.copy(buf, messageJsonStr.size());

			// send_message_mutex.lock();

			//send the messageJsonStr
			if (sendto(s, buf, strlen(buf), 0, reinterpret_cast<struct sockaddr *>(&client_struct),
			           client_struct_len) == SOCKET_ERROR)
			{
				cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
				exit(EXIT_FAILURE);
			}
			// send_message_mutex.unlock();


			socket_mutex.unlock();
		}
	}
}


void receive_message_from_server(
	SOCKET s,
	sockaddr_in client_struct,
	int client_struct_len,
	queue<json>& received_messages_queue)
{
	char buf[BUFLEN];

	while (!exit_program)
	{
		socket_mutex.lock();


		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN + 1);

		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, reinterpret_cast<struct sockaddr *>(&client_struct), &client_struct_len) ==
			SOCKET_ERROR)
		{
			cout_mutex.lock();
			cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
			cout_mutex.unlock();
			exit(EXIT_FAILURE);
		}


		string buffer = string(buf);
		json received_data = json::parse(buffer);
		{
			received_data = json({});
		}
		// Saving all the messages received in the global queue
		// received_message_queue_mutex.lock();
		received_messages_queue.push(received_data);
		// received_message_queue_mutex.unlock();

		socket_mutex.unlock();
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
	client_struct.sin_port = htons(LISTENING_PORT);
	// setup the variable: client_struct.sin_addr.S_un
	if (inet_pton(AF_INET, SERVER, &client_struct.sin_addr.S_un.S_addr) != 1)
	{
		cout << "Failed to convert IPv4 or IPv6 to standard binary format " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	};
}


