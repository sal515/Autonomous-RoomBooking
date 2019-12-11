#pragma once
#include "pch.h"

void processMessages(json &db, json& pendingdb, const json &req_data, const string & requesterIP, std::atomic <int> &global_meet_id,
	SOCKET s, sockaddr_in server_struct, int server_struct_len, std::queue<socket_messages>& sending_messages_queue);
sockaddr_in clientMaker(string);
void send_message_client(string ip, std::queue<socket_messages>& sending_messages_queue, string &msg);
