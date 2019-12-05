#pragma once

// struct process_messages
// {
// 	
// };


void processMessages(json &db, const json &req_data, const string & requesterIP, std::atomic <int> &global_meet_id);