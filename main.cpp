#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <limits> 

using namespace std;

// --- Class Definitions ---

class Course {
private:
    string code;
    string name;
    int credits;
    int maxCapacity;
    int currentEnrollment;

public:
    // Constructor
    Course(string c = "", string n = "", int cr = 0, int max = 0, int current = 0)
        : code(c), name(n), credits(cr), maxCapacity(max), currentEnrollment(current) {}

    // Accessors (Getters)
    string getCode() const { return code; }
    string getName() const { return name; }
    int getCredits() const { return credits; }
    int getMaxCapacity() const { return maxCapacity; }
    int getCurrentEnrollment() const { return currentEnrollment; }
    int getAvailableSeats() const { return maxCapacity - currentEnrollment; }

    // Mutators (Setters)
    void incrementEnrollment() { currentEnrollment++; }
    void decrementEnrollment() { currentEnrollment--; }
    void setCapacity(int max) { maxCapacity = max; }

    // Display
    void display() const {
        cout << "  Code: " << code
             << ", Name: " << name
             << ", Credits: " << credits
             << ", Capacity: " << currentEnrollment << "/" << maxCapacity
             << (getAvailableSeats() > 0 ? " (Open)" : " (Full)")
             << endl;
    }

    // File I/O format: CODE,NAME,CREDITS,MAX_CAP,CURRENT_ENROLL
    string toFileFormat() const {
        return code + "," + name + "," + to_string(credits) + ","
               + to_string(maxCapacity) + "," + to_string(currentEnrollment);
    }
};

class Student {
private:
    int id;
    string name;
    string password;
    vector<string> registeredCourseCodes; 
    bool isAdmin; // NEW: Flag to identify administrators

public:
    // Constructor
    Student(int i = 0, string n = "", string p = "", bool admin = false)
        : id(i), name(n), password(p), isAdmin(admin) {}

    // Accessors
    int getId() const { return id; }
    string getName() const { return name; }
    string getPassword() const { return password; }
    bool getIsAdmin() const { return isAdmin; } // NEW
    const vector<string>& getRegisteredCourses() const { return registeredCourseCodes; }

    // Mutators
    void addCourse(const string& courseCode) { registeredCourseCodes.push_back(courseCode); }
    void removeCourse(const string& courseCode) {
        for (auto it = registeredCourseCodes.begin(); it != registeredCourseCodes.end(); ++it) {
            if (*it == courseCode) {
                registeredCourseCodes.erase(it);
                return;
            }
        }
    }

    // Check if student is already registered in a course
    bool isRegistered(const string& courseCode) const {
        for (const string& code : registeredCourseCodes) {
            if (code == courseCode) return true;
        }
        return false;
    }

    // File I/O format: ID,NAME,PASSWORD,IS_ADMIN,COURSE_CODE1|...
    string toFileFormat() const {
        string courses = "";
        for (const string& code : registeredCourseCodes) {
            courses += code + "|";
        }
        if (!courses.empty()) {
            courses.pop_back(); // Remove trailing '|'
        }
        return to_string(id) + "," + name + "," + password + "," + (isAdmin ? "1" : "0") + "," + courses;
    }
};

// --- Registration System Core Logic & Global Data ---

vector<Course> allCourses;
vector<Student> allStudents;
Student* loggedInStudent = nullptr; 

// --- Utility Functions ---

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

Course* findCourse(const string& code) {
    for (auto& course : allCourses) {
        if (course.getCode() == code) {
            return &course;
        }
    }
    return nullptr;
}

Student* findStudent(int id) {
    for (auto& student : allStudents) {
        if (student.getId() == id) {
            return &student;
        }
    }
    return nullptr;
}

// --- File Handling Functions ---

