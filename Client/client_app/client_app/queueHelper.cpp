#include "pch.h"
#include "queueHelper.h"

void queueHelper::pop_from_queue(std::queue<json>& queue)
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

void queueHelper::push_to_queue(std::queue<json>& queue, const json& data)
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

json queueHelper::get_queue_top(std::queue<json>& queue)
{
	json msg = queue.front();
	return msg;
}
