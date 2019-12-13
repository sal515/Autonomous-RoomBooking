#include "pch.h"

#include "main_functions.h"


bool autonomous = true;

void menu(
	json& db,
	json& invitation_vec,
	std::mutex& socketMutex,
	std::queue<json>& sendingQueue,
	std::queue<json>& receivingQueue,
	const string& ownIP)
{
	string choiceStr;
	int choice;
	// string choice;
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
				<< "9. Exit the Program\n"
				<< "10. Debug: Book a room\n";
			cin >> choiceStr;

			choice = -1;
			if (is_string_a_number(choiceStr))
			{
				choice = std::stoi(choiceStr);
			}

			time_day_room possibilities;
			map<string, string> dayMap = possibilities.day_map();
			map<string, string>::iterator day_it;

			switch (choice)
			{
			case 1:
				{
					string day;
					// int mm, dd, yyyy, 
					int hh;
					int min_participants;
					string topic;

					cout << "Please provide a working day of the week eg. friday:\n"
						<< "Day: ";
					cin >> day;
					day_it = dayMap.find((day));
					while (day_it == dayMap.end())
					{
						cout << "Please provide a working day of the week eg. friday :\n"
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

					if (!db.at(day).at(std::to_string(hh)).empty())
					{
						cout <<
							"A meeting already exists, cannot schedule a new meeting without cancellation or withdrawal of the current meeting."
							<< endl;

						break;
					};


					cout << "\nTopic of meeting: ";
					cin >> topic;
					cout << "\nMinimum number of Participants: ";
					cin >> min_participants;
					vector<string> participants = list_of_participants(min_participants);

					//Copy into json to send to server + implement request id with incremental?
					//variables are day, time, topic, min_participants and participants(list)
					goodInput = true;
					// string dayStr = std::to_string(day);
					string requestID = std::to_string(REQUEST_COUNTER++);
					string timeH = std::to_string(hh);
					string minParts = std::to_string(min_participants);
					// string topic_str = std::to_string(topic);


					// TODO: Check if it available to request

					json request = messages::request(
						requestID,
						day,
						timeH,
						minParts,
						participants,
						topic
					);

					meeting requestMetObj = meeting(
						messageType.request,
						minParts,
						requestID,
						"",
						participants,
						vector<string>{},
						"",
						topic,
						day,
						timeH,
						ownIP,
						false,
						false
					);

					meeting::update_meeting(db, day, timeH, meeting::meetingObj_to_json(requestMetObj));
					db_helper::save_db(config.DB_PATH, db);

					queueHelper::push_to_queue(sendingQueue, request);

					break;
				}
			case 2:
				{
					string meetingID;
					while (db_helper::getMeetingByID(meetingID, db).empty())
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
			case 3:
				{
					if (!autonomous)
					{
						cout << "manual mode" << endl;

						bool subMenu = true;
						while (subMenu)
						{
							cout << "\n1. View your invitations (Accepted and Rejected)\n"
								<< "2. Accept invitation\n"
								<< "3. Decline invitation\n"
								<< "4. Request to join meeting\n"
								<< "5. Withdraw from meeting\n"
								<< "9. Go back to previous menu\n";
							cin >> choice;
							switch (choice)
							{
							case 1: // view invitations
								{
									// for (const auto& dayz : db)
									// {
									// 	for (const auto& timez : dayz)
									// 	{
									// 		if (!(timez).empty())
									// 		{
									// 			if (!messageType.invite.compare((timez).at("message")))
									// 			{
									// 				invites.push_back((timez));
									// 			}
									// 		}
									// 	}
									// }
									//
									//

									for (json invitation : invitation_vec)
									{
										for (int i = 0; i < invitation.size(); i++)
										{
											meeting thisMeet = meeting::json_to_meetingObj(invitation.at(i));
											meeting::print_meeting(thisMeet);
										}
									}

									break;
									//to test
								}
							case 2: // accept invitation
							case 4:
								{
									string meet_ID;
									while (db_helper::getMeetingByID(meet_ID, db).empty())
									{
										cout << "\nPlease enter a meeting ID: ";
										cin >> meet_ID;
									}
									json meetJsonObj = db_helper::getMeetingByID(meet_ID, db);
									meeting thisMeeting = meeting::json_to_meetingObj(meetJsonObj);
									thisMeeting.meetingStatus = true;
									//send msg to RBMS 

									// meeting::update_meeting(db, meetJsonObj.at("day"), meetJsonObj.at("time"), meeting::meetingObj_to_json(thisMeeting));
									// check if meeting in agenda
									// put acceptance if in local agenda.
									//store in json
									goodInput = true;
									break;
								}
							case 3: // decline invitation
							case 5: // withdraw
								{
									string meet_ID;
									while (db_helper::getMeetingByID(meet_ID, db).empty())
									{
										cout << "\nPlease enter a meeting ID: ";
										cin >> meet_ID;
									}
									json meetJsonObj = db_helper::getMeetingByID(meet_ID, db);
									meeting thisMeeting = meeting::json_to_meetingObj(meetJsonObj);
									thisMeeting.meetingStatus = false;
									//send msg to RBMS 

									// meeting::update_meeting(db, meetJsonObj.at("day"), meetJsonObj.at("time"), meeting::meetingObj_to_json(thisMeeting));
									// check if meeting in agenda
									// put withdraw if in local agenda.
									//store in json
									goodInput = true;
									break;
								}
							case 9:
								{
									cout << endl;
									subMenu = false;
									break;
								}
							}
						}
						break;
					}
					cout << "Autonomous mode: this selection doesn't do anything" << endl;
					break;
				}
			case 9:
				{
					// exit_program = true;

					cout << "exiting program";
					return;
				}
			case 10:
				{
					// This is purely a test case

					string requestID = std::to_string(REQUEST_COUNTER++);
					string day = "friday";
					string timeH = "10";
					string minParts = "1";
					vector<string> participants = {
						"192.168.0.183",
						"192.168.0.106",
						"172.31.8.16",
						"192.168.1.133"
					};
					string topic = "Test topic";


					json request = messages::request(
						requestID,
						day,
						timeH,
						minParts,
						participants,
						topic
					);

					meeting requestMetObj = meeting(
						messageType.request,
						minParts,
						requestID,
						"",
						participants,
						vector<string>{},
						"",
						topic,
						day,
						timeH,
						ownIP,
						false,
						false
					);


					if (!db.at(requestMetObj.meetingDay).at(requestMetObj.meetingTime).empty())
					{
						REQUEST_COUNTER--;
						cout <<
							"A meeting already exists, cannot schedule a new meeting without cancellation or withdrawal of the current meeting."
							<< endl;
						break;
					};

					meeting::update_meeting(db, day, timeH, meeting::meetingObj_to_json(requestMetObj));

					db_helper::save_db(config.DB_PATH, db);

					queueHelper::push_to_queue(sendingQueue, request);

					break;
				}
			default:
				cout << "Please enter a valid input\n";
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

bool check_ip(const string& ip)
{
	try
	{
		struct sockaddr_in sa;
		int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
		return result != 0;
	}
	catch (...)
	{
		cout << "Error: converting string to IP" << endl;
	}
}

bool is_string_a_number(string choiceStr)
{
	for (char x : choiceStr)
	{
		if (!isdigit(x))
		{
			return false;
			// break;
		}
	}
	return true;
}

void pop_from_queue(std::queue<json>& queue)
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


void push_to_queue(std::queue<json>& queue, const json& data)
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

json get_queue_top(std::queue<json>& queue)
{
	json msg = queue.front();
	return msg;
}


// bool check_schedule(json schedule)
// {
// 	//look into json file to see if available or not
// 	return false;
// }


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
