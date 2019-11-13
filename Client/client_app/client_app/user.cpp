#include "pch.h"
#include "user.h"

json user::userObj_to_json(const user &usr)
{
	json user_json = 
	{
		{"ip", usr.ip},
		{"listeningPort", usr.listeningPort},
		{"userName", usr.userName}
	};
	return user_json;
}
