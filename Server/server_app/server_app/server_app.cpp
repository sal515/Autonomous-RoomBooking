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
#define SERVER_PORT 8888    //The port on which to listen for incoming data

// Mutexes 
std::mutex socket_mutex;
// std::mutex queue_mutex;
std::mutex confirmedDBMutex;
std::mutex pendingDBMutex;


// Global variables in use
queue<json> messages_queue; // queues for messages from the clients
json confirmed_db;
json pending_db;
std::atomic<int> global_meet_id(0);
std::atomic<bool> exit_program(false);

// string CLIENT_IP;
string CLIENT_IP = "192.168.0.115"; // TODO: Delete this debug one
USHORT CLIENT_PORT = 9999;
// USHORT CLIENT_PORT = 8888; // DEBUG ONLY - send it to the server itself to test data

struct sockaddr_in client_struct;
int client_struct_len = sizeof(client_struct);


// Global variables in use
std::queue<socket_messages> received_messages_queue; // queue for messages received from server
std::queue<socket_messages> sending_messages_queue; // queue for messages to be sent from the clients

bool debugResendToClientAfterReceive = false;
// bool debugResendToClientAfterReceive = true;
bool debugTestData = false;
// bool debugTestData = true;
// bool resetDatabases = false;
bool resetDatabases = true;


// Function prototype

// Queue Accessor function both on server and client
// socket_messages get_queue_top(std::queue<socket_messages>& queue);
// void pop_from_queue(std::queue<socket_messages>& queue);
// void push_to_queue(std::queue<socket_messages>& queue, const socket_messages& data);
void maintenance_ui();
int get_other_room(int roomNameInt);


// Please do not call this function - Its already threaded
void send_to_client(SOCKET s, sockaddr_in clientAddrStr);
void processMsg(std::queue<socket_messages>& received_messages_queue,
                std::queue<socket_messages>& sending_messages_queue,
                json& db, json& pendingdb, SOCKET s, sockaddr_in server_struct, int server_struct_len);


