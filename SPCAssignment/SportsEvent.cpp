#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <iomanip>
using namespace std;

struct User {
	string username;
	string password;
	string email;
	bool limit = false;
};

struct Event {
	string eventName;
	string date;
	string time;
	string venue;
	string details;
	string equipments;
	string status = "Scheduled";
	string organiserEmail;
	double fee = 0.0;
	int maxParticipants = 0;
	vector<string> participants;
};

struct EventLog {
	string eventName;
	string organiserEmail;
	string eventNote;
};

struct EventFeedback {
	string eventName;
	string userEmail;
	string feedback;
};

const string USER_FILE = "users.txt";
const string ORGANISER_FILE = "organisers.txt";
const string EVENT_FILE = "events.txt";
const string LOG_FILE = "eventlogs.txt";
const string FEEDBACK_FILE = "feedback.txt";
const string HISTORY_FILE = "history.txt";

void clearScreen();
void mainMenu();
void userMenu(User& currentUser, vector<Event>& events);
void organiserMenu(User& currentUser, vector<Event>& events , EventLog& log , vector<EventFeedback> feedbacks , const vector<User> users);
void registerUser(string role);
bool loginUser(string role, User& currentUser);
bool isValidEmail(string email);
bool emailExists(string email, string role);
bool usernameExists(string username, string role);
bool isValidDate(int day, int month, int year);
void createEvent(User& currentUser, vector<Event>& events);
void joinEvent(User& currentUser, vector<Event>& events);
int payment(double total);
bool isValidTime(string& time);
int timeToMinutes(const string& timeStr);
void monitorEventMenu();
void emchoice(User& currentUser, vector<Event>& events , EventLog& log , vector<EventFeedback> feedbacks , const vector<User> users);
void viewOngoingEvents(User& currentUser, vector<Event>& events);
void updateEventStatus(User& currentUser, vector<User>& users ,vector<Event>& events);
void createUser(User& u, string role);
void updateUserFile(User& currentUser, string role);
vector<Event> getAllEvents(const string& filename);
vector<EventFeedback> getFeedback(const string& filename);
vector<User> getAllUsers(const string& filename);
void updateEvents(const vector<Event>& events);
void statusMenu();
void eventReport(User& currentUser, const vector<Event>& events);
void createEventLog(const EventLog& log);
void addEventLog(User& currentUser, const vector<Event>& events);
void viewEventLogs(User& currentUser);
void addFeedback(User& currentUser, const vector<Event>& events);
void createFeedback(const User& currentUser, const Event& e);
void viewFeedback(User& currentUser, const vector<Event> events);
void viewEventBillboard(const vector<Event>& events);
void cancelEventRegistration(User& currentUser, vector<Event>& events);
void saveHistory(const User& currentUser, const string& action, const string& eventName);
void viewHistory(const User& currentUser);
void viewParticipantList(User& currentUser, const vector<Event> events, const vector<User> users);
void currentEventTemplate(const Event& e, User& currentUser);
void resetLimit(User& currentUser, vector<User>& users, Event& e);

string getNonEmptyInput(const string& prompt) {
	string input;
	do {
		cout << prompt;
		getline(cin, input);
		if (input.empty()) {
			cout << "Input cannot be empty. Please try again.\n";
		}
	} while (input.empty());
	return input;
}

int getValidInt(const string& prompt) {
	string input;
	while (true) {
		cout << prompt;
		getline(cin, input);

		if (input.empty()) {
			cout << "Input cannot be empty\n";
			continue;
		}

		bool digitsOnly = true;
		for (char c : input) {
			if (!isdigit(c)) {
				digitsOnly = false;
				break;
			}
		}

		if (digitsOnly) {
			return stoi(input);
		}
		else {
			cout << "Invalid input. Please enter numbers only.\n";
		}
	}
}

void clearScreen() {
	system("cls");
}

int main() {
	User currentUser;
	EventLog log;
	string choice;
	vector<Event> events = getAllEvents("events.txt");
	vector<EventFeedback> feedbacks = getFeedback("feedback.txt");
	vector <User> users = getAllUsers("users.txt");

	do {
		mainMenu();
		choice = getNonEmptyInput("Enter your choice: ");

		if (choice == "1") {
			cout << "\nRegister as:\n";
			cout << "1. User\n";
			cout << "2. Organiser\n";
			string roleChoice = getNonEmptyInput("Choice: ");

			if (roleChoice == "1") registerUser("user");
			else if (roleChoice == "2") registerUser("organiser");
			else cout << "Invalid choice.\n";
		}
		else if (choice == "2") {
			cout << "\nLogin as:\n";
			cout << "1. User\n";
			cout << "2. Organiser\n";
			string roleChoice = getNonEmptyInput("Choice: ");

			bool success = false;
			int attempts = 0;
			while (!success && attempts < 3) {
				if (roleChoice == "1") {
					success = loginUser("user", currentUser);
					if (success) userMenu(currentUser, events);
				}
				else if (roleChoice == "2") {
					success = loginUser("organiser", currentUser);
					if (success) organiserMenu(currentUser, events , log , feedbacks , users);
				}
				else {
					cout << "Invalid choice.\n";
					break;
				}
				if (!success) {
					cout << "Login failed. Try again.\n";
					attempts++;
				}
			}
		}
		else if (choice == "3") {
			cout << "Exiting program...\n";
		}
		else {
			cout << "Invalid choice.\n";
		}

	} while (choice != "3");

	return 0;
}

