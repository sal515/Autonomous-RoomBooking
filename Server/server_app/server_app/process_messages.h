#pragma once

// struct process_messages
// {
// 	
// };


void processMessages(json &db, json& pendingdb, const json &req_data, const string & requesterIP, std::atomic <int> &global_meet_id);
sockaddr_in clientMaker(string);
void send_message_client(SOCKET,sockaddr_in,int,const string&);
