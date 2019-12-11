#pragma once
#include "pch.h"

void processMessages(json& db, vector<json> invitations_db, std::queue<json>& received_messages_queue, std::queue<json>& sending_messages_queue);
