#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <regex>
#include <conio.h>

using namespace std;

const string USER_FILE = "users.txt";
const string PACKAGE_FILE = "Services.txt";
const string RATE_FILE = "rates.txt";
const string BOOKING_FILE = "bookings.txt";

// ============================================================================
// DESIGN AND UTILITY FUNCTIONS
// ============================================================================

void printHeader(const string& title) {
    cout << "\n";
    cout << "============================================================\n";
    cout << "  " << title << "\n";
    cout << "============================================================\n";
}

void printSubHeader(const string& subtitle) {
    cout << "\n" << subtitle << "\n";
    cout << "------------------------------------------------------------\n";
}

void printSuccess(const string& message) {
    cout << "\n " << message << "\n";
}

void printError(const string& message) {
    cout << "\n " << message << "\n";
}

void printInfo(const string& message) {
    cout << "\n[i] " << message << "\n";
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...   ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

string toLowerStr(const string &s) {
    string out = s;
    transform(out. begin(), out.end(), out. begin(), ::tolower);
    return out;
}

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool looksLikeHeaderRow(const vector<string>& row) {
    if (row.empty()) return false;
    string first = toLowerStr(trim(row[0]));
    vector<string> headerNames = {"email", "password", "name", "type", "vehiclenumber", "id", "price", "servicename", "service", "rate", "customeremail", "servicetype", "totalamount", "date", "timeslot", "time", "status"};
    for (const auto& h : headerNames) {
        if (first == h) return true;
    }
    return false;
}

vector<vector<string>> readTXT(const string& filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            vector<string> row;
            stringstream ss(line);
            string cell;
            while (getline(ss, cell, ',')) {
                row.push_back(trim(cell));
            }
            if (!row.empty()) data.push_back(row);
        }
        file.close();
    }
    if (!data.empty() && looksLikeHeaderRow(data.front())) {
        data.erase(data.begin());
    }
    return data;
}

void writeTXT(const string& filename, const vector<vector<string>>& data) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); i++) {
                file << row[i];
                if (i < row. size() - 1) file << ",";
            }
            file << endl;
        }
        file. close();
    }
}

// ============================================================================
// VALIDATION FUNCTIONS (ENHANCED)
// ============================================================================

bool isValidEmail(const string& email) {
    string trimmedEmail = trim(email);
    if (trimmedEmail.empty()) {
        printError("Email cannot be empty!");
        return false;
    }
    static const regex pattern(R"(^[A-Za-z0-9. _%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    if (!regex_match(trimmedEmail, pattern)) {
        printError("Invalid email format. Example: user@example.com");
        return false;
    }
    return true;
}

bool isValidPassword(const string& pwd) {
    if (pwd.empty()) {
        printError("Password cannot be empty!");
        return false;
    }
    if (pwd.size() < 6) {
        printError("Password must be at least 6 characters long!");
        return false;
    }
    bool hasDigit = false, hasLower = false, hasUpper = false, hasSpecial = false;
    for (char c : pwd) {
        if (isdigit((unsigned char)c)) hasDigit = true;
        else if (islower((unsigned char)c)) hasLower = true;
        else if (isupper((unsigned char)c)) hasUpper = true;
        else if (ispunct((unsigned char)c)) hasSpecial = true;
    }
    if (!hasDigit) {
        printError("Password must contain at least one digit (0-9)!");
        return false;
    }
    if (!hasLower) {
        printError("Password must contain at least one lowercase letter (a-z)!");
        return false;
    }
    if (!hasUpper) {
        printError("Password must contain at least one uppercase letter (A-Z)!");
        return false;
    }
    if (!hasSpecial) {
        printError("Password must contain at least one special character (! @#$%^&*)!");
        return false;
    }
    return true;
}

bool isValidName(const string& name) {
    string s = trim(name);
    if (s.empty()) {
        printError("Name cannot be empty!");
        return false;
    }
    if (s.size() < 2) {
        printError("Name must be at least 2 characters long!");
        return false;
    }
    bool hasAlpha = false;
    for (char c : s) {
        if (!(isalpha((unsigned char)c) || c == ' ' || c == '-' || c == '\'')) {
            printError("Name can only contain letters, spaces, hyphens, and apostrophes!");
            return false;
        }
        if (isalpha((unsigned char)c)) hasAlpha = true;
    }
    if (!hasAlpha) {
        printError("Name must contain at least one letter!");
        return false;
    }
    return true;
}

