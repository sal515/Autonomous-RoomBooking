#pragma once
#include "pch.h"

static std::mutex queue_mutex;

struct queueHelper
{
	static socket_messages get_queue_top(std::queue<socket_messages>& queue);
	static void push_to_queue(std::queue<socket_messages>& queue, const socket_messages& data);
	static void pop_from_queue(std::queue<socket_messages>& queue);
};