//UserRegistration & Event Booking
void mainMenu() {
	cout << "\n=== MAIN MENU ===\n";
	cout << "1. Register\n";
	cout << "2. Login\n";
	cout << "3. Exit\n";
}

void userMenu(User& currentUser, vector<Event>& events) {
	string choice;
	do {
		cout << "\n=== USER MENU ===\n";
		cout << "1. Join Event\n";
		cout << "2. Events Billboard\n";
		cout << "3. Give Feedback\n";
		cout << "4. Cancel Event Registration\n";
		cout << "5. View History\n";
		cout << "6. Return\n";

		choice = getNonEmptyInput("Choice: ");

		if (choice == "1") {
			clearScreen();
			joinEvent(currentUser, events);
		}
		else if (choice == "2") {
			clearScreen();
			viewEventBillboard(events);
		}
		else if (choice == "3") {
			clearScreen();
			addFeedback(currentUser , events);
		}
		else if (choice == "4") {
			clearScreen();
			cancelEventRegistration(currentUser, events);
		}
		else if (choice == "5") {
			clearScreen();
			viewHistory(currentUser);
		}
		else if (choice == "6") {
			cout << "Return";
		}
		else {
			cout << "Invalid choice.\n";
		}
	} while (choice != "6");
}

void organiserMenu(User& currentUser, vector<Event>& events , EventLog& log , vector<EventFeedback> feedbacks , const vector<User> users) {
	string choice;

	while(true) {
		cout << "\n=== ORGANISER MENU ===\n";
		cout << "1. Register New Event\n";
		cout << "2. Monitor Event\n";
		cout << "3. Event Reports\n";
		cout << "4. View Event Feedbacks\n";
		cout << "5. Return\n";
		choice = getNonEmptyInput("Choice: ");

		if (choice == "1") {
			clearScreen();
			createEvent(currentUser, events);
		}
		else if (choice == "2") {
			clearScreen();
			emchoice(currentUser, events , log , feedbacks , users);
		}
		else if (choice == "3") {
			clearScreen();
			eventReport(currentUser, events);
		}
		else if (choice == "4") {
			clearScreen();
			viewFeedback(currentUser , events);
		}
		else if (choice == "5") {
			cout << "Logging out\n";
			break;
		}
		else {
			cout << "Invalid choice.\n";
		}
	}
}

void registerUser(string role) {
	User u;
	cout << "\n--- REGISTER " << role << " ---\n";

	u.username = getNonEmptyInput("Enter username: ");
	if (usernameExists(u.username, role)) {
		cout << "Username already exists.\n";
		return;
	}

	u.password = getNonEmptyInput("Enter password: ");
	u.email = getNonEmptyInput("Enter email: ");

	if (!isValidEmail(u.email)) {
		cout << "Invalid email format.\n";
		return;
	}
	if (emailExists(u.email, role)) {
		cout << "Email already registered.\n";
		return;
	}

	createUser(u , role);
	cout << "Registration successful!\n";
}

bool loginUser(string role, User& currentUser) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	string email, password;

	cout << "\n--- " << role << " LOGIN ---\n";
	email = getNonEmptyInput("Email: ");
	password = getNonEmptyInput("Password: ");

	ifstream inFile(fileName);
	if (!inFile) {
		cout << "Error opening file for reading.\n";
		return false;
	}

	string line , uname, pass, mail, limitStr;
	bool found = false;
	while (getline(inFile, line)) {
		stringstream ss(line);

		getline(ss, uname, '|');
		getline(ss, pass, '|');
		getline(ss, mail, '|');
		getline(ss, limitStr);

		if (mail == email && pass == password) {
			cout << role << " login successful!\n";
			found = true;

			currentUser.username = uname;
			currentUser.password = pass;
			currentUser.email = mail;
			currentUser.limit = (limitStr == "1");
			break;
		}
	}

	inFile.close();
	return found;
}

