// TONGD
// EE450
// Created by Daniel Tong on 11/26/22.

#include <sstream>
#include <vector>
#include "serverCS.h"

const int BUF_SIZE = 1024;
map<string, serverCS> databaseCS;

void populateCSDatabase() {
    ifstream ifs("cs.txt");
    string currline;

    while (getline(ifs, currline)) {
        vector<string> v;
        stringstream ss(currline);

        // convert LINE into VECTOR
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            v.push_back(substr);
        }
        serverCS newCourse;
        memset(&newCourse, 0, sizeof newCourse);

        newCourse.setCode(v[0]);
        newCourse.setCredits(v[1]);
        newCourse.setProfessor(v[2]);
        newCourse.setDays(v[3]);
        newCourse.setCourseName(v[4]);
        databaseCS.insert(pair<string, serverCS>(v[0], newCourse));
    }
}

string parseCourseCode(string s) {
    string const delimiterChar = ",";
    int pos = s.find(delimiterChar);

    return (s.substr(0, pos));
}

string parseCategory(string s) {
    string const delimiterChar = ",";
    int pos = s.find(delimiterChar);

    return (s.substr(pos + 1));
}

string processRequest(string course, string category) {

    if (databaseCS.find(course) == databaseCS.end()) {
        return "0";
    }
    else if (databaseCS.find(course) != databaseCS.end()) {
        if (category == "credit") {
            return databaseCS.at(course).getCredits();
        }
        else if (category == "professor") {
            return databaseCS.at(course).getProfessor();
        }
        else if (category == "days") {
            return databaseCS.at(course).getDays();
        }
        else if (category == "course name") {
            return databaseCS.at(course).getCourseName();
        }
    }
    return "0";
}


int main(int argc, char *argv[]) {
    const int UDP_PORT = 22267;
    const string LOCAL_IP_ADDRESS = "127.0.0.1";
    char buf[BUF_SIZE];
    string status = "0";

    populateCSDatabase();

    // server and client struct
    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;

    memset(&servAddr, 0, sizeof servAddr);
    memset(&clientAddr, 0, sizeof clientAddr);

    socklen_t clientAddrLength = sizeof(clientAddr);
    // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cout << "Error creating socket" << endl;
    }
    else {
        cout << "The ServerCS is up and running using UDP on port " + to_string(UDP_PORT) + "." << endl;
    }

    // BIND THE IP ADDRESS AND PORT TO A SOCKET
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, LOCAL_IP_ADDRESS.c_str(), &servAddr.sin_addr);
    bind(sockfd, (sockaddr*)&servAddr, sizeof servAddr);


    while (true) {
        // Clear client structure and receive buffer
        memset(&buf, 0, sizeof buf);
        int numBytesRecvd = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *) &clientAddr, &clientAddrLength);

        char refinedBuf[numBytesRecvd];
        copy(buf, buf + numBytesRecvd, refinedBuf);
        string refinedBytesString(refinedBuf);

        if (numBytesRecvd < 0) {
            cout << "failed recvform" << endl;
        } else {
            cout << "The ServerCS received a request from the Main Server." << endl;
        }

        // Get course code and print requested information
        string courseCode = parseCourseCode(refinedBytesString);
        string courseCategory = parseCategory(refinedBytesString);
        string informationRequested = processRequest(courseCode,courseCategory);

        if (informationRequested != "0") {
            cout << "The course information has been found: The " + courseCategory + " of " + courseCode + " is " + informationRequested + "." << endl;
        } else {
            cout << "Didn’t find the course: " + courseCode + "." << endl;
        }

        // Send information requested by user back to user
        // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
        int sendStatus = sendto(sockfd, informationRequested.c_str(), informationRequested.size() + 1, 0, (struct sockaddr*) &clientAddr, sizeof clientAddr);
        if (sendStatus < 0) {
            cout << "send failed" << endl;
        }
        else {
            cout << "The ServerCS finished sending the response to the Main Server." << endl;
        }
    }
}

const string &serverCS::getCode() const {
    return code;
}

const string &serverCS::getCredits() const {
    return credits;
}

const string &serverCS::getProfessor() const {
    return professor;
}

const string &serverCS::getDays() const {
    return days;
}

const string &serverCS::getCourseName() const {
    return courseName;
}

void serverCS::setCode(const string &code) {
    serverCS::code = code;
}

void serverCS::setCredits(const string &credits) {
    serverCS::credits = credits;
}

void serverCS::setProfessor(const string &professor) {
    serverCS::professor = professor;
}

void serverCS::setDays(const string &days) {
    serverCS::days = days;
}

void serverCS::setCourseName(const string &courseName) {
    serverCS::courseName = courseName;
}

