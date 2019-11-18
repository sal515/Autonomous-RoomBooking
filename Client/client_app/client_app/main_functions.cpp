#include "pch.h"

#include "main_functions.h"

string ask_for_ip()
{
	string SERVER_IP_IN;
	cout << "Please input the server IP" << endl;
	cin >> SERVER_IP_IN;

	if (SERVER_IP_IN == "d")
	{
		SERVER_IP_IN = config.DEBUG_HARDCODED_SERVER_IP_IN;
	}
	return SERVER_IP_IN;
}


void menu(json db)
{
	int reqCounter = 1;
	char choice;
	bool goodInput = false;

	while (true)
	{
		while (!goodInput)
		{
			cout << "Please select from the following options:\n"
				<< "1. Book a Room\n"
				<< "2. Cancel a Meeting\n"
				<< "3. Check inbox for invitations\n"
				<< "4. Request to Participate\n"
				<< "9. Exit the Program\n";
			cin >> choice;

			time_day_room possibilities;
			map<string, string> dayMap = possibilities.day_map();
			map<string, string>::iterator day_it;

			switch (choice)
			{
			case '1':
			{
				string day;
				// int mm, dd, yyyy, 
				int hh;
				int min_participants;
				string topic;

				cout << "Please provide the day of the week:\n"
					<< "Day: ";
				cin >> day;
				day_it = dayMap.find(day);
				while (day_it == dayMap.end())
				{
					cout << "Please provide the day of the week:\n"
						<< "Day: ";
					cin >> day;
					day_it = dayMap.find(day);
				}

				cout << "\nTime of meeting between " << time_day_room::startTime << " and " << time_day_room::
					endTime << ": " << endl;
				cin >> hh;
				while (hh > time_day_room::endTime || hh < time_day_room::startTime)
				{
					cout << "\nPlease input a valid time between " << time_day_room::startTime << " and " <<
						time_day_room::endTime << ": " << endl;
					cin >> hh;
				}

				cout << "\nTopic of meeting: ";
				cin >> topic;
				cout << "\nMinimum number of Participants: ";
				cin >> min_participants;
				vector<string> participants = list_of_participants(min_participants);

				//Copy into json to send to server + implement request id with incremental?
				//variables are day, time, topic, min_participants and participants(list)
				goodInput = true;
				string requestID = std::to_string(reqCounter);
				string timeH = std::to_string(hh);
				string minParts = std::to_string(min_participants);
				messages message;
				// json reqz = message.request(requestID, date, timeH, minParts, participants, topic);

				//put in json obj.
				break;
			}
			case '2':
			{
				string meetingID;
				while (getMeetID(meetingID, db).is_null())
				{
					cout << "\nPlease enter a meeting ID: ";
					cin >> meetingID;
				}
				//check if meeting in agenda.
				//check if meeting creator is same IP.
				//if it is, send cancellation.
				goodInput = true;
				messages message;
				json cancel = message.cancelMeet(meetingID);
				break;
			}
			case '3':
			{
				bool subMenu = true;
				while (subMenu){
					cout << "\n1. View your invitations (Accepted and Rejected)\n"
						<< "2. Accept invitation\n"
						<< "3. Decline invitation\n"
						<< "4. Request to join meeting\n"
						<< "9. Go back to previous menu\n";
					cin >> choice;
					switch (choice) {
					case '1':		// view invitations
					{
						vector<json> invites;
						for (const auto& dayz : db)
						{
							for (const auto& timez : dayz)
							{
								if (!(timez).empty()) {
									if (!messageType.invite.compare((timez).at("message"))) {
										invites.push_back((timez));
									}
								}
							}
						}
						for (int i = 0; i < invites.size(); i++) {
							meeting thisMeet = meeting::json_to_meetingObj(invites.at(i));
							meeting::print_meeting(thisMeet);
						}
						break;
						//to test
					}
					case '2':		// accept invitation
					case '4':
					{
						string meet_ID;
						while (getMeetID(meet_ID, db).is_null())
						{
							cout << "\nPlease enter a meeting ID: ";
							cin >> meet_ID;
						}
						json meetJsonObj = getMeetID(meet_ID, db);
						meeting thisMeeting = meeting::json_to_meetingObj(meetJsonObj);
						thisMeeting.meetingStatus = true;
						//send msg to RBMS 

						meeting::client_update_meeting(db, meetJsonObj.at("day"), meetJsonObj.at("time"), meeting::meetingObj_to_json(thisMeeting));
						// check if meeting in agenda
							// put acceptance if in local agenda.
							//store in json
						goodInput = true;
						break;
					}
					case '3':		// accept invitation
						{
							string meet_ID;
							while (getMeetID(meet_ID, db).is_null())
							{
								cout << "\nPlease enter a meeting ID: ";
								cin >> meet_ID;
							}
							json meetJsonObj = getMeetID(meet_ID, db);
							meeting thisMeeting = meeting::json_to_meetingObj(meetJsonObj);
							thisMeeting.meetingStatus = false;
							//send msg to RBMS 

							meeting::client_update_meeting(db, meetJsonObj.at("day"), meetJsonObj.at("time"), meeting::meetingObj_to_json(thisMeeting));
							// check if meeting in agenda
								// put withdraw if in local agenda.
								//store in json
							goodInput = true;
							break;
						}
					case '9':
						{
						cout << endl;
						subMenu = false;
						break;
						}
					}
				}
				break;
			}
			case '9':
			{
				cout << "exiting program";
				return;
			}
			default:
				cout << "Please enter a valid input";
			}
		}
		goodInput = false;
	}
}



