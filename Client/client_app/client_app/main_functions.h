#pragma once
#pragma once
#include "pch.h"
#include "global_variables.h"


extern std::mutex socket_mutex;
extern std::mutex queue_mutex;

string ask_for_ip();
void menu(json db, std::mutex& socketMutex, std::queue<json>& sendingQueue, std::queue<json>& receivingQueue,
          const string& ownIP);
vector<string> list_of_participants(int min);
bool check_ip(const string& ip);
// bool check_schedule(json schedule);


void pop_from_queue(std::queue<json>& queue);
void push_to_queue(std::queue<json>& queue, const json& data);
json get_front_of_queue(std::queue<json>& queue);



// void pop_from_queue(std::queue<json>& queue, std::mutex& queueMutex);
// void push_to_queue(std::queue<json>& queue, const json& data, std::mutex& queueMutex);

void use_socket_with_lock(
	const string sendOrReceive,
	json &data,
	std::queue<json>& queue,
	// std::mutex& socketMutex,
	SOCKET &s,
	sockaddr_in &client_struct,
	int &client_struct_len
	// ,char buf[32768]
);


struct send_receive
{
	string send = "send";
	string receive = "receive";
};


// ==================  Examples  ======================================
int test_pause_exit();
