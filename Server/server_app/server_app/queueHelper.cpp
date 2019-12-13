#include "pch.h"
#include "queueHelper.h"

socket_messages queueHelper::get_queue_top(std::queue<socket_messages>& queue)
{
	socket_messages msg = queue.front();
	return msg;
}

void queueHelper::push_to_queue(std::queue<socket_messages>& queue, const socket_messages& data)
// void queueHelper::push_to_queue(std::queue<socket_messages>& queue, std::mutex &queue_mutex, const socket_messages & data)
{
	bool saved = false;
	while (!saved)
	{
		if (queue_mutex.try_lock())
		{
			queue.push(data);
			queue_mutex.unlock();
			saved = true;
			break;
		}
	}
}

void queueHelper::pop_from_queue(std::queue<socket_messages>& queue)
// void queueHelper::pop_from_queue(std::queue<socket_messages>& queue, std::mutex &queue_mutex)
{
	bool deleted = false;
	while (!deleted)
	{
		if (queue_mutex.try_lock())
		{
			queue.pop();
			queue_mutex.unlock();
			deleted = true;
			break;
		}
	}
}
