#pragma once
#include "pch.h"

void processMessages(json& db,
                     json& pendingdb,
                     const json& req_data,
                     const string& requesterIP,
                     std::atomic<int>& global_meet_id,
                     std::queue<socket_messages>& sending_messages_queue);
sockaddr_in clientMaker(string);
void send_message_client(const string &ip, std::queue<socket_messages>& sending_messages_queue, const json& msg);