void createEvent(User& currentUser, vector<Event>& events) {
	vector<string> venues = {
		"Stadium 1",
		"Stadium 2",
		"Indoor Sports Complex",
		"FIFA Court 1",
		"FIFA Court 2"
	};

	Event e;
	int day, month, year;
	string startTime, endTime;

	cout << "\n--- REGISTER EVENT ---\n";

	if (currentUser.limit == true) {
		cout << "You Have Reach the Limit of Creating an Event\n";
		return;
	}

	e.eventName = getNonEmptyInput("Enter Sports Event Name: ");

	do {
		day = getValidInt("Enter event day (DD): ");
		month = getValidInt("Enter event month (MM): ");
		year = getValidInt("Enter event year (YYYY): ");

		if (!isValidDate(day, month, year)) {
			cout << "Invalid date. Please try again.\n";
			continue;
		}

		break;
	} while (true);

	e.date = (day < 10 ? "0" : "") + to_string(day) + "/" +
		(month < 10 ? "0" : "") + to_string(month) + "/" +
		to_string(year);

	do {
		startTime = getNonEmptyInput("Enter Start Time (HH:MM): ");
		endTime = getNonEmptyInput("Enter End Time (HH:MM): ");

		if (!isValidTime(startTime) || !isValidTime(endTime)) {
			cout << "Invalid Time Format. Please Enter Again\n";
			continue;
		}

		if (timeToMinutes(endTime) <= timeToMinutes(startTime)) {
			cout << "Invalid! End Time cannot be earlier than Start Time!\n";
			continue;
		}

		break;

	} while (true);

	e.time = startTime + " - " + endTime;


	cout << "\nAvailable Venues:\n";
	for (int i = 0; i < venues.size(); i++) {
		cout << i + 1 << ". " << venues[i] << endl;
	}

	int choice;
	do {
		cout << "Enter your choice (1-" << venues.size() << "): ";
		cin >> choice;

		if (choice < 1 || choice > venues.size()) {
			cout << "Invalid choice! Please try again.\n";
		}

	} while (choice < 1 || choice > venues.size());

	e.venue = venues[choice - 1];
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	cout << "Enter Event Details: ";
	getline(cin, e.details);

	cout << "Enter Needed Equipments: ";
	getline(cin, e.equipments);

	e.maxParticipants = getValidInt("Enter Amount of Participants: ");

	e.fee = getValidInt("Enter Event Fee (RM): ");

	e.organiserEmail = currentUser.email;
	currentUser.limit = true;

	events.push_back(e);
	updateEvents(events);
	updateUserFile(currentUser, "organiser");

	cout << "Event registered successfully!\n";
}

//Validations
bool emailExists(string email, string role) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ifstream inFile;
	inFile.open(fileName.c_str());

	if (!inFile) return false;

	string line, uname, pass, mail, limitStr;
	while (getline(inFile, line)) {
		stringstream ss(line);

		getline(ss, uname, '|');
		getline(ss, pass, '|');
		getline(ss, mail, '|');
		getline(ss, limitStr);

		if (mail == email) return true;
	}
	inFile.close();
	return false;
}

bool usernameExists(string username, string role) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ifstream inFile;
	inFile.open(fileName.c_str());

	if (!inFile) return false;

	string line , uname, pass, mail, limitStr;
	while (getline(inFile, line)) {
		stringstream ss(line);

		getline(ss, uname, '|');
		getline(ss, pass, '|');
		getline(ss, mail, '|');
		getline(ss, limitStr);

		if (uname == username) return true;
	}
	inFile.close();
	return false;
}

int timeToMinutes(const string& timeStr) {
	int hours = stoi(timeStr.substr(0, 2));
	int minutes = stoi(timeStr.substr(3, 2));
	return hours * 60 + minutes;
}

bool isValidDate(int day, int month, int year) {
	if (year < 1900 || year > 2100) return false;
	if (month < 1 || month > 12) return false;

	// For Leap year 
	bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

	if (month == 1 || month == 3 || month == 5 || month == 7 ||
		month == 8 || month == 10 || month == 12) {
		if (day < 1 || day > 31) return false;
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11) {
		if (day < 1 || day > 30) return false;
	}
	else if (month == 2) {
		if (day < 1 || day >(isLeap ? 29 : 28)) return false;
	}

	return true;
}

bool isValidTime(string& time) {
	regex pattern("^(?:[01][0-9]|2[0-3]):[0-5][0-9]$");
	return regex_match(time, pattern);
}

