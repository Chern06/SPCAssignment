#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <iomanip>
#include <cstdio>
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

const string USER_FILE = "users.txt";
const string ORGANISER_FILE = "organisers.txt";
const string EVENT_FILE = "events.txt";

void clearScreen();
void mainMenu();
void userMenu(User& currentUser, vector<Event>& events);
void organiserMenu(User& currentUser, vector<Event>& events);
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
void emchoice(User& currentUser, vector<Event>& events);
void viewOngoingEvents(User& currentUser, vector<Event>& events);
void updateEventStatus(User& currentUser, vector<Event>& events);
void updateUserFile(User& currentUser, string role);
vector<Event> getAllEvents(const string& filename);
void updateEvents(const vector<Event>& events);
void statusMenu();
void eventReport(User& currentUser, const vector<Event>& events);
void viewEventsBillBoard(const vector<Event>& events);

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
		cin >> input;

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
	for (int i = 0; i < 50; i++) {
		cout << "\n";
	}
}

int main() {
	User currentUser;
	string choice;
	vector<Event> events = getAllEvents("events.txt");

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
					if (success) organiserMenu(currentUser, events);
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

//Event Marketing
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

void userMenu(User& currentUser, vector<Event>& events) {
	string choice;
	do {
		cout << "\n=== USER MENU ===\n";
		cout << "1. Join Event\n";
		cout << "2. Events Billboard\n";
		choice = getNonEmptyInput("Choice: ");

		if (choice == "1") {
			clearScreen();
			joinEvent(currentUser, events);
		}
		else if (choice == "2") {
			viewEventBillboard(events);
		}
		else {
			cout << "Invalid choice.\n";
		}
	} while (choice != "1");
}

void organiserMenu(User& currentUser, vector<Event>& events) {
	string choice;
	do {
		cout << "\n=== ORGANISER MENU ===\n";
		cout << "1. Register New Event\n";
		cout << "2. Monitor Event\n";
		cout << "3. Event Reports\n";
		cout << "4. Logout\n";
		choice = getNonEmptyInput("Choice: ");

		if (choice == "1") {
			clearScreen();
			createEvent(currentUser, events);
		}
		else if (choice == "2") {
			clearScreen();
			emchoice(currentUser, events);
		}
		else if (choice == "3") {
			clearScreen();
			eventReport(currentUser, events);
		}
		else if (choice == "4") {
			cout << "Logging out\n";
		}
		else {
			cout << "Invalid choice.\n";
		}
	} while (choice != "4");
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

	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ofstream outFile;
	outFile.open(fileName.c_str(), ios::app);

	if (!outFile) {
		cout << "Error opening file for writing.\n";
		return;
	}

	outFile << u.username << " " << u.password << " " << u.email << " " << u.limit << endl;
	outFile.close();

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

	string uname, pass, mail;
	bool eventLimit = false;
	bool found = false;

	while (inFile >> uname >> pass >> mail >> eventLimit) {
		if (mail == email && pass == password) {
			cout << role << " login successful!\n";
			found = true;


			currentUser.username = uname;
			currentUser.password = pass;
			currentUser.email = mail;
			currentUser.limit = eventLimit;
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
		"Field 1",
		"Field 2"
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
	} while (choice < 1 || choice > venues.size());
	e.venue = venues[choice - 1];

	cin.ignore();
	cout << "Enter Event Details: ";
	getline(cin, e.details);

	cout << "Enter Needed Equipments: ";
	getline(cin, e.equipments);

	cout << "Enter Amount of Participants: ";
	cin >> e.maxParticipants;

	cout << "Enter Event Fee (RM): ";
	cin >> e.fee;
	cin.ignore();

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

	string uname, pass, mail;
	inFile >> uname >> pass >> mail;
	while (!inFile.eof()) {
		if (mail == email) {
			inFile.close();
			return true;
		}
		inFile >> uname >> pass >> mail;
	}

	inFile.close();
	return false;
}

bool usernameExists(string username, string role) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ifstream inFile;
	inFile.open(fileName.c_str());

	if (!inFile) return false;

	string uname, pass, mail;
	inFile >> uname >> pass >> mail;
	while (!inFile.eof()) {
		if (uname == username) {
			inFile.close();
			return true;
		}
		inFile >> uname >> pass >> mail;
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
	ifstream inFile(filename);
	vector<Event> events;

	if (!inFile) {
		cout << "Cannot open file: " << filename << endl;
		return events;
	}

	string line;
	while (getline(inFile, line)) {
		stringstream ss(line);
		Event e;
		string maxP_str, fee_str, participants_str;

		if (!getline(ss, e.eventName, ',')) continue;
		if (!getline(ss, e.date, ',')) continue;
		if (!getline(ss, e.time, ',')) continue;
		if (!getline(ss, e.venue, ',')) continue;
		if (!getline(ss, e.details, ',')) continue;
		if (!getline(ss, e.equipments, ',')) continue;
		if (!getline(ss, maxP_str, ',')) continue;
		if (!getline(ss, e.organiserEmail, ',')) continue;
		if (!getline(ss, fee_str, ',')) continue;
		if (!getline(ss, e.status, ',')) continue;
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

void updateUserFile(User& currentUser, string role) {
	string fileName = (role == "organiser") ? ORGANISER_FILE : USER_FILE;
	ifstream inFile(fileName);
	ofstream tempFile("temp.txt");

	string uname, pass, mail;
	bool lim;
	while (inFile >> uname >> pass >> mail >> lim) {
		if (uname == currentUser.username) {
			tempFile << currentUser.username << " " << currentUser.password << " " << currentUser.email << " " << currentUser.limit << endl;
		}
		else {
			tempFile << uname << " " << pass << " " << mail << " " << lim << endl;
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

		outFile << e.eventName << ","
			<< e.date << ","
			<< e.time << ","
			<< e.venue << ","
			<< e.details << ","
			<< e.equipments << ","
			<< e.maxParticipants << ","
			<< e.organiserEmail << ","
			<< e.fee << ","
			<< e.status << ","
			<< participants_str << "\n";
	}

	outFile.close();
}




//Payment & Event Registration
void joinEvent(User& currentUser, vector<Event>& events) {

	clearScreen();
	cout << "Events Tab\n";
	cout << "-------------------------------------------------------\n";

	if (currentUser.limit) {
		cout << "You have already joined an event. Cannot join another one.\n";
		return;
	}

	if (events.empty()) {
		cout << "No Events Available Right Now";
		cout << "Please enter to return to user menu";
		string userInput;
		getline(cin, userInput);
		return;
	}
	else {
		for (int i = 0; i < events.size(); i++) {
			cout << i + 1 << ". " << events[i].eventName << endl;
		}
	}

	int choice;
	string input;

	while (true) {
		input = getNonEmptyInput("Select an Event to join: ");

		stringstream ss(input);
		if (ss >> choice && choice >= 1 && choice <= (int)events.size()) {
			break;
		}
		else {
			cout << "Invalid selection! Please enter a number between 1 and "
				<< events.size() << ".\n";
		}
	}

	Event& selected = events[choice - 1];

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

}

int payment(double total) {
	string input;
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
			break;
		case 2:
			cout << "Redirecting to Online Banking portal...\n";
			break;
		case 3:
			cout << "Launching eWallet interface...\n";
			break;
		case 4:
			cout << "Please pay cash at the registration counter.\n";
			break;
		}
	} while (method == 0);

	cout << "Payment successful!\n";
	return method;
}



//Event Monitoring & Reporting
void monitorEventMenu() {
	cout << "===MONITOR EVENTS===\n";
	cout << "1. View Ongoing Event\n";
	cout << "2. Update Event Status\n";
	cout << "3. Return\n";
}

void emchoice(User& currentUser, vector<Event>& events) {
	int organizer_choice;

	do {
		monitorEventMenu();
		cout << "Enter an option: ";
		cin >> organizer_choice;


		switch (organizer_choice) {
		case 1:
			viewOngoingEvents(currentUser, events);
			break;
		case 2:
			updateEventStatus(currentUser, events);
			break;
		case 3:
			clearScreen();
			organiserMenu(currentUser, events);
			break;
		default:
			cout << "Invalid choice.\n";
		}


	} while (organizer_choice != 3);


}

void viewOngoingEvents(User& currentUser, vector<Event>& events) {
	bool eventFound = false;

	for (Event& e : events) {
		if (e.organiserEmail == currentUser.email && (e.status == "Scheduled" || e.status == "Started")) {
			eventFound = true;
			cout << fixed << setprecision(2) << showpoint;
			cout << " " << setw(94) << setfill('-') << " " << endl << setfill(' ');
			cout << "|" << setw(52) << "CURRENT ONGOING EVENT" << setw(42) << "|" << endl;
			cout << "|" << setw(65) << e.eventName << setw(29) << "|" << endl;
			cout << "|" << setw(56) << "STATUS : " << e.status << setw(38) << "|" << endl;
			cout << "|" << setw(56) << "ORGANIZED BY : " << currentUser.username << setw(38) << "|" << endl;
			cout << " " << setw(94) << setfill('-') << " " << endl << setfill(' ');

			cout << "Date: " << e.date << endl << endl;
			cout << "Time: " << e.time << endl << endl;
			cout << "Max Participants: " << e.maxParticipants << endl << endl;
			cout << "Equipments: " << e.equipments << endl << endl;
			cout << "Details: " << e.details << endl << endl;
			cout << " " << setw(94) << setfill('-') << " " << endl << setfill(' ') << endl;
		}
	}

	if (!eventFound) {
		clearScreen();
		cout << "*** There is no ongoing event right now ***\n\n";
	}

}

void statusMenu() {
	cout << "===UPDATE EVENT STATUS===\n";
	cout << "1. STARTED\n";
	cout << "2. ENDED\n";
	cout << "3. CANCELLED\n";
}

void updateEventStatus(User& currentUser, vector<Event>& events) {
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

	statusMenu();
	cout << "Please Select Which Status are you changing to: ";
	cin >> status_option;

	if (status_option == 1) {
		cout << "Event Status Changed from " << e.status << " to Started";
		e.status = "Started";
	}
	else if (status_option == 2) {
		cout << "Event Status Changed from " << e.status << " to Ended";
		e.status = "Ended";
		currentUser.limit = false;
		updateUserFile(currentUser, "organiser");
	}
	else if (status_option == 3) {
		cout << "Event Status Changed from " << e.status << " to Cancelled";
		e.status = "Cancelled";
		currentUser.limit = false;
		updateUserFile(currentUser, "organiser");
	}
	else {
		cout << "Invalid Option";
		return;
	}

	updateEvents(events);
}

void eventReport(User& currentUser, const vector<Event>& events) {
	int participantCount = 0;
	double revenue = 0.0;

	cout << fixed << setprecision(2) << showpoint;
	cout << "\n--- Event Report (Ended Events Only) ---\n";
	cout << left << setw(20) << "Event Name"
		<< setw(15) << "Participants"
		<< setw(10) << "Revenue" << "\n";
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