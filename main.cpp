#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <algorithm> // For transform function
#include <cmath> // For mathematical functions

using namespace std;

// Function to convert a string to lowercase
string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// Function to calculate the distance between two geographical coordinates
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371; // Radius of the Earth in kilometers

    double lat1_rad = lat1 * M_PI / 180;
    double lon1_rad = lon1 * M_PI / 180;
    double lat2_rad = lat2 * M_PI / 180;
    double lon2_rad = lon2 * M_PI / 180;

    double dlon = lon2_rad - lon1_rad;
    double dlat = lat2_rad - lat1_rad;

    double a = pow(sin(dlat / 2), 2) + cos(lat1_rad) * cos(lat2_rad) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = R * c; // Distance in kilometers

    return distance;
}

// A boolean function for condition 4 to check if an employee is near the company's office
bool isNearOffice(double empLat, double empLon, double officeLat, double officeLon, double maxDistance) {
    double distance = calculateDistance(empLat, empLon, officeLat, officeLon);
    return distance <= maxDistance;
}

// A boolean function for condition 5 to check if the employee has taken a lunch break
bool hasTakenLunchBreak(const string& lunchStart, const string& lunchEnd) {
    // Assuming lunch break is taken if it starts between 11:00 AM and 2:00 PM
    string lunchStartLower = toLower(lunchStart);
    return lunchStartLower >= "11:00 am" && lunchStartLower <= "2:00 pm";
}

// A boolean function for condition 2 to calculate time difference
bool isTimeDifferenceValid(const string& timeStr1, const string& timeStr2) {
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

        // Additional conditions and processing

        // Condition - 4: Check if the employee is near the office location
        double empLatitude = stod(data[i][8]); // Assuming latitude is in the 9th column
        double empLongitude = stod(data[i][9]); // Assuming longitude is in the 10th column
        double companyOfficeLatitude = 37.7749; // Example latitude of the company office
        double companyOfficeLongitude = -122.4194; // Example longitude of the company office
        double maxDistanceToOffice = 5.0; // Maximum distance an employee can be from the office in kilometers

        if (!isNearOffice(empLatitude, empLongitude, companyOfficeLatitude, companyOfficeLongitude, maxDistanceToOffice)) {
            cerr << "Position ID: " << data[i][0] << " " << currentEmployee << " is not near the company's office." << endl;
        }

        // Condition - 5: Check if the employee has taken a lunch break
        string lunchStart = data[i][5]; // Assuming lunch start time is in the 6th column
        string lunchEnd = data[i][6]; // Assuming lunch end time is in the 7th column

        if (!hasTakenLunchBreak(lunchStart, lunchEnd)) {
            cerr << "Position ID: " << data[i][0] << " " << currentEmployee << " has not taken a lunch break." << endl;
        }

        // Condition - 2, where an employee has less than 10 hours and greater than 1 hour of time between shifts
        if (i > 0 && currentEmployee == previousEmployee) {
            shiftStarted = data[i][2]; // when the current shift is started 
            shiftEnded = data[i-1][3]; // when the last shift was ended
            
            if (isTimeDifferenceValid(shiftEnded, shiftStarted)) {
                cerr << "Position ID: " << data[i][0] << " " << currentEmployee << " has less than 10 hours of time between shifts but greater than 1 hour." << endl;
            }
        }

        // Condition - 1, where if an employee has worked for 7 days consecutively.
        currentEmployee = data[i][7]; // Assuming the employee name is in the 8th column (0-based index)
        if (currentEmployee == previousEmployee) {
            consecutiveDays++;
            if (consecutiveDays > 7) {
                cerr << "Position ID: " << data[i][0] << " " << currentEmployee << " has worked for more than 7 consecutive days." << endl;
                consecutiveDays = 1;
            }
        } else {
            consecutiveDays = 1; // Reset consecutive days count for a new employee
        }

        // Condition - 3, who has worked more than 14 hours in one shift
        string timecard = data[i][4];
        istringstream time(timecard);
        
        int hours, minutes;
        char delimeters;
        time >> hours >> delimeters >> minutes;
        
        int tsecs = (hours * 3600) + (minutes * 60); // Calculating total seconds
        
        if(tsecs > 50400) {
            cerr << "Position ID: " << data[i][0] << " " << currentEmployee << " has worked for more than 14 hours in a single shift." << endl;
        } 

        // Update counters and variables for next iteration
        i++;
        previousEmployee = currentEmployee;
    }

    // Close the file
    file.close();

    return 0;
}