bool isValidEmail(string email) {
	const regex pattern(R"(^[a-z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
	return regex_match(email, pattern);
}

//Files Operations
vector<Event> getAllEvents(const string& filename) {
	ifstream inFile(EVENT_FILE);
	vector<Event> events;

	if (!inFile) {
		cout << "Cannot open file" << endl;
		return events;
	}

	string line;
	while (getline(inFile, line)) {
		stringstream ss(line);
		Event e;
		string maxP_str, fee_str, participants_str;

		if (!getline(ss, e.eventName, '|')) continue;
		if (!getline(ss, e.date, '|')) continue;
		if (!getline(ss, e.time, '|')) continue;
		if (!getline(ss, e.venue, '|')) continue;
		if (!getline(ss, e.details, '|')) continue;
		if (!getline(ss, e.equipments, '|')) continue;
		if (!getline(ss, maxP_str, '|')) continue;
		if (!getline(ss, e.organiserEmail, '|')) continue;
		if (!getline(ss, fee_str, '|')) continue;
		if (!getline(ss, e.status, '|')) continue;
		getline(ss, participants_str, '\n');

		e.maxParticipants = stoi(maxP_str);
		e.fee = stod(fee_str);

		stringstream ps(participants_str);
		string email;
		while (getline(ps, email, ';')) {
			if (!email.empty()) e.participants.push_back(email);
		}

		events.push_back(e);

	}

	inFile.close();
	return events;
}

vector<EventFeedback> getFeedback(const string& filename) {
	EventFeedback fb;
	vector<EventFeedback> feedbacks;
	ifstream inFile(FEEDBACK_FILE);

	if (!inFile) {
		return feedbacks;
	}

	string line;
	while (getline(inFile, line)) {
		stringstream ss(line);
		EventFeedback fb;

		getline(ss, fb.eventName, '|');
		getline(ss, fb.userEmail, '|');
		getline(ss, fb.feedback, '\n');

		feedbacks.push_back(fb);
	}

	inFile.close();
	return feedbacks;

}

vector<User> getAllUsers(const string& filename) {
	vector<User> users;
	ifstream inFile(USER_FILE);

	if (!inFile) {
		return users;
	}

	User u;
	string line , limitStr;
	while (getline(inFile, line)) {
		stringstream ss(line);

		getline(ss, u.username, '|');
		getline(ss, u.password, '|');
		getline(ss, u.email, '|');
		getline(ss, limitStr);

		u.limit = (limitStr == "1");
		users.push_back(u);
	}
	inFile.close();
	return users;
}

void createUser(User& u , string role) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ofstream outFile;
	outFile.open(fileName.c_str(), ios::app);

	if (!outFile) {
		cout << "Error opening file for writing.\n";
		return;
	}

	outFile << u.username << "|" << u.password << "|" << u.email << "|" << u.limit << endl;
	outFile.close();
}

void updateUserFile(User& currentUser, string role) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ifstream inFile(fileName);
	ofstream tempFile("temp.txt");

	string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string uname, pass, mail, limStr;
        bool lim = false;

        getline(ss, uname, '|');
        getline(ss, pass, '|');
        getline(ss, mail, '|');
        getline(ss, limStr, '|');

        if (!limStr.empty())
            lim = (limStr == "1" || limStr == "true");

        if (uname == currentUser.username) {
            tempFile << currentUser.username << "|"
                     << currentUser.password << "|"
                     << currentUser.email << "|"
                     << (currentUser.limit ? 1 : 0) << "\n";
        } else {
            tempFile << uname << "|" << pass << "|" << mail << "|" << (lim ? 1 : 0) << "\n";
        }
    }

	inFile.close();
	tempFile.close();

	if (remove(fileName.c_str()) != 0) {
		cout << "Error deleting old file";
		return;
	}

	if (rename("temp.txt", fileName.c_str()) != 0) {
		cout << "Error renaming temp file";
		return;
	}
}

void updateEvents(const vector<Event>& events) {
	ofstream outFile(EVENT_FILE);
	if (!outFile) {
		cout << "Error opening events file.\n";
		return;
	}

	for (const Event& e : events) {
		string participants_str;
		for (size_t i = 0; i < e.participants.size(); i++) {
			participants_str += e.participants[i];
			if (i < e.participants.size() - 1) participants_str += ";";
		}

		outFile << e.eventName << "|"
			<< e.date << "|"
			<< e.time << "|"
			<< e.venue << "|"
			<< e.details << "|"
			<< e.equipments << "|"
			<< e.maxParticipants << "|"
			<< e.organiserEmail << "|"
			<< e.fee << "|"
			<< e.status << "|"
			<< participants_str << "\n";
	}

	outFile.close();
}

