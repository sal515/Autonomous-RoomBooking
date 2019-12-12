#pragma once
#include "pch.h"

using json = nlohmann::json;


static std::mutex queue_mutex;


struct queueHelper
{
	static void pop_from_queue(std::queue<json>& queue);
	static void push_to_queue(std::queue<json>& queue, const json& data);
	static json get_queue_top(std::queue<json>& queue);
};
