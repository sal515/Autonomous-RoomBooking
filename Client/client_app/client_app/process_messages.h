#pragma once
#include "pch.h"

void processMessages(nlohmann::json& db, vector<nlohmann::json> &invitations_db, std::queue<nlohmann::json>& received_messages_queue, std::queue<nlohmann::json>& sending_messages_queue);