//Payment & Event Registration
void joinEvent(User& currentUser, vector<Event>& events) {
	int choice;
	string input;
	vector<int> availableEvents;

	clearScreen();
	cout << "Events Tab\n";
	cout << "-------------------------------------------------------\n";

	if (currentUser.limit) {
		cout << "You have already joined an event. Cannot join another one.\n";
		return;
	}

	if (events.empty()) {
		cout << "No Events Available Right Now\n";
		return;
	}

	for (int i = 0; i < events.size(); i++) {
		if (events[i].status == "Scheduled") {
			availableEvents.push_back(i);
			cout << availableEvents.size() << ". " << events[i].eventName << endl;
		}
	}

	if (availableEvents.empty()) {
		cout << "No Available Events Right Now\n";
		return;
	}

	while (true) {
		input = getNonEmptyInput("Select an Event to join: ");
		stringstream ss(input);

		if (ss >> choice && choice >= 1 && choice <= (int)availableEvents.size()) {
			break;
		}
		else {
			cout << "Invalid selection!\n";
		}
	}

	Event& selected = events[availableEvents[choice - 1]];

	if ((int)selected.participants.size() >= selected.maxParticipants) {
		cout << "Sorry, this event is already full.\n";
		return;
	}

	selected.participants.push_back(currentUser.email);

	cout << "You have successfully joined the event: " << selected.eventName << endl;
	cout << "Fee: RM " << fixed << setprecision(2) << selected.fee << endl;

	payment(selected.fee);

	currentUser.limit = true;
	updateUserFile(currentUser, "user");
	updateEvents(events);
	saveHistory(currentUser, "join", selected.eventName);
}

int payment(double total) {
	string input;
	string cardNumber, cvv;
	string bankChoice;
	string eWalletChoice;
	string phoneNum;
	double cash;
	int method;
	regex validPattern("^[1-4]$");


	do {
		cout << "\nSelect your payment method:\n";
		cout << "1. Credit Card\n";
		cout << "2. Online Banking\n";
		cout << "3. eWallet\n";
		cout << "4. Cash\n";
		cout << "Enter your choice (1-4): ";
		cin >> input;

		if (!regex_match(input, validPattern)) {
			cout << "Invalid input. Please enter a number from 1 to 4.\n";
			method = 0;
			continue;
		}

		method = stoi(input);
		switch (method) {
		case 1:
			cout << "Processing Credit Card payment of RM"
				<< fixed << setprecision(2) << total << "...\n";


			cout << "Enter your 16-digit card Number: ";

			while (!regex_match(cardNumber, regex("^[0-9]{16}$"))) {
				cout << " Invalid card number. Please enter a valid 16-digit number: ";
				cin >> cardNumber;
			}

			cout << "Enter your 3-digit CVV: ";
			cin >> cvv;

			while (!regex_match(cvv, regex("^[0-9]{3}$"))) {
				cout << " Invalid CVV. Please enter a valid 3-digit number: ";
				cin >> cvv;
			}

			cout << " Payment of RM" << fixed << setprecision(2) << total << " successful!\n";

			break;
		case 2:
			cout << "Redirecting to Online Banking portal...\n";

			cout << "\n--- Online Banking ---\n";
			cout << "Available Banks:\n";
			cout << "1. Maybank\n2. CIMB\n3. RHB\n4. Public Bank\n";
			cout << "Select your bank (1-4): ";
			cin >> bankChoice;

			while (!regex_match(bankChoice, regex("^[1-4]$"))) {
				cout << " Invalid choice. Please enter 1-4: ";
				cin >> bankChoice;
			}
			cout << " Payment of RM" << fixed << setprecision(2) << total << " completed via online banking.\n";

			break;
		case 3:
			cout << "Launching eWallet interface...\n";

			cout << "\n--- eWallet Payment ---\n";
			cout << "Available Providers:\n";
			cout << "1. Touch 'n Go\n2. GrabPay\n3. Boost\n";
			cout << "Select your provider (1-3): ";
			cin >> eWalletChoice;

			while (!regex_match(eWalletChoice, regex("^[1-3]$"))) {
				cout << " Invalid choice. Please enter 1-3: ";
				cin >> eWalletChoice;
			}

			cout << "Enter your phone number (10-11 digits): ";
			cin >> phoneNum;

			while (!regex_match(phoneNum, regex("^[0-9]{10,11}$"))) {
				cout << " Invalid phone number. Please enter again: ";
				cin >> phoneNum;
			}

			cout << " Payment of RM" << fixed << setprecision(2) << total << " completed via eWallet.\n";
			break;

		case 4:
			cout << "\n--- Cash Payment ---\n";

			do {
				cout << "Total to pay: RM" << fixed << setprecision(2) << total << "\n";
				cout << "Enter cash amount: RM";
				cin >> cash;

				if (cash < total) {
					cout << " Insufficient amount. Please pay at least RM" << total << ".\n";
				}
				else {
					cout << " Payment successful!\n";
					cout << "Your change: RM" << fixed << setprecision(2) << (cash - total) << "\n";
				}
			} while (cash < total);
			break;
		}
	} while (method == 0);

	cout << "Payment successful!\n";
	return method;
}

//Event Monitoring & Reporting
void monitorEventMenu() {
	cout << "\n\n===MONITOR EVENTS===\n\n";
	cout << "1. View Ongoing Event\n";
	cout << "2. Update Event Status\n";
	cout << "3. View Participant List\n";
	cout << "4. Add Event Log\n";
	cout << "5. View Event Log\n";
	cout << "6. Return\n";
}

