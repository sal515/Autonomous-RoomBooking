#pragma once
#include "pch.h"
void discarder(json& invitations, 
    const string& requestID, 
    const string& meetingDay, 
    const string& meetingTime, 
    const string& requesterID,
    std::queue<socket_messages>& sending_messages_queue);
void processMessages(json& db,
                     json& pendingdb,
                     const json& req_data,
                     const string& requesterIP,
                     std::atomic<int>& global_meet_id,
                     std::queue<socket_messages>& sending_messages_queue);
sockaddr_in clientMaker(string);
void send_message_client(const string &ip, std::queue<socket_messages>& sending_messages_queue, const json& msg);
