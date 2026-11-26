# vehicle-parking-management-system


A C++ program to manage a parking lot. It records the vehicle number,
entry time and exit time, calculates the parking fee, and stores
the vehicle logs in a file.

## Team
- Joshan: Vehicle entry & data model
- Sandeep: Vehicle exit & fee calculation
- Balaram: File handling
- Sampath: Menu, reports & testing

## How to run
g++ -std=c++11 parking_system.cpp -o parking
./parking

## Features
- Vehicle entry and exit
- Fee: Rs. 20 per hour, rounded up, minimum 1 hour
- Logs saved in vehicle_logs.txt
- Search, parked list, all logs and revenue report