void emchoice(User& currentUser, vector<Event>& events , EventLog& log , vector<EventFeedback> feedbacks , vector<User> users) {
	int organizer_choice;

	do {
		monitorEventMenu();
		cout << "Enter an option: ";
		cin >> organizer_choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (organizer_choice) {
		case 1:
			clearScreen();
			viewOngoingEvents(currentUser, events);
			break;
		case 2:
			clearScreen();
			viewOngoingEvents(currentUser, events);
			updateEventStatus(currentUser, users , events);
			break;
		case 3:
			clearScreen();
			viewParticipantList(currentUser, events, users);
			break;
		case 4:
			clearScreen();
			addEventLog(currentUser, events);
			break;
		case 5:
			clearScreen();
			viewEventLogs(currentUser);
			break;
		case 6:
			clearScreen();
			organiserMenu(currentUser, events, log, feedbacks , users);
			break;
		default:
			cout << "Invalid choice.\n";
		}


	} while (organizer_choice != 6);

}

void currentEventTemplate(const Event& e , User& currentUser) {
	string currentEvent[6][2] = { {"Current Event: " , e.eventName} ,
		{"Organized By: " , currentUser.username} ,
		{"Date: " , e.date} , {"Time: " , e.time} , 
		{"Max Participants: " , to_string(e.maxParticipants)} , 
		{"Event Status: " , e.status}
	};

	cout << "\n-------------------------------------------\n";
	cout << "           CURRENT ONGOING EVENT           \n";
	cout << "-------------------------------------------\n";

	for (int i = 0; i < 6; i++) {
		cout << setw(18) << left << currentEvent[i][0] << " : " << currentEvent[i][1] << "\n";
	}

	cout << "-------------------------------------------\n\n";
}

void viewOngoingEvents(User& currentUser, vector<Event>& events) {
	bool eventFound = false;

	for (Event& e : events) {
		if (e.organiserEmail == currentUser.email && (e.status == "Scheduled" || e.status == "Started")) {
			eventFound = true;
			currentEventTemplate(e , currentUser);
		}
	}

	if (!eventFound) {
		cout << "*** There is no ongoing event right now ***\n\n";
	}

}

void statusMenu() {
	cout << "===UPDATE EVENT STATUS===\n";
	cout << "1. STARTED\n";
	cout << "2. ENDED\n";
	cout << "3. CANCELLED\n";
}

void updateEventStatus(User& currentUser, vector<User>& users ,vector<Event>& events) {
	int status_option;
	int eventId = -1;


	for (int i = 0; i < (int)events.size(); ++i) {
		if (events[i].organiserEmail == currentUser.email && (events[i].status == "Scheduled" || events[i].status == "Started")) {
			eventId = i;
			break;
		}
	}

	if (eventId == -1) {
		cout << "You have no active scheduled event to update.\n";
		return;
	}

	Event& e = events[eventId];

	while (true) {
		statusMenu();
		status_option = getValidInt("Please Select Which Status you are changing to: ");

		if (status_option == 1) {
			cout << "Event Status Changed from " << e.status << " to Started\n";
			e.status = "Started";
			break;
		}
		else if (status_option == 2) {
			cout << "Event Status Changed from " << e.status << " to Ended\n";
			e.status = "Ended";
			resetLimit(currentUser, users, e);
			break;
		}
		else if (status_option == 3) {
			cout << "Event Status Changed from " << e.status << " to Cancelled\n";
			e.status = "Cancelled";
			resetLimit(currentUser, users, e);
			break;
		}
		else {
			cout << "Invalid Option. Please Enter Again \n";
		}
	}
	
	updateEvents(events);
}

void eventReport(User& currentUser, const vector<Event>& events) {
	int participantCount = 0;
	double revenue = 0.0;

	cout << fixed << setprecision(2) << showpoint;
	cout << "\n--- Event Report (Ended Events Only) ---\n";
	cout << left << setw(20) << "Event Name" << setw(15) << "Participants" << setw(10) << "Revenue" << "\n";
	cout << string(50, '-') << "\n";

	for (const Event& e : events) {
		if (e.organiserEmail == currentUser.email && e.status == "Ended") {
			participantCount = (int)e.participants.size();
			revenue = participantCount * e.fee;

			cout << left << setw(20) << e.eventName
				<< setw(15) << participantCount
				<< "RM " << revenue << "\n";
		}
	}

}

void addEventLog(User& currentUser, const vector<Event>& events ) {
	string note;
	EventLog log;
	bool eventFound = false;

	for (const Event& e : events) {
		if (currentUser.email == e.organiserEmail && e.status == "Started") {
			eventFound = true;
			cout << "Add an Event Log: ";
			getline(cin, note);

			if (note.empty()) {
				cout << "Log cannot be empty";
				return;
			}
			
			log.eventName = e.eventName;
			log.organiserEmail = currentUser.email;
			log.eventNote = note;

			createEventLog(log);

		}
		
	}

	if (!eventFound) {
		cout << "No Active Events to Add Log to";
	}
}

