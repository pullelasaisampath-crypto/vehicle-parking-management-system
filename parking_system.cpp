// ============================================================
//  VEHICLE PARKING MANAGEMENT SYSTEM  (C++)
//  Compile:  g++ -std=c++11 parking_system.cpp -o parking
//  Run:      ./parking
//
//  ROLE 1 (Joshan)  - Data Model & Vehicle Entry
//  ROLE 2 (Sandeep) - Vehicle Exit & Fee Calculation
//  ROLE 3 (Balaram) - File Handling
//  ROLE 4 (Sampath) - Menu, Reports, Search & Testing
// ============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

const int    MAX_SLOTS     = 50;               // parking capacity
const double RATE_PER_HOUR = 20.0;             // Rs. per hour
const string LOG_FILE      = "vehicle_logs.txt";

// ============================================================
// ROLE 1 (JOSHAN): DATA MODEL & VEHICLE ENTRY
// ============================================================
struct Vehicle {
    string number;      // vehicle number plate
    time_t entryTime;   // epoch seconds
    time_t exitTime;    // 0 = still parked
    double fee;         // 0 until vehicle exits
};

vector<Vehicle> logs;   // all records (parked + exited)

string formatTime(time_t t) {
    if (t == 0) return "-";
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

string cleanNumber(string s) {
    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

// returns index of a currently parked vehicle, or -1
int findParked(const string& number) {
    for (size_t i = 0; i < logs.size(); i++)
        if (logs[i].number == number && logs[i].exitTime == 0)
            return (int)i;
    return -1;
}

int countParked() {
    int c = 0;
    for (size_t i = 0; i < logs.size(); i++)
        if (logs[i].exitTime == 0) c++;
    return c;
}

bool vehicleEntry(const string& number) {
    if (number.empty()) {
        cout << "Vehicle number cannot be empty.\n";
        return false;
    }
    if (countParked() >= MAX_SLOTS) {
        cout << "Parking lot is FULL.\n";
        return false;
    }
    if (findParked(number) != -1) {
        cout << "This vehicle is already parked.\n";
        return false;
    }
    Vehicle v;
    v.number    = number;
    v.entryTime = time(0);
    v.exitTime  = 0;
    v.fee       = 0.0;
    logs.push_back(v);
    cout << "Vehicle " << number << " entered at " << formatTime(v.entryTime) << "\n";
    return true;
}

// ============================================================
// ROLE 2 (SANDEEP): VEHICLE EXIT & FEE CALCULATION
// ============================================================
// Rule: charged per hour, rounded UP, minimum 1 hour.
double calculateFee(time_t entry, time_t exitT) {
    double seconds = difftime(exitT, entry);
    int hours = (int)(seconds / 3600);
    if (fmod(seconds, 3600) > 0 || hours == 0) hours++;
    return hours * RATE_PER_HOUR;
}

bool vehicleExit(const string& number) {
    int idx = findParked(number);
    if (idx == -1) {
        cout << "No parked vehicle found with number " << number << ".\n";
        return false;
    }
    logs[idx].exitTime = time(0);
    logs[idx].fee      = calculateFee(logs[idx].entryTime, logs[idx].exitTime);

    cout << "\n------- PARKING RECEIPT -------\n";
    cout << "Vehicle : " << logs[idx].number << "\n";
    cout << "Entry   : " << formatTime(logs[idx].entryTime) << "\n";
    cout << "Exit    : " << formatTime(logs[idx].exitTime) << "\n";
    cout << "Rate    : Rs. " << RATE_PER_HOUR << " / hour\n";
    cout << fixed << setprecision(2);
    cout << "Fee     : Rs. " << logs[idx].fee << "\n";
    cout << "-------------------------------\n";
    return true;
}

// ============================================================
// ROLE 3 (BALARAM): FILE HANDLING
// File format (one record per line):  number|entryTime|exitTime|fee
// ============================================================
bool saveLogs() {
    ofstream out(LOG_FILE.c_str());
    if (!out) {
        cout << "Error: cannot open " << LOG_FILE << " for writing.\n";
        return false;
    }
    for (size_t i = 0; i < logs.size(); i++) {
        out << logs[i].number << "|"
            << (long long)logs[i].entryTime << "|"
            << (long long)logs[i].exitTime  << "|"
            << logs[i].fee << "\n";
    }
    return true;
}

bool loadLogs() {
    ifstream in(LOG_FILE.c_str());
    if (!in) return false;              // first run: no file yet
    logs.clear();
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string num, en, ex, fe;
        if (!getline(ss, num, '|') || !getline(ss, en, '|') ||
            !getline(ss, ex, '|')  || !getline(ss, fe, '|'))
            continue;                   // skip damaged line
        try {
            Vehicle v;
            v.number    = num;
            v.entryTime = (time_t)stoll(en);
            v.exitTime  = (time_t)stoll(ex);
            v.fee       = stod(fe);
            logs.push_back(v);
        } catch (...) {
            continue;                   // skip damaged line
        }
    }
    return true;
}

// ============================================================
// ROLE 4 (SAMPATH): MENU, REPORTS, SEARCH & TESTING
// ============================================================
void printHeader() {
    cout << left << setw(14) << "Vehicle No"
         << setw(21) << "Entry Time"
         << setw(21) << "Exit Time"
         << "Fee (Rs)\n";
    cout << string(66, '-') << "\n";
}

void printRow(const Vehicle& v) {
    cout << left << setw(14) << v.number
         << setw(21) << formatTime(v.entryTime)
         << setw(21) << formatTime(v.exitTime)
         << fixed << setprecision(2) << v.fee << "\n";
}

void showParked() {
    cout << "\nCurrently parked (" << countParked() << "/" << MAX_SLOTS << "):\n";
    printHeader();
    bool any = false;
    for (size_t i = 0; i < logs.size(); i++)
        if (logs[i].exitTime == 0) { printRow(logs[i]); any = true; }
    if (!any) cout << "(none)\n";
}

void showAllLogs() {
    cout << "\nAll vehicle logs:\n";
    printHeader();
    if (logs.empty()) cout << "(no records)\n";
    for (size_t i = 0; i < logs.size(); i++) printRow(logs[i]);
}

void searchVehicle(const string& number) {
    bool found = false;
    printHeader();
    for (size_t i = 0; i < logs.size(); i++)
        if (logs[i].number == number) { printRow(logs[i]); found = true; }
    if (!found) cout << "No records for " << number << ".\n";
}

void showRevenue() {
    double total = 0;
    int done = 0;
    for (size_t i = 0; i < logs.size(); i++)
        if (logs[i].exitTime != 0) { total += logs[i].fee; done++; }
    cout << fixed << setprecision(2);
    cout << "Completed visits : " << done << "\n";
    cout << "Total revenue    : Rs. " << total << "\n";
}

string askNumber() {
    string s;
    cout << "Enter vehicle number: ";
    getline(cin, s);
    return cleanNumber(s);
}

int main() {
    if (loadLogs()) cout << "Previous logs loaded (" << logs.size() << " records).\n";

    int choice = 0;
    do {
        cout << "\n===== VEHICLE PARKING MANAGEMENT SYSTEM =====\n"
             << "1. Vehicle Entry\n"
             << "2. Vehicle Exit (calculate fee)\n"
             << "3. Show Parked Vehicles\n"
             << "4. Show All Logs\n"
             << "5. Search Vehicle\n"
             << "6. Total Revenue\n"
             << "0. Save & Exit\n"
             << "Choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            choice = -1;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: if (vehicleEntry(askNumber())) saveLogs(); break;
            case 2: if (vehicleExit(askNumber()))  saveLogs(); break;
            case 3: showParked();  break;
            case 4: showAllLogs(); break;
            case 5: searchVehicle(askNumber()); break;
            case 6: showRevenue(); break;
            case 0: saveLogs(); cout << "Data saved. Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}