void loadData() {
    // Load Courses (unchanged)
    ifstream courseFile("courses.txt");
    string line;
    allCourses.clear();

    while (getline(courseFile, line)) {
        stringstream ss(line);
        string segment;
        vector<string> parts;
        while (getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        if (parts.size() == 5) {
            Course c(parts[0], parts[1], stoi(parts[2]), stoi(parts[3]), stoi(parts[4]));
            allCourses.push_back(c);
        }
    }
    courseFile.close();
    
    // Load Students (MODIFIED to include isAdmin flag)
    ifstream studentFile("students.txt");
    allStudents.clear();

    while (getline(studentFile, line)) {
        stringstream ss(line);
        string segment;
        vector<string> parts;
        while (getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        // Expected parts: ID,NAME,PASSWORD,IS_ADMIN,COURSE_LIST
        if (parts.size() >= 4) {
            try {
                int id = stoi(parts[0]);
                string name = parts[1];
                string password = parts[2];
                bool isAdmin = (parts[3] == "1");

                Student s(id, name, password, isAdmin);

                // Handle the course list (part 4 and potentially beyond if commas in name)
                if (parts.size() > 4) {
                    stringstream cs(parts[4]);
                    string courseCode;
                    while (getline(cs, courseCode, '|')) {
                        if (!courseCode.empty()) {
                            s.addCourse(courseCode);
                        }
                    }
                }
                allStudents.push_back(s);
            } catch (...) {
                cerr << "Error loading student data line: " << line << endl;
            }
        }
    }
    studentFile.close();
}

void saveData() {
    // Save Courses
    ofstream courseFile("courses.txt");
    for (const auto& course : allCourses) {
        courseFile << course.toFileFormat() << endl;
    }
    courseFile.close();

    // Save Students
    ofstream studentFile("students.txt");
    for (const auto& student : allStudents) {
        studentFile << student.toFileFormat() << endl;
    }
    studentFile.close();
}

// --- Student Menu Functions (Same as before) ---

void studentRegisterForCourse() {
    if (!loggedInStudent) return;

    cout << "\n--- Course Registration ---" << endl;
    string code;
    cout << "Enter Course Code to Register: ";
    cin >> code;

    Course* course = findCourse(code);

    if (!course) {
        cout << "** Error: Course not found! **" << endl;
    } else if (loggedInStudent->isRegistered(code)) {
        cout << "** Error: You are already registered in this course. **" << endl;
    } else if (course->getAvailableSeats() <= 0) {
        cout << "** Error: Course is full! **" << endl;
    } else {
        loggedInStudent->addCourse(code);
        course->incrementEnrollment();
        cout << "✅ Successfully registered for " << course->getName() << endl;
    }
}

void studentDropCourse() {
    if (!loggedInStudent) return;

    cout << "\n--- Drop Course ---" << endl;
    string code;
    cout << "Enter Course Code to Drop: ";
    cin >> code;

    Course* course = findCourse(code);

    if (!course) {
        cout << "** Error: Course not found! **" << endl;
    } else if (!loggedInStudent->isRegistered(code)) {
        cout << "** Error: You are not registered in this course. **" << endl;
    } else {
        loggedInStudent->removeCourse(code);
        course->decrementEnrollment();
        cout << "🗑️ Successfully dropped " << course->getName() << endl;
    }
}

void studentViewRegisteredCourses() {
    if (!loggedInStudent) return;

    cout << "\n--- Your Registered Courses ---" << endl;
    const auto& registered = loggedInStudent->getRegisteredCourses();

    if (registered.empty()) {
        cout << "You are not currently registered for any courses." << endl;
        return;
    }

    for (const string& code : registered) {
        Course* course = findCourse(code);
        if (course) {
            course->display();
        } else {
            cout << "  [DELETED] Course Code: " << code << endl;
        }
    }
}

void studentMenu() {
    int choice;
    do {
        cout << "\n*** Student Menu (" << loggedInStudent->getName() << ") ***" << endl;
        cout << "1. View All Available Courses" << endl;
        cout << "2. Register for a Course" << endl;
        cout << "3. Drop a Course" << endl;
        cout << "4. View My Registered Courses" << endl;
        cout << "5. Logout" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) {
            clearInputBuffer();
            choice = 0;
            cout << "** Invalid input. Please enter a number. **" << endl;
            continue;
        }

        switch (choice) {
            case 1:
                cout << "\n--- All Available Courses ---" << endl;
                for (const auto& course : allCourses) {
                    course.display();
                }
                break;
            case 2:
                studentRegisterForCourse();
                break;
            case 3:
                studentDropCourse();
                break;
            case 4:
                studentViewRegisteredCourses();
                break;
            case 5:
                loggedInStudent = nullptr;
                cout << "Logging out." << endl;
                break;
            default:
                cout << "** Invalid choice. Please try again. **" << endl;
        }
    } while (choice != 5);
}

// --- Admin Menu Functions (Same as before) ---

void adminAddCourse() {
    cout << "\n--- Add New Course ---" << endl;
    string code, name;
    int credits, capacity;

    cout << "Enter Course Code (e.g., CS101): ";
    cin >> code;
    if (findCourse(code)) {
        cout << "** Error: Course code already exists. **" << endl;
        return;
    }
    
    cout << "Enter Course Name: ";
    cin.ignore(); 
    getline(cin, name);

    cout << "Enter Credits: ";
    if (!(cin >> credits)) { clearInputBuffer(); cout << "** Invalid input. **" << endl; return; }
    
    cout << "Enter Max Capacity: ";
    if (!(cin >> capacity)) { clearInputBuffer(); cout << "** Invalid input. **" << endl; return; }

    Course newCourse(code, name, credits, capacity);
    allCourses.push_back(newCourse);
    cout << "✅ Course " << code << " added successfully!" << endl;
}

void adminViewAllCourses() {
    cout << "\n--- All Courses in System ---" << endl;
    if (allCourses.empty()) {
        cout << "No courses defined in the system." << endl;
        return;
    }
    for (const auto& course : allCourses) {
        course.display();
    }
}

void adminViewAllStudents() {
    cout << "\n--- All Students in System ---" << endl;
    for (const auto& student : allStudents) {
        cout << (student.getIsAdmin() ? "✨ ADMIN " : "👤 STUDENT ") 
             << "ID: " << student.getId() 
             << ", Name: " << student.getName()
             << ", Courses: " << student.getRegisteredCourses().size() << endl;
    }
}

void adminMenu() {
    int choice;
    do {
        cout << "\n*** Administrator Menu (" << loggedInStudent->getName() << ") ***" << endl;
        cout << "1. Add New Course" << endl;
        cout << "2. View All Courses" << endl;
        cout << "3. View All Students (Data Check)" << endl;
        cout << "4. Logout" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) {
            clearInputBuffer();
            choice = 0;
            cout << "** Invalid input. Please enter a number. **" << endl;
            continue;
        }

        switch (choice) {
            case 1:
                adminAddCourse();
                break;
            case 2:
                adminViewAllCourses();
                break;
            case 3:
                adminViewAllStudents();
                break;
            case 4:
                loggedInStudent = nullptr;
                cout << "Logging out of Admin." << endl;
                break;
            default:
                cout << "** Invalid choice. Please try again. **" << endl;
        }
    } while (choice != 4);
}