void createEventLog(const EventLog& log) {
	ofstream outFile(LOG_FILE, ios::app);
	if (!outFile) {
		cout << "Error opening log file!" << endl;
		return;
	}

	outFile << log.eventName << "|"
		<< log.organiserEmail << "|"
		<< log.eventNote << "\n";

	outFile.close();
	cout << "Event log added successfully!" << endl;
}

void viewEventLogs(User& currentUser){
	ifstream inFile(LOG_FILE);

	if (!inFile) {
		cout << "Cannot open log file!" << endl;
		return;
	}

	string line;
	bool found = false;

	cout << "\n--- Event Logs ---\n";
	while (getline(inFile, line)) {
		stringstream ss(line);
		EventLog log;

		getline(ss, log.eventName, '|');
		getline(ss, log.organiserEmail, '|');
		getline(ss, log.eventNote, '\n');  

		if (log.organiserEmail == currentUser.email) {
			found = true;
			cout << "Event: " << log.eventName << "\n"
				<< "Note : " << log.eventNote << "\n"
				<< "---------------------------\n";
		}
	}

	if (!found) {
		cout << "No logs found\n";
	}

	inFile.close();
}

void addFeedback(User& currentUser , const vector<Event>& events) {
	bool found = false;
	int choice;
	vector<int> joinedEvents;

	cout << "\n=== GIVE FEEDBACK ===\n";
	for (int i = 0; i < events.size() ; i++) {
		if (find(events[i].participants.begin(), events[i].participants.end(), currentUser.email)
			!= events[i].participants.end()
			&& events[i].status == "Ended") {
			cout << joinedEvents.size() + 1 << ". " << events[i].eventName << endl;
			joinedEvents.push_back(i);
			found = true;
		}
	}

	if (found == false) {
		cout << "You haven't joined any events or no current ended Events \n";
		return;
	}

	while (true) {
		cout << "Select an event to give feedback: ";
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice < 1 || choice > joinedEvents.size()) {
			cout << "Invalid choice.\n";
		}
		else {
			break;
		}
	}

	const Event& selectedEvent = events[joinedEvents[choice - 1]];
	createFeedback(currentUser , selectedEvent);
}

void createFeedback(const User& currentUser, const Event& e) {
	EventFeedback fb;
	fb.eventName = e.eventName;
	fb.userEmail = currentUser.email;

	cout << "Enter your feedback: ";
	getline(cin, fb.feedback);

	if (fb.feedback.empty()) {
		cout << "Feedback cannot be empty!\n";
		return;
	}

	ofstream outFile(FEEDBACK_FILE, ios::app);
	if (!outFile) {
		cout << "Error opening feedback file.\n";
		return;
	}

	outFile << fb.eventName << "|" << fb.userEmail << "|" << fb.feedback << "\n";
	outFile.close();

	cout << "Feedback submitted successfully!\n";
}

void viewFeedback(User& currentUser , const vector<Event> events) {
	vector<EventFeedback> feedbacks = getFeedback(FEEDBACK_FILE);
	bool found = false;

	for (const EventFeedback& fb : feedbacks) {
		for (const Event& e : events) {
			if (e.organiserEmail == currentUser.email && e.eventName == fb.eventName) {
				found = true;
				cout << "Event: " << fb.eventName << "\n"
					<< "From: " << fb.userEmail << "\n"
					<< "Feedback: " << fb.feedback << "\n"
					<< "-----------------------------\n";
			}
		}
	}

	if (!found) {
		cout << "No feedback found for your events yet.\n";
	}
}

void viewEventBillboard(const vector<Event>& events) {
	clearScreen();
	cout << "===EVENTS BILLBOARD===\n";
	cout << "-------------------------------------------------------\n";

	if (events.empty()) {
		cout << "No event available right now\n";
	}
	else {
		bool eventsFound = false;
		for (size_t i = 0; i < events.size(); i++) {
			if (events[i].status != "Cancelled") {
				eventsFound = true;
				cout << i + 1 << ".	" << events[i].eventName << " - " << events[i].date << " at " << events[i].time << "\n";
				cout << "	Venue: " << events[i].venue << "\n";
				cout << "	Fee: RM" << fixed << setprecision(2) << events[i].fee << "\n";
				cout << "	Status: " << events[i].status << "\n";
				cout << "	Available Slot: " << events[i].maxParticipants - events[i].participants.size() << "/" << events[i].maxParticipants << "\n";
				cout << "-------------------------------------------------------\n";
			}
		}

		if (!eventsFound) {
			cout << "No event available right now\n";
		}
	}
	cout << "\nEnter 0 to return to User Menu: ";
	string input;
	while (true) {
		getline(cin, input);
		if (input == "0") {
			break;
		}
		else {
			cout << "Please enter 0 to return to user menu: ";
		}
	}
}

