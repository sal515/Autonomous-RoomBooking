cmake_minimum_required(VERSION 3.15)
project(COEN445_Autonomous_Roombooking)

set(CMAKE_CXX_STANDARD 14)

include_directories(Client/client_app/client_app)
include_directories(json_library_header)
include_directories(Server/server_app/server_app)

add_executable(COEN445_Autonomous_Roombooking
    Client/client_app/client_app/client_app.cpp
    Client/client_app/client_app/dbHelper.cpp
    Client/client_app/client_app/dbHelper.h
    Client/client_app/client_app/json.hpp
    Client/client_app/client_app/pch.cpp
    Client/client_app/client_app/pch.h
    json_library_header/json.hpp
    Server/server_app/server_app/dbHelper.cpp
    Server/server_app/server_app/dbHelper.h
    Server/server_app/server_app/json.hpp
    Server/server_app/server_app/pch.cpp
    Server/server_app/server_app/pch.h
    Server/server_app/server_app/server_app.cpp)
