#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm> // For std::find

// 1. Course Class Definition
class Course {
private:
    std::string courseCode;
    std::string name;
    int maxCapacity;
    int currentEnrollment;

public:
    // Constructor
    Course(std::string code, std::string courseName, int capacity)
        : courseCode(code), name(courseName), maxCapacity(capacity), currentEnrollment(0) {}

    // Getters
    std::string getCode() const { return courseCode; }
    std::string getName() const { return name; }
    int getCapacity() const { return maxCapacity; }
    int getEnrollment() const { return currentEnrollment; }

    // Enrollment methods
    bool enrollStudent() {
        if (currentEnrollment < maxCapacity) {
            currentEnrollment++;
            return true;
        }
        return false; // Course is full
    }

    void dropStudent() {
        if (currentEnrollment > 0) {
            currentEnrollment--;
        }
    }

    // Display info
    void displayCourseInfo() const {
        std::cout << "  Code: " << courseCode
                  << ", Name: " << name
                  << ", Enrollment: " << currentEnrollment << "/" << maxCapacity
                  << std::endl;
    }
};

// 2. Student Class Definition
class Student {
private:
    int studentID;
    std::string name;
    // Store codes of enrolled courses
    std::vector<std::string> enrolledCourses;

public:
    // Constructor
    Student(int id, std::string sName) : studentID(id), name(sName) {}

    // Getters
    int getID() const { return studentID; }
    std::string getName() const { return name; }
    const std::vector<std::string>& getEnrolledCourses() const {
        return enrolledCourses;
    }

    // Registration Methods
    bool registerCourse(std::string courseCode) {
        // Check if already enrolled
        if (std::find(enrolledCourses.begin(), enrolledCourses.end(), courseCode) == enrolledCourses.end()) {
            enrolledCourses.push_back(courseCode);
            return true;
        }
        return false; // Already enrolled
    }

    // Display info
    void displayEnrollment() const {
        std::cout << "\n--- **" << name << " (" << studentID << ")** Enrolled Courses ---" << std::endl;
        if (enrolledCourses.empty()) {
            std::cout << "  No courses registered." << std::endl;
        } else {
            for (const auto& code : enrolledCourses) {
                std::cout << "  - " << code << std::endl;
            }
        }
    }
};


// Main System Logic
void viewAvailableCourses(const std::map<std::string, Course>& courses) {
    std::cout << "\n--- **Available Courses** ---" << std::endl;
    if (courses.empty()) {
        std::cout << "  No courses are currently available." << std::endl;
        return;
    }
    for (const auto& pair : courses) {
        pair.second.displayCourseInfo();
    }
}

void registerStudentForCourse(Student& student, std::map<std::string, Course>& courses, const std::string& courseCode) {
    auto it = courses.find(courseCode);

    if (it == courses.end()) {
        std::cout << "❌ **Error:** Course code " << courseCode << " not found." << std::endl;
        return;
    }

    Course& course = it->second;

    if (course.getEnrollment() >= course.getCapacity()) {
        std::cout << "❌ **Error:** Course " << courseCode << " is full." << std::endl;
        return;
    }

    if (student.registerCourse(courseCode)) {
        course.enrollStudent();
        std::cout << "✅ **Success!** " << student.getName()
                  << " registered for " << course.getName() << " (" << courseCode << ")." << std::endl;
    } else {
        std::cout << "❌ **Error:** " << student.getName()
                  << " is already enrolled in " << course.getName() << " (" << courseCode << ")." << std::endl;
    }
}


int main() {
    // Data Storage:
    // 1. Available Courses: Use a map (Code -> Course object) for quick lookups
    std::map<std::string, Course> availableCourses;
    // 2. Student Records: Use a vector
    std::vector<Student> studentRecords;


    // --- 1. System Initialization (Adding initial data) ---
    availableCourses.emplace("CS101", Course("CS101", "Intro to Programming", 5));
    availableCourses.emplace("MA205", Course("MA205", "Calculus III", 3));
    availableCourses.emplace("PH102", Course("PH102", "Classical Mechanics", 4));

    studentRecords.emplace_back(1001, "Alice");
    studentRecords.emplace_back(1002, "Bob");


    std::cout << "--- **Course Registration System Start** ---" << std::endl;

    // --- 2. View Available Courses ---
    viewAvailableCourses(availableCourses);

    // --- 3. Registration Simulation ---

    // Student Alice (ID: 1001) registers for CS101 and MA205
    registerStudentForCourse(studentRecords[0], availableCourses, "CS101");
    registerStudentForCourse(studentRecords[0], availableCourses, "MA205");
    // Attempt to register again (should fail)
    registerStudentForCourse(studentRecords[0], availableCourses, "CS101");
    
    // Student Bob (ID: 1002) registers for PH102
    registerStudentForCourse(studentRecords[1], availableCourses, "PH102");

    // Let's make CS101 full
    Student newStudent1(1003, "Charlie");
    registerStudentForCourse(newStudent1, availableCourses, "CS101");
    Student newStudent2(1004, "David");
    registerStudentForCourse(newStudent2, availableCourses, "CS101");
    Student newStudent3(1005, "Eve");
    registerStudentForCourse(newStudent3, availableCourses, "CS101");

    // Attempt to register for a full course (should fail)
    Student newStudent4(1006, "Frank");
    registerStudentForCourse(newStudent4, availableCourses, "CS101");

    // Attempt to register for a non-existent course (should fail)
    registerStudentForCourse(studentRecords[0], availableCourses, "BI300");

    // --- 4. Final Status ---
    viewAvailableCourses(availableCourses);

    studentRecords[0].displayEnrollment();
    studentRecords[1].displayEnrollment();

    std::cout << "\n--- **System End** ---" << std::endl;

    return 0;
}