void cancelEventRegistration(User& currentUser, vector<Event>& events) {
	clearScreen();
	cout << "===CANCEL EVENT REGISTRATION===\n";
	cout << "-------------------------------------------------------\n";

	if (!currentUser.limit) {
		cout << "You have not joined any event to cancel.\n";
		cout << "Please enter to return to user menu";
		string back;
		getline(cin, back);
		return;
	}

	bool found = false;
	int eventIndex = -1;

	for (int i = 0; i < events.size(); i++) {
		for (int j = 0; j < events[i].participants.size(); j++) {
			if (events[i].participants[j] == currentUser.email) {
				found = true;
				eventIndex = i;
				break;
			}
		}
		if (found) break;
	}
	if (!found) {
		cout << "Error: Event not found.\n";
		cout << "Please enter to return to user menu";
		string back;
		getline(cin, back);
		return;
	}
	cout << "You are registered for the event: \n";
	cout << "Event Name: " << events[eventIndex].eventName << endl;
	cout << "Date: " << events[eventIndex].date << endl;
	cout << "Time: " << events[eventIndex].time << endl;
	cout << "Venue: " << events[eventIndex].venue << endl;

	cout << "\nAre you sure you want to cancel your registration? (y/n): ";
	string confirmation;
	while (true) {
		confirmation = getNonEmptyInput("");
		if (confirmation == "y" || confirmation == "Y" || confirmation == "n" || confirmation == "N") {
			break;
		}
		else {
			cout << "Invalid input. Please enter 'y' or 'n': ";
		}
	}

	if (confirmation == "y" || confirmation == "Y") {
		for (size_t j = 0; j < events[eventIndex].participants.size(); j++) {
			if (events[eventIndex].participants[j] == currentUser.email) {
				events[eventIndex].participants.erase(events[eventIndex].participants.begin() + j);
				break;
			}
		}
		currentUser.limit = false;

		updateUserFile(currentUser, "user");
		updateEvents(events);

		saveHistory(currentUser, "cancel", events[eventIndex].eventName);

		cout << "Your registration has been cancelled successfully.\n";

	}
	else {
		cout << "Cancellation aborted. You are still registered for the event.\n";
	}

	cout << "Please enter to return to user menu";
	string back;
	getline(cin, back);
}

void saveHistory(const User& currentUser, const string& action, const string& eventName) {
	ofstream out(HISTORY_FILE, ios::app);
	if (!out) {
		cout << "Failed to open history file." << endl;
		return;
	}

	out << currentUser.email << "|" << action << "|" << eventName << "\n";
}

void viewHistory(const User& currentUser) {
	clearScreen();
	cout << "===VIEW HISTORY===\n";
	cout << "-------------------------------------------------------\n";

	ifstream in(HISTORY_FILE);
	if (!in) {
		cout << "Enter to return to user menu";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		string back;
		getline(cin, back);
		return;
	}

	string line;
	bool found = false;
	while (getline(in, line)) {
		stringstream ss(line);
		string email, action, eventName;

		getline(ss, email, '|');
		getline(ss, action, '|');
		getline(ss, eventName);

		if (email == currentUser.email) {
			found = true;
			if (action == "join") {
				cout << "- Joined: " << eventName << "\n";
			}
			else if (action == "cancel") {
				cout << "- Cancelled: " << eventName << "\n";
			}
			else {
				cout << "- " << action << ": " << eventName << "\n";
			}
		}
	}

	if (!found) {
		cout << "No history found for your account." << endl;
	}

	cout << "Enter to return to user menu";
	string back;
	getline(cin, back);
}

void viewParticipantList(User& currentUser , const vector<Event> events , const vector<User> users) {
	string email;
	string name;
	bool found = false;

	cout << "Participant Names\n";
	cout << "-------------------------------------------------\n";
	for (int i = 0; i < events.size(); i++ ) {
		if (events[i].organiserEmail == currentUser.email && (events[i].status == "Scheduled" || events[i].status == "Started")) {
			found = true;
			for (int j = 0; j < events[i].participants.size(); j++) {
				email = events[i].participants[j];
				for (const User& u : users) {
					if (u.email == email) {
						name = u.username;
					}
				}
				cout << j + 1 << ". " << name << " " << "(" << email << ")" << endl;
			}

		}
	}

	if (!found) {
		cout << "Currently , No active Event";
		return;
	}
}

void resetLimit(User& currentUser , vector<User>& users , Event& e)  {
	currentUser.limit = false;
	updateUserFile(currentUser, "organiser");

	for (string& email : e.participants) {
		for (User& u : users) {  
			if (u.email == email) {
				u.limit = false;
				updateUserFile(u, "user");  
			}
		}
	}
}

