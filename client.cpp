// TONGD
// EE450
// Created by Daniel Tong on 11/26/22.

#include "client.h"

int sockfd;
string user;

struct sockaddr_in serverAddr;
const string SERVER_IP_ADDRESS = "127.0.0.1";
const int TCP_PORT = 25267;
int CLIENT_PORT;
const int BUF_SIZE = 1024;


/*
 *  categoryToLowerCase(): takes the category given by the user and converts it to lowercase for presentation
 *  input: category
 *  output: category in lowercase
 */


string categoryToLowerCase(string c) {
    if (c == "Professor") {
        return "professor";
    }
    else if (c == "Days") {
        return "days";
    } else if (c == "CourseName") {
        return "course name";
    } else if (c == "Credit") {
        return "credit";
    }
    return "0";
}

/**
 * queryLoop(): this function loops repeatedly, asking for the user to give a course code and category
 * Wait for a response from the serverM and prints.
 */

void queryLoop() {
    string courseCode;
    string category;
    string combined;
    char buf[BUF_SIZE];

    cout << "Please enter the course code to query:";
    cin >> courseCode;
    cout << "Please enter the category (Credit / Professor / Days / CourseName):" << endl;
    cin >> category;

    category = categoryToLowerCase(category);

    combined = courseCode + "," + category;

    int sendStatus = send(sockfd, combined.c_str(), combined.size() + 1, 0);
    if (sendStatus < 0) {
        cout << "send error" << endl;
    } else {
        cout << user + " sent a request to the main server." << endl;
    }
    memset(&buf, 0, sizeof buf);
    int numBytesRecvd = recv(sockfd, buf, BUF_SIZE, 0);
    if (numBytesRecvd < 0) {
        cout << "didn't receive anything back for my query";
    } else {
        cout << "The client received the response from the Main server using TCP over port " + to_string(CLIENT_PORT) + "." << endl;
    }
    char refinedBuf[numBytesRecvd];
    memset(&refinedBuf, 0, sizeof refinedBuf);
    copy(buf, buf+numBytesRecvd, refinedBuf);
    string information(refinedBuf);

    if (information == "0") {
        cout << "Didn’t find the course: " + courseCode + "." << endl << endl;
    } else {
        string output = "The " + category + " of " + courseCode + " is " + information + " ";
        cout << output << endl << endl;
    }
}

/**
 * createTCPSocket(): this function creates the TCP socket using the serverM's IP address and port
 * REUSED CODE; CREDIT TO BEEJ's GUIDE TO NETWORK PROGRAMMING
 */


void createTCPSocket() {
    struct sockaddr_in getPortNumAddr;
    memset(&getPortNumAddr, 0, sizeof getPortNumAddr);
    int portNumAddrLen = sizeof getPortNumAddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Set serverAddress socket struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP_ADDRESS.c_str(), &serverAddr.sin_addr);

    // Tell socket to connect to specified IP address, and the size of the address struct
    connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // No bind because we want it to be dynamic
    cout << "The client is up and running." << endl;

    //Retrieve the locally-bound name of the specified socket and store it in the sockaddr structure
    // Modified a portion of the getsockname block of code from https://gist.github.com/listnukira/4045436
    int getsock_check = getsockname(sockfd, (struct sockaddr*)&getPortNumAddr, (socklen_t *)&portNumAddrLen);
    if (getsock_check < 0) {
        cout << "getsockname" << endl;
        exit(1);
    }
    CLIENT_PORT = ntohs(getPortNumAddr.sin_port);
}

/**
 * getUserCredentials(): this function loops repeatedly, asking for the user to give username and password
 * Wait for a response from the serverM and prints.
 *
 * @return
 */

bool getUserCredentials() {
    const int DEFAULT_NUM_OF_ATTEMPTS = 3;
    string pass;
    string combo;
    bool authSuccessful = false;
    int attemptsRemaining = DEFAULT_NUM_OF_ATTEMPTS;
    char buf[BUF_SIZE];

    while (!authSuccessful && (attemptsRemaining > 0)) {
        cout << "Please enter the username:" << endl;
        cin >> user;
        cout << "Please enter the password:" << endl;
        cin >> pass;
        combo = user + " " + pass;

        int sendStatus = send(sockfd, combo.c_str(), combo.size() + 1, 0);
        if (sendStatus < 0) {
            cout << "send failed" << endl;
        }

        memset(&buf, 0, sizeof buf);
        int recvStatus = recv(sockfd, buf, BUF_SIZE, 0);

        if (recvStatus < 0) {
            cout << "receive failed" << endl;
        } else {
            char refined[recvStatus - 1];
            copy(buf, buf + recvStatus, refined);
            string authStatus(refined);
            int authStatusInt = stoi(authStatus);
            if (authStatusInt == 0) {
                cout << user + " received the result of authentication using TCP over port "
                        + to_string(CLIENT_PORT) + ". Authentication failed: Username Does not exist" << endl;
                attemptsRemaining--;
                cout << "Attempts remaining:" << attemptsRemaining << endl;

            } else if (authStatusInt == 1) {
                cout << user + " received the result of authentication using TCP over port "
                        + to_string(CLIENT_PORT) + ". Authentication failed: Password does not match" << endl;
                attemptsRemaining--;
                cout << "Attempts remaining:" << attemptsRemaining << endl;
            } else if (authStatusInt == 2) {
                cout << user + " received the result of authentication using TCP over port "
                        + to_string(CLIENT_PORT) + ". Authentication is successful" << endl;
                authSuccessful = true;
                return true;
            }
        }
    }
    return false;
}

/*
 *  runs the main program
 */

int main () {
    createTCPSocket();
    if (getUserCredentials()) {
        while (true) {
            queryLoop();
            cout << "-----Start a new request-----" << endl;
        }
    } else {
        cout << "Authentication Failed for 3 attempts. Client will shut down." << endl;
        close(sockfd);
    }
    return 0;
}