bool isValidDate(const string& date) {
    if (date.empty()) {
        printError("Date cannot be empty!");
        return false;
    }
    static const regex pattern(R"(^(19|20)\d\d-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
    if (! regex_match(date, pattern)) {
        printError("Invalid date format. Use YYYY-MM-DD (e.g., 2024-12-25)!");
        return false;
    }
    int y, m, d;
    char dash1, dash2;
    stringstream ss(date);
    ss >> y >> dash1 >> m >> dash2 >> d;
    if (ss.fail()) {
        printError("Invalid date values!");
        return false;
    }
    int mdays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    bool leap = ((y%4==0 && y%100!=0) || (y%400==0));
    if (leap) mdays[2] = 29;
    if (m < 1 || m > 12) {
        printError("Month must be between 01 and 12!");
        return false;
    }
    if (d < 1 || d > mdays[m]) {
        printError("Day is invalid for the given month!");
        return false;
    }
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int currentYear = 1900 + ltm->tm_year;
    int currentMonth = 1 + ltm->tm_mon;
    int currentDay = ltm->tm_mday;
    if (y < currentYear || (y == currentYear && m < currentMonth) || (y == currentYear && m == currentMonth && d < currentDay)) {
        printError("Cannot book for past dates!");
        return false;
    }
    return true;
}

bool isValidVehicleNumber(const string& veh) {
    string v = trim(veh);
    if (v.empty()) {
        printError("Vehicle number cannot be empty!");
        return false;
    }
    if (v. length() < 6) {
        printError("Vehicle number must be at least 6 characters long!");
        return false;
    }
    if (v.length() > 15) {
        printError("Vehicle number is too long (max 15 characters)!");
        return false;
    }
    for (char c : v) {
        if (!(isalnum((unsigned char)c))) {
            printError("Vehicle number can only contain letters and numbers!");
            return false;
        }
    }
    return true;
}

bool isValidTime12HourSlot(const string& timeStr) {
    if (timeStr.empty()) {
        printError("Time slot cannot be empty!");
        return false;
    }
    static const regex pattern(R"(^(0?[1-9]|1[0-2]):[0-5]\d\s?(am|pm|AM|PM)$)");
    string trimmedTime = trim(timeStr);
    if (!regex_match(trimmedTime, pattern)) {
        printError("Invalid time format. Use 12-hour format (e.g., 09:30 AM, 02:30 PM)!");
        return false;
    }

    int hour, minute;
    string ampm;
    stringstream ss(trimmedTime);
    char colon;
    ss >> hour >> colon >> minute >> ws;
    string remaining;
    ss >> remaining;
    ampm = toLowerStr(trim(remaining));
    
    if (ampm == "pm" && hour != 12) {
        hour += 12;
    }
    if (ampm == "am" && hour == 12) {
        hour = 0;
    }
    
    int totalMinutes = hour * 60 + minute;
    int startMinutes = 9 * 60;
    int endMinutes = 19 * 60 + 30;
    
    if (totalMinutes < startMinutes || totalMinutes > endMinutes) {
        printError("Working hours are 09:00 AM to 07:30 PM only!");
        return false;
    }
    return true;
}

bool isValidPrice(int price) {
    if (price <= 0) {
        printError("Price must be greater than 0!");
        return false;
    }
    if (price > 1000000) {
        printError("Price seems too high. Please enter a valid price!");
        return false;
    }
    return true;
}

string getHiddenPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password. empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else if (ch >= 32 && ch <= 126) {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct User {
    string email;
    string password;
    string name;
    string type;
    string vehicleNumber;

    vector<string> toTXT() const {
        return {email, password, name, type, vehicleNumber};
    }

    static User fromTXT(const vector<string>& data) {
        User user;
        if (data.size() >= 5) {
            user.email = data[0];
            user. password = data[1];
            user.name = data[2];
            user.type = data[3];
            user.vehicleNumber = data[4];
        }
        return user;
    }
};

struct ServicePackage {
    int id;
    string name;
    int price;

    vector<string> toTXT() const {
        return {to_string(id), name, to_string(price)};
    }

    static ServicePackage fromTXT(const vector<string>& data) {
        ServicePackage pkg;
        try {
            if (data.size() >= 3) {
                pkg.id = stoi(data[0]);
                pkg.name = data[1];
                pkg.price = stoi(data[2]);
            }
        } catch (...) {
            pkg.id = 0;
            pkg.name = "";
            pkg.price = 0;
        }
        return pkg;
    }
};

struct ServiceRate {
    int id;
    string serviceName;
    int rate;

    vector<string> toTXT() const {
        return {to_string(id), serviceName, to_string(rate)};
    }

    static ServiceRate fromTXT(const vector<string>& data) {
        ServiceRate sr;
        try {
            if (data.size() >= 3) {
                sr.id = stoi(data[0]);
                sr.serviceName = data[1];
                sr.rate = stoi(data[2]);
            }
        } catch (... ) {
            sr.id = 0;
            sr.serviceName = "";
            sr.rate = 0;
        }
        return sr;
    }
};

struct Booking {
    int id;
    string customerEmail;
    string serviceType;
    int totalAmount;
    string date;
    string timeSlot;
    string status;

    vector<string> toTXT() const {
        return {to_string(id), customerEmail, serviceType, to_string(totalAmount), date, timeSlot, status};
    }

    static Booking fromTXT(const vector<string>& data) {
        Booking booking;
        try {
            if (data.size() >= 7) {
                booking.id = stoi(data[0]);
                booking.customerEmail = data[1];
                booking.serviceType = data[2];
                booking.totalAmount = stoi(data[3]);
                booking.date = data[4];
                booking.timeSlot = data[5];
                booking.status = data[6];
            }
        } catch (...) {
            booking.id = 0;
            booking.customerEmail = "";
            booking.serviceType = "";
            booking.totalAmount = 0;
            booking.date = "";
            booking.timeSlot = "";
            booking.status = "";
        }
        return booking;
    }
};

// ============================================================================
// CAR SERVICE SYSTEM CLASS
// ============================================================================

class CarServiceSystem {
private:
    vector<User> users;
    vector<ServicePackage> Services;
    vector<ServiceRate> rates;
    vector<Booking> bookings;
    User currentUser;
    bool isLoggedIn;

    void loadUsers() {
        users.clear();
        vector<vector<string>> data = readTXT(USER_FILE);
        for (const auto& row : data) {
            users.push_back(User::fromTXT(row));
        }
    }

    void loadPackages() {
        Services.clear();
        vector<vector<string>> data = readTXT(PACKAGE_FILE);
        for (const auto& row : data) {
            Services.push_back(ServicePackage::fromTXT(row));
        }
    }

    void loadRates() {
        rates.clear();
        vector<vector<string>> data = readTXT(RATE_FILE);
        for (const auto& row : data) {
            rates.push_back(ServiceRate::fromTXT(row));
        }
    }

    void loadBookings() {
        bookings.clear();
        vector<vector<string>> data = readTXT(BOOKING_FILE);
        for (const auto& row : data) {
            bookings.push_back(Booking::fromTXT(row));
        }
    }

    void saveUsers() {
        vector<vector<string>> data;
        data.push_back({"email","password","name","type","vehiclenumber"});
        for (const auto& u : users) {
            data. push_back(u.toTXT());
        }
        writeTXT(USER_FILE, data);
    }

    void savePackages() {
        vector<vector<string>> data;
        data.push_back({"id","name","price"});
        for (const auto& p : Services) {
            data.push_back(p.toTXT());
        }
        writeTXT(PACKAGE_FILE, data);
    }

    void saveRates() {
        vector<vector<string>> data;
        data.push_back({"id","servicename","rate"});
        for (const auto& r : rates) {
            data.push_back(r.toTXT());
        }
        writeTXT(RATE_FILE, data);
    }

    void saveBookings() {
        vector<vector<string>> data;
        data.push_back({"id","customeremail","servicetype","totalamount","date","timeslot","status"});
        for (const auto& b : bookings) {
            data. push_back(b.toTXT());
        }
        writeTXT(BOOKING_FILE, data);
    }

    void initializeDefaultData() {
        users.clear();
        Services.clear();
        rates.clear();
        bookings.clear();

        users.push_back({"admin@carser.com", "Admin@123", "Admin User", "admin", "ADMIN0001"});

        Services.push_back({1, "Engine Service", 5000});
        Services.push_back({2, "Brake & Basic Service", 7000});
        Services.push_back({3, "Full Car Service", 10000});
        Services. push_back({4, "AC Service", 4000});
        Services.push_back({5, "Transmission Service", 6000});

        rates.push_back({1, "Oil Change", 1000});
        rates.push_back({2, "Chain Set Change", 2000});
        rates.push_back({3, "Brake Pad Replacement", 1500});
        rates.push_back({4, "Tire Rotation", 800});
        rates.push_back({5, "Battery Replacement", 2500});

        saveUsers();
        savePackages();
        saveRates();
        saveBookings();
    }

    string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
        return string(buffer);
    }

    int getNextBookingId() const {
        int maxId = 0;
        for (const auto& b : bookings) {
            if (b.id > maxId) maxId = b.id;
        }
        return maxId + 1;
    }

    bool isSlotAvailable(const string& date, const string& time) const {
        for (const auto& b : bookings) {
            if (b. date == date && toLowerStr(b.timeSlot) == toLowerStr(time) && toLowerStr(b.status) != "cancelled") {
                return false;
            }
        }
        return true;
    }

public:
    CarServiceSystem() : isLoggedIn(false) {
        loadUsers();
        loadPackages();
        loadRates();
        loadBookings();

        if (users.empty() || Services.empty() || rates. empty()) {
            initializeDefaultData();
            loadUsers();
            loadPackages();
            loadRates();
            loadBookings();
        }
    }

    void run() {
        while (true) {
            clearScreen();
            printHeader("CAR SERVICE MANAGEMENT SYSTEM");
            cout << "\n  Welcome to our Car Service Management System\n\n";
            cout << "  1. Admin Login\n";
            cout << "  2. Customer Registration\n";
            cout << "  3. Customer Login\n";
            cout << "  4. Forgot Password (Customer)\n";
            cout << "  5. Exit\n";
            cout << "\n============================================================\n";
            cout << "Enter your choice (1-5): ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid input! Please enter a number between 1-5.");
                pressEnterToContinue();
                continue;
            }
            cin.ignore();

            switch (choice) {
                case 1:
                    adminLogin();
                    break;
                case 2:
                    customerRegistration();
                    break;
                case 3:
                    customerLogin();
                    break;
                case 4:
                    forgotPasswordMenu();
                    break;
                case 5:
                    clearScreen();
                    cout << "\n============================================================\n";
                    cout << " Thank you for using our Car Service Management System!\n";
                    cout << " Have a great day!\n";
                    cout << "============================================================\n\n";
                    return;
                default:
                    printError("Invalid choice! Please enter a number between 1-5.");
                    pressEnterToContinue();
            }
        }
    }

    void adminLogin() {
        clearScreen();
        printHeader("ADMIN LOGIN");

        string email, password;
        cout << "Email: ";
        getline(cin, email);

        if (!isValidEmail(email)) {
            pressEnterToContinue();
            return;
        }

        cout << "Password: ";
        password = getHiddenPassword();

        for (const auto& u : users) {
            if (u.type == "admin" && toLowerStr(u.email) == toLowerStr(email) && u.password == password) {
                currentUser = u;
                isLoggedIn = true;
                clearScreen();
                printSuccess("Login successful!");
                cout << "Welcome back, " << u.name << "!\n";
                pressEnterToContinue();
                adminMenu();
                return;
            }
        }

        printError("Invalid admin credentials!");
        printInfo("Default Email: admin@carser.com");
        printInfo("Default Password: Admin@123");
        pressEnterToContinue();
    }

    void customerRegistration() {
        clearScreen();
        printHeader("CUSTOMER REGISTRATION");

        string email, password, name, vehicleNumber;
        
        while (true) {
            cout << "Email: ";
            getline(cin, email);
            if (!isValidEmail(email)) {
                continue;
            }
            bool exists = false;
            for (const auto& u : users) {
                if (toLowerStr(u.email) == toLowerStr(email)) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                printError("Email already registered! Please use a different email.");
                pressEnterToContinue();
                return;
            }
            break;
        }

        while (true) {
            cout << "\nPassword requirements:\n";
            cout << " - Minimum 6 characters\n";
            cout << " - At least one uppercase letter (A-Z)\n";
            cout << " - At least one lowercase letter (a-z)\n";
            cout << " - At least one digit (0-9)\n";
            cout << " - At least one special character (! @#$%^&*)\n";
            cout << "\nPassword: ";
            password = getHiddenPassword();
            if (! isValidPassword(password)) {
                continue;
            }
            break;
        }

        while (true) {
            cout << "\nFull Name: ";
            getline(cin, name);
            if (!isValidName(name)) {
                continue;
            }
            break;
        }

        while (true) {
            cout << "\nVehicle Number (e.g., GJ01AB1234): ";
            getline(cin, vehicleNumber);
            if (!isValidVehicleNumber(vehicleNumber)) {
                continue;
            }
            break;
        }

        users.push_back({email, password, name, "customer", vehicleNumber});
        saveUsers();

        clearScreen();
        printSuccess("Registration successful!");
        printInfo("Your account has been created. You can now login with your email and password.");
        pressEnterToContinue();
    }

    void customerLogin() {
        clearScreen();
        printHeader("CUSTOMER LOGIN");

        string email, password;
        cout << "Email: ";
        getline(cin, email);
        cout << "Password: ";
        password = getHiddenPassword();

        for (const auto& user : users) {
            if (toLowerStr(user.email) == toLowerStr(email) && user.password == password && user.type == "customer") {
                currentUser = user;
                isLoggedIn = true;
                clearScreen();
                printSuccess("Login successful!");
                cout << "Welcome, " << user.name << "!\n";
                pressEnterToContinue();
                customerMenu();
                return;
            }
        }

        printError("Invalid login credentials!");
        pressEnterToContinue();
    }

    void forgotPasswordMenu() {
        clearScreen();
        printHeader("CUSTOMER PASSWORD RESET");

        string vehicleNumber;
        cout << "Enter your registered vehicle number: ";
        getline(cin, vehicleNumber);
        vehicleNumber = trim(vehicleNumber);

        if (!isValidVehicleNumber(vehicleNumber)) {
            pressEnterToContinue();
            return;
        }

        vector<User*> matchingUsers;
        for (auto& u : users) {
            if (toLowerStr(u.vehicleNumber) == toLowerStr(vehicleNumber) && u.type == "customer") {
                matchingUsers.push_back(&u);
            }
        }

        if (matchingUsers.empty()) {
            printError("No customer account found with this vehicle number.");
            pressEnterToContinue();
            return;
        }

        User* selectedUser = nullptr;
        if (matchingUsers.size() == 1) {
            selectedUser = matchingUsers[0];
            printInfo("Account found: " + selectedUser->email);
        } else {
            cout << "\nMultiple accounts found:\n";
            for (size_t i = 0; i < matchingUsers.size(); i++) {
                cout << "  " << i + 1 << ". " << matchingUsers[i]->email << "\n";
            }
            cout << "\nSelect account number (1-" << matchingUsers. size() << "): ";
            int selection;
            if (!(cin >> selection) || selection < 1 || selection > (int)matchingUsers.size()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid selection!");
                pressEnterToContinue();
                return;
            }
            cin.ignore();
            selectedUser = matchingUsers[selection - 1];
        }

        resetPassword(selectedUser);
    }

    void resetPassword(User* user) {
        string newPassword, confirmPassword;

        while (true) {
            cout << "\nPassword requirements:\n";
            cout << " - Minimum 6 characters\n";
            cout << " - At least one uppercase letter\n";
            cout << " - At least one lowercase letter\n";
            cout << " - At least one digit\n";
            cout << " - At least one special character\n";
            cout << "\nEnter new password: ";
            newPassword = getHiddenPassword();
            cout << "Confirm new password: ";
            confirmPassword = getHiddenPassword();

            if (newPassword != confirmPassword) {
                printError("Passwords do not match!");
                continue;
            }

            if (!isValidPassword(newPassword)) {
                continue;
            }

            break;
        }

        user->password = newPassword;
        saveUsers();
        clearScreen();
        printSuccess("Password reset successfully!");
        printInfo("You can now login with your new password.");
        pressEnterToContinue();
    }

    void adminMenu() {
        while (isLoggedIn) {
            clearScreen();
            printHeader("ADMIN DASHBOARD");
            cout << "Welcome, " << currentUser.name << "!\n\n";
            cout << "  1. Manage Services\n";
            cout << "  2. Manage Service Rates\n";
            cout << "  3. Generate Invoice / Mark Paid\n";
            cout << "  4. View Customer Activities\n";
            cout << "  5. Logout\n";
            cout << "\n============================================================\n";
            cout << "Enter your choice (1-5): ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin. ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid input! Please enter a number.");
                pressEnterToContinue();
                continue;
            }
            cin.ignore();

            switch (choice) {
                case 1:
                    managePackages();
                    break;
                case 2:
                    manageRates();
                    break;
                case 3:
                    generateInvoice();
                    break;
                case 4:
                    viewCustomerActivities();
                    break;
                case 5:
                    isLoggedIn = false;
                    clearScreen();
                    printSuccess("Logged out successfully!");
                    pressEnterToContinue();
                    return;
                default:
                    printError("Invalid choice! Please try again.");
                    pressEnterToContinue();
            }
        }
    }

    void managePackages() {
        while (true) {
            clearScreen();
            printHeader("MANAGE SERVICES");
            cout << "\n  1. View All SERVICES\n";
            cout << "  2. Add New Service\n";
            cout << "  3. Update Service\n";
            cout << "  4. Delete Service\n";
            cout << "  5. Back to Main Menu\n";
            cout << "\n============================================================\n";
            cout << "Enter your choice (1-5): ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin. ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid input! Please enter a number.");
                pressEnterToContinue();
                continue;
            }
            cin. ignore();

            switch (choice) {
                case 1:
                    viewAllPackages();
                    break;
                case 2:
                    addPackage();
                    break;
                case 3:
                    updatePackage();
                    break;
                case 4:
                    deletePackage();
                    break;
                case 5:
                    return;
                default:
                    printError("Invalid choice! Please try again.");
                    pressEnterToContinue();
            }
        }
    }

    void viewAllPackages() {
        clearScreen();
        printHeader("ALL SERVICES");

        if (Services.empty()) {
            printInfo("No Services available!");
        } else {
            cout << setw(5) << "ID" << setw(30) << "Name" << setw(15) << "Price (Rs. )" << "\n";
            cout << "------------------------------------------------------------\n";
            for (const auto& pkg : Services) {
                cout << setw(5) << pkg. id
                     << setw(30) << pkg.name
                     << setw(15) << pkg.price << "\n";
            }
        }
        pressEnterToContinue();
    }

    void addPackage() {
        clearScreen();
        printHeader("ADD NEW SERVICE PACKAGE");

        string name;
        int price = 0;

        cout << "Package Name: ";
        getline(cin, name);
        name = trim(name);
        if (name.empty()) {
            printError("Package name cannot be empty!");
            pressEnterToContinue();
            return;
        }
        
        cout << "Price (Rs.): ";
        while (!(cin >> price) || ! isValidPrice(price)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();

        int newId = Services.empty() ? 1 : (Services.back().id + 1);
        Services.push_back({newId, name, price});
        savePackages();

        clearScreen();
        printSuccess("Package added successfully!");
        cout << "Package ID: " << newId << "\n";
        cout << "Package Name: " << name << "\n";
        cout << "Price: Rs.  " << price << "\n";
        pressEnterToContinue();
    }

    void updatePackage() {
        clearScreen();
        printHeader("UPDATE SERVICE PACKAGE");

        viewAllPackages();
        if (Services.empty()) return;

        int id;
        cout << "Enter Package ID to update: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input!");
            pressEnterToContinue();
            return;
        }
        cin.ignore();

        for (auto& pkg : Services) {
            if (pkg.id == id) {
                string name;
                int price;

                cout << "\nCurrent Name: " << pkg.name << "\n";
                cout << "New Name (press enter to keep): ";
                getline(cin, name);
                if (!name.empty()) pkg.name = trim(name);

                cout << "Current Price: Rs. " << pkg.price << "\n";
                cout << "New Price (enter 0 to keep): ";
                while (!(cin >> price)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Please enter a valid number: ";
                }
                cin.ignore();
                if (price > 0 && isValidPrice(price)) pkg.price = price;

                savePackages();
                clearScreen();
                printSuccess("Package updated successfully!");
                pressEnterToContinue();
                return;
            }
        }

        printError("Package not found!");
        pressEnterToContinue();
    }

    void deletePackage() {
        clearScreen();
        printHeader("DELETE SERVICE PACKAGE");

        viewAllPackages();
        if (Services.empty()) return;

        int id;
        cout << "Enter Package ID to delete: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input!");
            pressEnterToContinue();
            return;
        }
        cin.ignore();

        for (auto it = Services.begin(); it != Services.end(); ++it) {
            if (it->id == id) {
                cout << "\nAre you sure you want to delete '" << it->name << "'?  (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore();
                if (tolower(confirm) == 'y') {
                    Services.erase(it);
                    savePackages();
                    clearScreen();
                    printSuccess("Package deleted successfully!");
                } else {
                    printInfo("Deletion cancelled.");
                }
                pressEnterToContinue();
                return;
            }
        }

        printError("Package not found!");
        pressEnterToContinue();
    }

    void manageRates() {
        while (true) {
            clearScreen();
            printHeader("MANAGE SERVICE RATES");
            cout << "\n  1. View All Rates\n";
            cout << "  2. Add New Rate\n";
            cout << "  3. Update Rate\n";
            cout << "  4. Delete Rate\n";
            cout << "  5. Back to Main Menu\n";
            cout << "\n============================================================\n";
            cout << "Enter your choice (1-5): ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid input! Please enter a number.");
                pressEnterToContinue();
                continue;
            }
            cin.ignore();

            switch (choice) {
                case 1:
                    viewAllRates();
                    break;
                case 2:
                    addRate();
                    break;
                case 3:
                    updateRate();
                    break;
                case 4:
                    deleteRate();
                    break;
                case 5:
                    return;
                default:
                    printError("Invalid choice! Please try again.");
                    pressEnterToContinue();
            }
        }
    }

    void viewAllRates() {
        clearScreen();
        printHeader("ALL SERVICE RATES");

        if (rates.empty()) {
            printInfo("No service rates available!");
        } else {
            cout << setw(5) << "ID" << setw(30) << "Service" << setw(15) << "Rate (Rs.)" << "\n";
            cout << "------------------------------------------------------------\n";
            for (const auto& rate : rates) {
                cout << setw(5) << rate.id
                     << setw(30) << rate.serviceName
                     << setw(15) << rate.rate << "\n";
            }
        }
        pressEnterToContinue();
    }

    void addRate() {
        clearScreen();
        printHeader("ADD NEW SERVICE RATE");

        string name;
        int rateVal = 0;

        cout << "Service Name: ";
        getline(cin, name);
        name = trim(name);
        if (name.empty()) {
            printError("Service name cannot be empty!");
            pressEnterToContinue();
            return;
        }
        
        cout << "Rate (Rs.): ";
        while (!(cin >> rateVal) || !isValidPrice(rateVal)) {
            cin.clear();
            cin. ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();

        int newId = rates.empty() ? 1 : (rates.back().id + 1);
        rates.push_back({newId, name, rateVal});
        saveRates();

        clearScreen();
        printSuccess("Service rate added successfully!");
        cout << "Rate ID: " << newId << "\n";
        cout << "Service: " << name << "\n";
        cout << "Rate: Rs.  " << rateVal << "\n";
        pressEnterToContinue();
    }

    void updateRate() {
        clearScreen();
        printHeader("UPDATE SERVICE RATE");

        viewAllRates();
        if (rates.empty()) return;

        int id;
        cout << "Enter Service Rate ID to update: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input!");
            pressEnterToContinue();
            return;
        }
        cin.ignore();

        for (auto& rate : rates) {
            if (rate.id == id) {
                string name;
                int newRate = 0;

                cout << "\nCurrent Service: " << rate.serviceName << "\n";
                cout << "New Service Name (press enter to keep): ";
                getline(cin, name);
                if (!name. empty()) rate.serviceName = trim(name);

                cout << "Current Rate: Rs. " << rate.rate << "\n";
                cout << "New Rate (enter 0 to keep): ";
                while (!(cin >> newRate)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Please enter a valid number: ";
                }
                cin.ignore();
                if (newRate > 0 && isValidPrice(newRate)) rate.rate = newRate;

                saveRates();
                clearScreen();
                printSuccess("Service rate updated successfully!");
                pressEnterToContinue();
                return;
            }
        }

        printError("Service rate not found!");
        pressEnterToContinue();
    }

    void deleteRate() {
        clearScreen();
        printHeader("DELETE SERVICE RATE");

        viewAllRates();
        if (rates.empty()) return;

        int id;
        cout << "Enter Service Rate ID to delete: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input!");
            pressEnterToContinue();
            return;
        }
        cin.ignore();

        for (auto it = rates.begin(); it != rates.end(); ++it) {
            if (it->id == id) {
                cout << "\nAre you sure you want to delete '" << it->serviceName << "'? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore();
                if (tolower(confirm) == 'y') {
                    rates.erase(it);
                    saveRates();
                    clearScreen();
                    printSuccess("Service rate deleted successfully!");
                } else {
                    printInfo("Deletion cancelled.");
                }
                pressEnterToContinue();
                return;
            }
        }

        printError("Service rate not found!");
        pressEnterToContinue();
    }

    void generateInvoice() {
        clearScreen();
        printHeader("GENERATE INVOICE");

        viewCustomerActivities();
        if (bookings.empty()) {
            printInfo("No bookings available!");
            pressEnterToContinue();
            return;
        }

        int bookingId;
        cout << "Enter Booking ID: ";
        if (!(cin >> bookingId)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input!");
            pressEnterToContinue();
            return;
        }
        cin.ignore();

        for (auto& booking : bookings) {
            if (booking.id == bookingId) {
                clearScreen();
                printHeader("INVOICE DETAILS");
                cout << "\nBooking ID         : " << booking.id << "\n";
                cout << "Customer Email     : " << booking.customerEmail << "\n";
                cout << "Service            : " << booking.serviceType << "\n";
                cout << "Date               : " << booking.date << "\n";
                cout << "Time               : " << booking.timeSlot << "\n";
                cout << "Total Amount (Rs.) : " << booking.totalAmount << "\n";
                cout << "Status             : " << booking.status << "\n";
                cout << "\n============================================================\n";

                if (toLowerStr(booking.status) != "paid") {
                    cout << "Mark this invoice as paid? (y/n): ";
                    char choice;
                    cin >> choice;
                    cin.ignore();

                    if (tolower(choice) == 'y') {
                        booking.status = "Paid";
                        saveBookings();
                        clearScreen();
                        printSuccess("Invoice marked as paid!");
                    }
                } else {
                    printInfo("This invoice is already marked as paid.");
                }

                pressEnterToContinue();
                return;
            }
        }

        printError("Booking not found!");
        pressEnterToContinue();
    }

    void viewCustomerActivities() {
        clearScreen();
        printHeader("CUSTOMER ACTIVITIES");

        if (bookings.empty()) {
            printInfo("No customer activities found!");
        } else {
            cout << setw(8) << "Booking" << setw(25) << "Customer Email" << setw(25) << "Service"
                 << setw(12) << "Amount" << setw(12) << "Date" << setw(12) << "Status" << "\n";
            cout << "------------------------------------------------------------\n";
            for (const auto& booking : bookings) {
                cout << setw(8) << booking. id
                     << setw(25) << booking.customerEmail
                     << setw(25) << booking.serviceType
                     << setw(12) << booking.totalAmount
                     << setw(12) << booking.date
                     << setw(12) << booking.status << "\n";
            }
        }
        pressEnterToContinue();
    }

    void customerMenu() {
        while (isLoggedIn) {
            clearScreen();
            printHeader("CUSTOMER DASHBOARD");
            cout << "Welcome, " << currentUser. name << "!\n\n";
            cout << "  1. Book Service Slot\n";
            cout << "  2. View Service Charges\n";
            cout << "  3. View My Invoices\n";
            cout << "  4. Logout\n";
            cout << "\n============================================================\n";
            cout << "Enter your choice (1-4): ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid input! Please enter a number.");
                pressEnterToContinue();
                continue;
            }
            cin.ignore();

            switch (choice) {
                case 1:
                    bookServiceSlot();
                    break;
                case 2:
                    viewServiceCharges();
                    break;
                case 3:
                    viewMyInvoices();
                    break;
                case 4:
                    isLoggedIn = false;
                    clearScreen();
                    printSuccess("Logged out successfully!");
                    pressEnterToContinue();
                    return;
                default:
                    printError("Invalid choice! Please try again.");
                    pressEnterToContinue();
            }
        }
    }

    void bookServiceSlot() {
        clearScreen();
        printHeader("BOOK SERVICE SLOT");

        if (Services.empty()) {
            printInfo("No Services available!");
            pressEnterToContinue();
            return;
        }

        cout << "\nAvailable Service Services:\n";
        cout << setw(5) << "ID" << setw(30) << "Name" << setw(15) << "Price (Rs.)" << "\n";
        cout << "------------------------------------------------------------\n";
        for (const auto& pkg : Services) {
            cout << setw(5) << pkg. id
                 << setw(30) << pkg.name
                 << setw(15) << pkg. price << "\n";
        }

        int packageId;
        cout << "\nSelect Package ID: ";
        if (!(cin >> packageId)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input!");
            pressEnterToContinue();
            return;
        }
        cin.ignore();

        ServicePackage* selectedPackage = nullptr;
        for (auto& pkg : Services) {
            if (pkg.id == packageId) {
                selectedPackage = &pkg;
                break;
            }
        }

        if (!selectedPackage) {
            printError("Invalid package selection!");
            pressEnterToContinue();
            return;
        }

        string date;
        cout << "\nEnter preferred date (YYYY-MM-DD)\n";
        cout << "Today: [" << getCurrentDate() << "]\n";
        cout << "Date: ";
        getline(cin, date);
        date = trim(date);
        if (date.empty()) date = getCurrentDate();
        if (! isValidDate(date)) {
            pressEnterToContinue();
            return;
        }

        string timeSlot;
        cout << "\nAvailable working hours: 09:00 AM to 07:30 PM\n";
        cout << "Enter time slot (format: HH:MM AM/PM, e.g., 09:30 AM): ";
        getline(cin, timeSlot);
        timeSlot = trim(timeSlot);
        
        if (! isValidTime12HourSlot(timeSlot)) {
            pressEnterToContinue();
            return;
        }

        if (! isSlotAvailable(date, timeSlot)) {
            printError("Selected slot is already booked.  Choose another time.");
            pressEnterToContinue();
            return;
        }

        Booking newBooking;
        newBooking.id = getNextBookingId();
        newBooking.customerEmail = currentUser.email;
        newBooking.serviceType = selectedPackage->name;
        newBooking. totalAmount = selectedPackage->price;
        newBooking.date = date;
        newBooking.timeSlot = timeSlot;
        newBooking.status = "Pending";

        bookings.push_back(newBooking);
        saveBookings();

        clearScreen();
        printSuccess("Service booked successfully!");
        cout << "\n  Booking ID      : " << newBooking.id << "\n";
        cout << "  Service         : " << newBooking.serviceType << "\n";
        cout << "  Date            : " << newBooking.date << "\n";
        cout << "  Time            : " << newBooking.timeSlot << "\n";
        cout << "  Amount (Rs.)    : " << newBooking.totalAmount << "\n";
        cout << "  Status          : " << newBooking.status << "\n";

        pressEnterToContinue();
    }

    void viewServiceCharges() {
        clearScreen();
        printHeader("SERVICE CHARGES");

        cout << "SERVICE RATES:\n";
//        cout << "------------------------------------------------------------\n";
	      cout << setw(5) << "ID" << setw(30) << "Name" << setw(15) << "Price (Rs.)" << "\n";
//        cout << "------------------------------------------------------------\n";
        for (const auto& pkg : Services) {
            cout << setw(5) << pkg.id
                 << setw(30) << pkg.name
                 << setw(15) << pkg.price << "\n";
        }

//        cout << "\n\nINDIVIDUAL SERVICES:\n";
//        cout << "------------------------------------------------------------\n";
//        cout << setw(5) << "ID" << setw(30) << "Service" << setw(15) << "Rate (Rs.)" << "\n";
//        cout << "------------------------------------------------------------\n";
        for (const auto& rate : rates) {
            cout << setw(5) << rate. id
                 << setw(30) << rate.serviceName
                 << setw(15) << rate.rate << "\n";
        }

        pressEnterToContinue();
    }

    void viewMyInvoices() {
        clearScreen();
        printHeader("MY INVOICES");

        bool found = false;
        for (const auto& booking : bookings) {
            if (toLowerStr(booking.customerEmail) == toLowerStr(currentUser.email)) {
                if (!found) {
                    cout << setw(12) << "Booking" << setw(20) << "Service" << setw(12) << "Date"
                         << setw(12) << "Time" << setw(12) << "Amount" << setw(12) << "Status" << "\n";
                    cout << "------------------------------------------------------------\n";
                    found = true;
                }
                cout << setw(12) << booking.id
                     << setw(20) << booking.serviceType
                     << setw(12) << booking. date
                     << setw(12) << booking.timeSlot
                     << setw(12) << booking.totalAmount
                     << setw(12) << booking.status << "\n";
            }
        }

        if (!found) {
            printInfo("No invoices found for your account!");
        }

        pressEnterToContinue();
    }
};

int main() {
    CarServiceSystem system;
    system.run();
    return 0;
}