int main(void)
{
	// ============= Initialization of database ==========================

	if (!resetDatabases)
	{
		cout << "Reset databases: (y/n)" << endl;
		char resetDB = 'y';
		// cin >> resetDB;
		resetDB = tolower(resetDB);
		if (resetDB == 'y')
		{
			db_helper::removeDirectory(config.DIR_LOCAL_STORAGE);
		}
	}


	// db_helper::removeDirectory(clientPath.DIR_LOCAL_STORAGE);
	db_helper::createDirectory(config.DIR_LOCAL_STORAGE);
	db_helper::initialize_db(config.PENDING_DB_PATH, config.CONFIRMED_DB_PATH);
	logger::initialize_log_file(config.SENT_RECEIVED_LOG_PATH);


	// loading db from file to memory
	confirmed_db = db_helper::db_to_json(config.CONFIRMED_DB_PATH);
	pending_db = db_helper::db_to_json(config.PENDING_DB_PATH);
	// ============= Initialization of database ==========================


	// TODO: Delete test =====================
	// 
	// maintenance_ui();

	// json jsonMsg;
	// jsonMsg["message"] = "REQUEST";
	// jsonMsg["meetingDay"] = "monday";
	// jsonMsg["meetingTime"] = "10";
	// jsonMsg["requestID"] = "1";
	// jsonMsg["topic"] = "yomama";
	// jsonMsg["participantsIP"] = json::array({});
	// jsonMsg["participantsIP"].push_back("192.168.1.133");
	// jsonMsg["participantsIP"].push_back("192.168.0.188");
	// string iptemp = "111.111.111.111";
	//
	//
	// socket_messages sock;
	// sock.message = jsonMsg;
	// sock.ip_for_message = iptemp;
	// logger::add_received_log(config.SENT_RECEIVED_LOG_PATH, sock);
	//
	// test_pause_exit();
	//
	// return 0;

	// TODO: Delete test =====================


	// --------------- Test codes below  -------------------------
	if (debugTestData)
	{
		// json jsonMsg;
		// jsonMsg["message"] = "REQUEST";
		// jsonMsg["meetingDay"] = "monday";
		// jsonMsg["meetingTime"] = "10";
		// jsonMsg["requestID"] = "1";
		// jsonMsg["topic"] = "yomama";
		// jsonMsg["participantsIP"] = json::array({});
		// jsonMsg["participantsIP"].push_back("192.168.1.133");
		// jsonMsg["participantsIP"].push_back("192.168.0.188");
		// string iptemp = "111.111.111.111";
		//
		// socket_messages sockMsg;
		// sockMsg.message = jsonMsg;
		// sockMsg.ip_for_message = CLIENT_IP;
		// // push_to_queue(sending_messages_queue, sockMsg);
		// queueHelper::push_to_queue(sending_messages_queue, sockMsg);
		//
		// jsonMsg["message"] = "REQUEST";
		// jsonMsg["day"] = "monday";
		// jsonMsg["time"] = "10";
		// jsonMsg["requestID"] = "1";
		// jsonMsg["topic"] = "checking second multiple client";
		// jsonMsg["participantsIP"] = json::array({});
		// jsonMsg["participantsIP"].push_back("192.168.1.133");
		// jsonMsg["participantsIP"].push_back("192.168.0.188");
		// iptemp = "111.111.111.111";
		//
		// sockMsg.message = jsonMsg;
		// sockMsg.ip_for_message = "192.168.0.106";
		// queueHelper::push_to_queue(sending_messages_queue, sockMsg);
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
	if (bind(s, (struct sockaddr*)&serverAddrStr, sizeof(serverAddrStr)) == SOCKET_ERROR)
	{
		cout << "Bind failed with error code : " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}


	// ==================== Socket setup and binding =====================


	//==================== Sending thread call ===========================
	char received_buffer[BUFLEN];
	thread sendingThread(send_to_client, ref(s), ref(clientAddrStr));
	thread processingThread(processMsg, ref(received_messages_queue), ref(sending_messages_queue), ref(confirmed_db),
	                        ref(pending_db), ref(s), ref(clientAddrStr), (sizeof(clientAddrStr)));
	//==================== Sending thread call  ===========================

	thread maintenanceUI(maintenance_ui);


	//==================== Receiving while loop ===========================
	cout << "Waiting for data..." << endl;
	fflush(stdout);


	while (true)
	{
		//clear the buffer by filling null, it might have previously received data
		memset(received_buffer, '\0', BUFLEN);

		try
		{
			if ((recvfrom(s, received_buffer, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr*>(&serverAddrStr),
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
				queueHelper::push_to_queue(received_messages_queue, socketMsgToPush);

				bool saved = false;
				while (!saved)
				{
					if (logFileMutex.try_lock())
					{
						logger::add_received_log(config.SENT_RECEIVED_LOG_PATH, socketMsgToPush);
						saved = true;
						logFileMutex.unlock();
					}
				}

				if (debugResendToClientAfterReceive)
				{
					// push_to_queue(sending_messages_queue, socketMsgToPush);
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

	//==================== Receiving while loop ===========================


	//==================== Program clean up code below ===========================
	processingThread.detach();
	sendingThread.join();

	closesocket(s);
	WSACleanup();
	return 0;
}

void processMsg(std::queue<socket_messages>& received_messages_queue,
                std::queue<socket_messages>& sending_messages_queue,
                json& db, json& pendingdb, SOCKET s, sockaddr_in server_struct, int server_struct_len)
{
	while (true)
	{
		if (!received_messages_queue.empty())
		{
			processMessages(db,
			                pendingdb,
			                received_messages_queue.front().message,
			                received_messages_queue.front().ip_for_message,
			                global_meet_id,
			                sending_messages_queue);
			queueHelper::pop_from_queue(received_messages_queue);
			// received_messages_queue.pop();
		}
	}
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
			socket_messages socket_message = queueHelper::get_queue_top(sending_messages_queue);

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

				// if (debug1)
				// {
				// 	json testJson;
				// 	testJson["Test param 1"] = "Test string to send from server";
				// 	testJson["Test param 2"] = "This should work";
				// 	messageJsonStr = testJson.dump();
				// }

				memset(buf, '\0', BUFLEN);
				messageJsonStr.copy(buf, messageJsonStr.size());


				if (socket_mutex.try_lock())
				{
					//send the messageJsonStr
					if (sendto(s, buf, (BUFLEN - 1), 0, reinterpret_cast<struct sockaddr*>(&clientAddrStr),
					           sizeof(clientAddrStr)) == SOCKET_ERROR)
					{
						// cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
						// exit(EXIT_FAILURE);
						throw WSAGetLastError();
					}
					socket_mutex.unlock();
				}

				socket_messages sockMsgToLog = queueHelper::get_queue_top(sending_messages_queue);
				queueHelper::pop_from_queue(sending_messages_queue);

				bool saved = false;
				while (!saved)
				{
					if (logFileMutex.try_lock())
					{
						logger::add_sent_log(config.SENT_RECEIVED_LOG_PATH, sockMsgToLog);
						saved = true;
						logFileMutex.unlock();
					}
				}
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


int get_other_room(int roomNameInt)
{
	if (roomNameInt == 0)
	{
		return 1;
	}
	else if (roomNameInt == 1)
	{
		return 0;
	}
	else
	{
		cout << "Something wrong with the room - get_other_room() " << endl;
		exit(0);
	}
}

void maintenance_ui()
{
	// room maintenance UI

	const vector<string> all_rooms = time_day_room::room_vec();
	int counter = 0;
	string hour = "";
	string roomName = "";
	int roomNameInt = -1;
	bool properSelection = false;


	string day = "";
	map<string, string>::iterator day_it;
	int hh;

	bool saved = false;


	while (true)
	{
		saved = false;

		counter = 0;
		hour = "";
		roomName = "";
		roomNameInt = -1;
		properSelection = false;
		day = "";
		hh = -1;

		// cout << "Please choose a room from the following options to schedule maintenance:" << endl;
		cout << "Please select one of the integer values to select the corresponding room for maintenance: " << endl;
		for (const string& room : all_rooms)
		{
			cout << counter++ << " - " << room << endl;
		}


		while (!properSelection)
		{
			try
			{
				cout << "Enter your selection: " << endl;
				cin >> roomName;
				roomNameInt = std::stoi(roomName);
				properSelection = true;
				if ((roomNameInt < 0 || roomNameInt > (all_rooms.size() - 1)))
				{
					properSelection = false;
				}
				// break;
			}
			catch (const std::invalid_argument& ia)
			{
				properSelection = false;
			}
		}


		map<string, string> dayMap = time_day_room::day_map();
		cout << "Please provide a working day of the week eg. friday:\n"
			<< "Day: ";
		cin >> day;
		day_it = dayMap.find((day));
		while (day_it == dayMap.end())
		{
			cout << "Please provide a working day of the week eg. friday :\n"
				<< "Day: ";
			cin >> day;
			day_it = dayMap.find(day);
		}


		cout << "\nTime of meeting between " << time_day_room::startTime << " and " << time_day_room::
			endTime << ": " << endl;
		cin >> hh;

		while (hh > time_day_room::endTime || hh < time_day_room::startTime)
		{
			cout << "\nPlease input a valid time between " << time_day_room::startTime << " and " <<
				time_day_room::endTime << ": " << endl;
			cin >> hh;
		}

		hour = std::to_string(hh);


		// switch (roomNameInt)
		// {
		// case (0):
		// 	{
		cout << "You selected " << all_rooms.at(roomNameInt) << " for maintenance" << endl;
		cout << "The day and time of maintenance is set to " << day << " at " << hh << endl;


		/*
		 *	Summary of the logic -  Maintenance of a room
		 *
		// check confirmed db if the room is busy at the time and date
		// if(meeting exist in the room to be maintained)
		//		check the other room
		//		if (the other room is busy)
		//			get the meeting to be cancelled
		//			cancel the meeting by informing participants and requester
		//			update the slot with a maintenance meeting obj in the db
		//			save db
		//
		//		else if (the other room is not busy)
		//			if (check someone is waiting for the other room in the pending db)
		//				get the meeting obj of the other room
		//				cancel ppl in the pending db
		//				delete pending db meeting obj after cancellation
		//				save pendingDB
		//
		//				-->>THEN MOVE THE PPL in the confirmed db to the new room
		//				set the initial room to maintained by updating with maintenance obj
		//				save the confrimedDB
		//
		//			else
		//				do not do anything
		//				-->>THEN MOVE THE PPL in the confirmed db to the new room
		//				save the confirmedDB
		//
		//	else if(there is no meeting in the room to be mainted)
		//		save a meeting object to the time slot with the requesterIP of 000.000.000.000
		//		to identify that it is under maintenance
		//		save the confirmedDB
		//
		*/


		// create a meeting object to block the room for maintenance
		meeting maintenanceMeettingObj(
			"MAINTENANCE",
			"",
			"",
			"",
			vector<string>{},
			vector<string>{},
			all_rooms.at(roomNameInt),
			"",
			day,
			hour,
			"000.000.000.000",
			""
		);


		// check if a meeting exist at the specifid time
		if (meeting::isMeeting(confirmed_db, day, hour, all_rooms.at(roomNameInt)))
		{
			// the other room at the specifid time has a confirmed meeting
			if (meeting::isMeeting(confirmed_db, day, hour, all_rooms.at(get_other_room(roomNameInt))))
			{
				// other room is busy


				json meetingToBeCancelledJson = meeting::
					get_meeting(confirmed_db, day, hour, all_rooms.at(roomNameInt));
				meeting meetingToBeCancelledObj = meeting::json_to_meetingObj(meetingToBeCancelledJson);

				// create message to inform participants
				json cancelled = messages::cancel(
					meetingToBeCancelledObj.meetingID,
					"Unavoidable maintenance is scheduled for the room and no other room was available at the specified time."
				);

				// create message to inform requester
				json cancelledToRequester = messages::not_sched(
					meetingToBeCancelledObj.requestID,
					meetingToBeCancelledObj.meetingDay,
					meetingToBeCancelledObj.meetingTime,
					meetingToBeCancelledObj.minimumParticipants,
					meetingToBeCancelledObj.confirmedParticipantsIP,
					meetingToBeCancelledObj.topic
				);

				// send message to all participants
				for (const string& participantIP : meetingToBeCancelledObj.invitedParticipantsIP)
				{
					socket_messages cancellationMsgToSendToClient;
					cancellationMsgToSendToClient.ip_for_message = participantIP;
					cancellationMsgToSendToClient.message = cancelled;
					queueHelper::push_to_queue(sending_messages_queue, cancellationMsgToSendToClient);
				}

				// send message to requester
				socket_messages cancellationMsgToSendToServer;
				cancellationMsgToSendToServer.ip_for_message = meetingToBeCancelledObj.requesterIP;
				cancellationMsgToSendToServer.message = cancelled;
				queueHelper::push_to_queue(sending_messages_queue, cancellationMsgToSendToServer);

				// store a maintenance object in the time slot

				// create a meeting object to block the room for maintenance (created above)

				// adding a maintenance obj at the time slot in the db
				meeting::update_meeting(confirmed_db, day, hour, all_rooms.at(roomNameInt),
				                        meeting::meetingObj_to_json(maintenanceMeettingObj));
				saved = false;
				while (!saved)
				{
					if (confirmedDBMutex.try_lock())
					{
						db_helper::save_db(config.CONFIRMED_DB_PATH, confirmed_db);
						saved = true;
						confirmedDBMutex.unlock();
					}
				}
			}
			else
			{
				// other room is not busy

				// check if someone in the pending db is waiting for the time slot of the other room
				if (meeting::isMeeting(pending_db, day, hour, all_rooms.at(get_other_room(roomNameInt))))
				{
					// cancel the ppl waiting for the other room in the pending db
					json meetingToBeCancelledJson = meeting::get_meeting(pending_db, day, hour,
					                                                     all_rooms.at(get_other_room(roomNameInt)));
					meeting meetingToBeCancelledObj = meeting::json_to_meetingObj(meetingToBeCancelledJson);

					// create message to inform participants
					json cancelled = messages::cancel(
						meetingToBeCancelledObj.meetingID,
						"Unavoidable maintenance was scheduled from a different room but the previously booked room is moved to this room."
					);

					// create message to inform requester
					json cancelledToRequester = messages::not_sched(
						meetingToBeCancelledObj.requestID,
						meetingToBeCancelledObj.meetingDay,
						meetingToBeCancelledObj.meetingTime,
						meetingToBeCancelledObj.minimumParticipants,
						meetingToBeCancelledObj.confirmedParticipantsIP,
						meetingToBeCancelledObj.topic
					);

					// send message to all participants
					for (const string& participantIP : meetingToBeCancelledObj.invitedParticipantsIP)
					{
						socket_messages cancellationMsgToSendToClient;
						cancellationMsgToSendToClient.ip_for_message = participantIP;
						cancellationMsgToSendToClient.message = cancelled;
						queueHelper::push_to_queue(sending_messages_queue, cancellationMsgToSendToClient);
					}

					// send message to requester
					socket_messages cancellationMsgToSendToServer;
					cancellationMsgToSendToServer.ip_for_message = meetingToBeCancelledObj.requesterIP;
					cancellationMsgToSendToServer.message = cancelled;
					queueHelper::push_to_queue(sending_messages_queue, cancellationMsgToSendToServer);

					// delete pending db meeting object
					meeting::update_meeting(pending_db, day, hour, all_rooms.at(get_other_room(roomNameInt)),
					                        json({}));

					// save the pending db to file

					saved = false;
					while (!saved)
					{
						if (pendingDBMutex.try_lock())
						{
							db_helper::save_db(config.PENDING_DB_PATH, pending_db);
							saved = true;
							pendingDBMutex.unlock();
						}
					}

					// move the ppl from the room to be maintained to the other room in the confirmed db
					json meetingToBeMovedToNewRoom = meeting::get_meeting(confirmed_db, day, hour,
					                                                      all_rooms.at(roomNameInt));

					meeting::update_meeting(confirmed_db, day, hour, all_rooms.at(get_other_room(roomNameInt)),
					                        meetingToBeMovedToNewRoom);

					// set the initial room in question to be under maintenance with maintenance meeting obj
					meeting::update_meeting(confirmed_db, day, hour, all_rooms.at((roomNameInt)),
					                        meeting::meetingObj_to_json(maintenanceMeettingObj));

					// save the confirmed db to file
					saved = false;
					while (!saved)
					{
						if (confirmedDBMutex.try_lock())
						{
							db_helper::save_db(config.CONFIRMED_DB_PATH, confirmed_db);
							saved = true;
							confirmedDBMutex.unlock();
						}
					}
				}
					// no one is waiting for the room in the pending db
				else
				{
					// move the ppl from the room to be maintained to the other room
					json meetingToBeMovedToNewRoom = meeting::get_meeting(confirmed_db, day, hour,
					                                                      all_rooms.at(roomNameInt));

					meeting::update_meeting(confirmed_db, day, hour, all_rooms.at(get_other_room(roomNameInt)),
					                        meetingToBeMovedToNewRoom);

					// set the initial room in question to be under maintenance with maintenance meeting obj
					meeting::update_meeting(confirmed_db, day, hour, all_rooms.at((roomNameInt)),
					                        meeting::meetingObj_to_json(maintenanceMeettingObj));

					// save the confirmed db to file
					saved = false;
					while (!saved)
					{
						if (!confirmedDBMutex.try_lock())
						{
							db_helper::save_db(config.CONFIRMED_DB_PATH, confirmed_db);
							saved = true;
							confirmedDBMutex.unlock();
						}
					}
				}
			}
		}
			// no meeting exist at the specified time
		else
		{
			// create a meeting object to block the room for maintenance (created above)

			// adding a maintenance obj at the time slot in the db
			meeting::update_meeting(confirmed_db, day, hour, all_rooms.at(roomNameInt),
			                        meeting::meetingObj_to_json(maintenanceMeettingObj));
			saved = false;
			while (!saved)
			{
				if (confirmedDBMutex.try_lock())
				{
					cout << "saved to db" << endl;
					db_helper::save_db(config.CONFIRMED_DB_PATH, confirmed_db);
					saved = true;
					confirmedDBMutex.unlock();
				}
			}
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
