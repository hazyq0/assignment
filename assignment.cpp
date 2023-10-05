#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <ctime>

using namespace std;

// A boolean function for condition 2 to calculate time difference
bool isTimeDifferenceValid(const std::string& timeStr1, const std::string& timeStr2) {
    // Define the format of the time strings
    string format = "%m/%d/%Y %H:%M:%S";

    // Parse the time strings into tm structures
    struct tm tm1 = {};
    struct tm tm2 = {};
    istringstream ss1(timeStr1);
    istringstream ss2(timeStr2);
    
    ss1 >> get_time(&tm1, format.c_str());
    ss2 >> get_time(&tm2, format.c_str());

    // Convert tm structures to time_t (seconds since epoch)
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    // Calculate the time difference in seconds
    double diffSeconds = difftime(time2, time1);

    // Check if the time difference is less than 10 hours and greater than 1 hour (3600 seconds)
    if (diffSeconds > 3600 && diffSeconds < 36000) {
        return true;
    } else {
        return false;
    }
}


int main() {
    // Open the CSV file
    ifstream file("Assignment_Timecard.csv");

    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    string line; // Reading line
    vector<vector<string>> data; // storing data as data[row][column];
    string currentEmployee, previousEmployee;
    string shiftEnded, shiftStarted;
    int consecutiveDays = 0;
    int i = 0;

    // Skip the header row if it exists
    if (getline(file, line)) {
        // Optionally, you can process the header row here
        // For example, you can extract column names.
    }
                                                            
    // Read and parse the CSV file line by line
    while (getline(file, line)) {
        istringstream lineStream(line);
        string cell;
        vector<string> row;

        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);

        // CHECKING FOR CONDITIONS

        // Condition - 1, where if an employee has worked for 7 days consecutively.
        currentEmployee = data[i][7]; // Assuming the employee name is in the 8th column (0-based index)
        if (currentEmployee == previousEmployee) {
            consecutiveDays++;
            if (consecutiveDays > 7) {
                cerr << currentEmployee << " has worked for more than 7 consecutive days." << endl;
                consecutiveDays = 1;
            }
        } else {
            consecutiveDays = 1; // Reset consecutive days count for a new employee
        }

        // Condition - 2, where an employee has less than 10 hours and greater than 1 hour of time between shifts
        if (i > 0 && currentEmployee == previousEmployee) {
            shiftStarted = data[i][2];
            shiftEnded = data[i-1][3];
            if (isTimeDifferenceValid(shiftEnded, shiftStarted)) {
                cerr << currentEmployee << " has less than 10 hours of time between shifts but greater than 1 hour." << endl;
            }
        }

        // Condition - 3, who has worked more than 14 hours in one shift
        string timecard = data[i][4];
        istringstream time(timecard);
        
        int hours, minutes;
        char delimeters;
        time >> hours >> delimeters >> minutes;
        
        int tsecs = (hours * 3600) + (minutes * 60); // Calculating total seconds
        
        if(tsecs > 50400) {
            cerr << currentEmployee << " has worked for more than 14 hours in a single shift." << endl;
        } 
        
        i++;
        previousEmployee = currentEmployee;
        
    }
    
    // Close the file
    file.close();
    
    return 0;
}
