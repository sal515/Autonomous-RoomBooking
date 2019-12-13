// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H


#include <iostream>
// socket library
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Linking Winsock Library
#include <ws2tcpip.h> // Used to convert IPv4 or IPv6 addressed to standard binary and vice versa

// Other libraries
#include <ctime>
#include <ctype.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>
#include <cstdio>

#include <queue>          // std::queue
#include <thread>         // std::thread
#include <future>         // std::async, std::future
#include <mutex>          // std::mutex
#include <atomic>         // std::atomic


#include <windows.h>

#include "socket_messages.h"
#include "queueHelper.h"
#include "logger.h"
#include "db_helper.h"
#include "app_config.h"
#include "time_day_room.h"
#include "meeting.h"
#include "json_messages.h"


#include "json.hpp"

// #include "spdlog/spdlog.h"
// #include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
// #include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging
#define PCH_H
#ifndef _MSG
#define _MSG

#endif //_MSG
// TODO: add headers that you want to pre-compile here

#endif //PCH_H