vector<string> list_of_participants(int min)
{
	vector<string> participants;
	string participants_menu =
		"\na. add new participant\n v. view participants\n f. finish\n Please enter your selection: ";
	string ip;
	bool loop = true;
	char choice = 'm';
	while (loop)
	{
		cout << participants_menu;
		cin >> choice;
		switch (choice)
		{
			// case 'm': //show menu
			// 	cout << participants_menu;
			// 	cin >> choice;
			// 	break;
		case 'a': //add participants
		{
			cout << "IP: ";
			cin >> ip;
			if (participants.empty())
			{
				if (check_ip(ip))
				{
					participants.push_back(ip);
				}
				else
				{
					cout << "IP address is not valid.\n";
				}
			}
			else
			{
				vector<string>::iterator it = find(participants.begin(), participants.end(), ip);
				if (it != participants.end())
				{
					cout << "Participant is already part of the list\n";
				}
				else
				{
					if (check_ip(ip))
					{
						participants.push_back(ip);
					}
					else
					{
						cout << "IP address is not valid.\n";
					}
				}
			}
			break;
		}
		case 'v':
		{
			//output list of participants
			for (auto participant : participants)
			{
				cout << participant << endl;
			}
			break;
		}
		case 'f':
		{
			//check if list is complete
			if (min <= static_cast<int>(participants.size()))
			{
				loop = false;
			}
			else
			{
				cout << "The total number of participants is smaller than the minimum required.\nYou need to add "
					<< min - participants.size() << " more participants.\n";
			}
			break;
		}
		case 'e':
		{
			loop = false;
			break;
		}
		default:
		{
			cout << "Please input a valid choice.\n";
		}
		}
	}
	return participants;
}

json getMeetID(const string& meetingID, const json& db)
{
	//add funciton to look in agenda
	json day;
	json time;
	map<string, string> day_map = time_day_room::day_map();
	map<string, string> time_map = time_day_room::time_map(time_day_room::startTime, time_day_room::endTime);
	for (const auto& element : day_map)
	{
		for (const auto& element1 : time_map)
		{
			if (!db.at(element.first).at(element1.first).empty()) {
				if (!messageType.invite.compare(db.at(element.first).at(element1.first).at("message"))) {
					return db.at(element.first).at(element1.first);
				}
			}
		}
	}
	json emptyJ;
	return emptyJ;
}


bool check_ip(const string& ip)
{
	try {
		struct sockaddr_in sa;
		int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
		return result != 0;
	}
	catch (...) {
		cout << "Error: converting string to IP" << endl;
	}
}

bool check_schedule(json schedule)
{
	//look into json file to see if available or not
	return false;
}


// function expects the string in format dd/mm/yyyy:
// bool extract_date(const string& s, int& d, int& m, int& y)
// {
// 	std::istringstream is(s);
// 	char delimiter;
// 	if (is >> d >> delimiter >> m >> delimiter >> y)
// 	{
// 		struct tm t = { 0 };
// 		t.tm_mday = d;
// 		t.tm_mon = m - 1;
// 		t.tm_year = y - 1900;
// 		t.tm_isdst = -1;
//
// 		// normalize:
// 		time_t when = mktime(&t);
// 		// const struct tm *norm = localtime(&when);
// 		// the actual date would be:
// 		// m = norm->tm_mon + 1;
// 		// d = norm->tm_mday;
// 		// y = norm->tm_year;
// 		// e.g. 29/02/2013 would become 01/03/2013
//
// 		// validate (is the normalized date still the same?):
// 		return (norm->tm_mday == d &&
// 			norm->tm_mon == m - 1 &&
// 			norm->tm_year == y - 1900);
// 	}
// 	return false;
// }

// ==================  Examples  ======================================
int test_pause_exit()
{
	int pause = 0;
	cin >> pause;
	return 0;
}

