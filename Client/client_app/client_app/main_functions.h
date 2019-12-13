#pragma once
#pragma once
#include "pch.h"

using json = nlohmann::json;


extern std::mutex socket_mutex;
// extern int REQUEST_COUNTER;
extern std::atomic<int> REQUEST_COUNTER;

static std::string CLIENT_IP;


struct send_receive
{
	string send = "send";
	string receive = "receive";
};


string ask_for_ip();

void menu(
	json& db, 
	vector<json>& invitations_db,
	std::mutex& socketMutex, 
	std::queue<json>& sendingQueue, 
	std::queue<json>& receivingQueue,
	const string& ownIP
);

vector<string> list_of_participants(int min);

bool check_ip(const string& ip);

bool is_string_a_number(string choiceStr);

// bool check_schedule(json schedule);


// void pop_from_queue(std::queue<json>& queue);
// void push_to_queue(std::queue<json>& queue, const json& data);
// json get_queue_top(std::queue<json>& queue);


// void use_socket_with_lock(
// 	const string sendOrReceive,
// 	json &data,
// 	std::queue<json>& queue,
// 	// std::mutex& socketMutex,
// 	SOCKET &s,
// 	sockaddr_in &client_struct,
// 	int &client_struct_len
// 	// ,char buf[32768]
// );




// ==================  Examples  ======================================
int test_pause_exit();