// --- First-Run Setup Function ---

void setupFirstAdmin() {
    cout << "\n=======================================" << endl;
    cout << "✨ FIRST-RUN SYSTEM SETUP" << endl;
    cout << "=======================================" << endl;
    cout << "No users found. Please create the initial Administrator account." << endl;
    
    int id;
    string name, password;

    cout << "Enter New Admin ID (e.g., 9999): ";
    if (!(cin >> id)) { clearInputBuffer(); cout << "** Invalid ID. **" << endl; return; }

    cout << "Enter Admin Full Name: ";
    cin.ignore(); 
    getline(cin, name);

    cout << "Enter Admin Password: ";
    cin >> password;

    // Create the admin account (isAdmin set to true)
    Student newAdmin(id, name, password, true);
    allStudents.push_back(newAdmin);
    
    cout << "✅ Administrator account for " << name << " (ID: " << id << ") created successfully!" << endl;
    cout << "Please remember this ID and Password to log in as Admin." << endl;
    saveData(); // Save immediately so the admin account is permanent
}

// --- Authentication and Main Menu ---

void studentLogin() {
    int id;
    string password;

    cout << "\n--- Student Login ---" << endl;
    cout << "Enter Student ID: ";
    if (!(cin >> id)) { clearInputBuffer(); cout << "** Invalid ID. **" << endl; return; }
    cout << "Enter Password: ";
    cin >> password;

    Student* student = findStudent(id);

    if (student && !student->getIsAdmin() && student->getPassword() == password) {
        loggedInStudent = student;
        cout << "Welcome, " << student->getName() << "!" << endl;
        studentMenu();
    } else {
        cout << "** Login failed. Invalid ID, Password, or account is an Admin account. **" << endl;
    }
}

void studentSignup() {
    int id;
    string name, password;
    cout << "\n--- Student Signup ---" << endl;
    
    cout << "Enter New Student ID (4-digit recommended): ";
    if (!(cin >> id)) { clearInputBuffer(); cout << "** Invalid ID. **" << endl; return; }

    // Check if ID already exists
    if (findStudent(id)) {
        cout << "** Error: User ID already registered. **" << endl;
        return;
    }

    cout << "Enter Full Name: ";
    cin.ignore(); 
    getline(cin, name);

    cout << "Enter Password: ";
    cin >> password;

    // Create the student account (isAdmin set to false by default)
    Student newStudent(id, name, password, false); 
    allStudents.push_back(newStudent);
    cout << "✅ Student account created successfully! Remember to Save and Exit to keep it." << endl;
}

void adminLogin() {
    int id;
    string password;
    
    if (allStudents.empty()) {
        cout << "** Error: No users registered. Please create the first admin account using the main menu prompt. **" << endl;
        return;
    }

    cout << "\n--- Administrator Login ---" << endl;
    cout << "Enter Admin ID: ";
    if (!(cin >> id)) { clearInputBuffer(); cout << "** Invalid ID. **" << endl; return; }
    cout << "Enter Password: ";
    cin >> password;

    Student* admin = findStudent(id);
    
    if (admin && admin->getIsAdmin() && admin->getPassword() == password) {
        loggedInStudent = admin;
        cout << "Admin login successful. Welcome, " << admin->getName() << "." << endl;
        adminMenu();
    } else {
        cout << "** Login failed. Invalid Admin ID or Password. **" << endl;
    }
}

// --- Main Program Entry ---

int main() {
    // 1. Load data from files at startup
    loadData();
    cout << "System initialized. Loaded " << allCourses.size() << " courses and " 
         << allStudents.size() << " users." << endl;

    // 2. Check for first run and prompt admin setup
    if (allStudents.empty()) {
        setupFirstAdmin();
    }

    int choice;
    do {
        cout << "\n=======================================" << endl;
        cout << "  College Course Registration System" << endl;
        cout << "=======================================" << endl;
        cout << "1. Student Login" << endl;
        cout << "2. Student Signup" << endl;
        cout << "3. Administrator Login" << endl;
        cout << "4. Save and Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            clearInputBuffer();
            choice = 0;
            cout << "** Invalid input. Please enter a number. **" << endl;
            continue;
        }

        switch (choice) {
            case 1:
                studentLogin();
                break;
            case 2:
                studentSignup();
                break;
            case 3:
                adminLogin();
                break;
            case 4:
                saveData();
                cout << "All data saved. Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "** Invalid choice. Please try again. **" << endl;
        }
    } while (choice != 4);

    return 0;
}
