// TONGD
// EE450
// Created by Daniel Tong on 11/26/22.

#include <iostream>
#include "serverC.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <map>
#include <fstream>

#include "serverM.h"

map<string,string> authMap;


/**
 *  Read in the credential file
 */
void readInCredFile() {
    ifstream fileIn ("cred.txt");
    string currLine;
    string username;
    string password;
    string const delimiter_char = ",";
    int pos = 0;

    while (getline(fileIn, currLine)) {
        pos = currLine.find(delimiter_char);
        username = currLine.substr(0, pos);
        password = currLine.substr(pos + 1);
        authMap.insert(pair<string,string> (username,password));
    }


//    map<string, string>::iterator it;
//    for (it = authMap.begin(); it != authMap.end(); it++) {
//        cout << it->first << endl;
//        cout << it->second << endl;
//    }

}

/**
 * get username
 * @param fromServerM
 * @return
 */

string getUsername(string fromServerM) {
    string const delimiter_char = " ";
    int pos = 0;

    pos = fromServerM.find(delimiter_char);
    return fromServerM.substr(0, pos);
}

/**
 * get password
 * @param fromServerM
 * @return
 */


string getPassword(string fromServerM) {
    string const delimiter_char = " ";
    int pos = 0;
    pos = fromServerM.find(delimiter_char);
    return fromServerM.substr(pos + 1);
}

/**
 * check the authentication map
 * @param username
 * @param password
 * @return
 */

string checkAuthMap(string username, string password) {
    //Check if username exists
    if (authMap.find(username) == authMap.end()) {
        return "0";
    }
    // username exists, check for matching password
    else if (authMap.find(username) != authMap.end()) {
        string storedPassword = authMap.at(username);
        storedPassword = storedPassword.erase(storedPassword.size()-1);
        int res = storedPassword.compare(password);

        if ( res == 0) {
//            cout << "match!" << endl;
//            cout << storedPassword << endl;
//            cout << password << endl;
            return "2";
        }
        else {
//            cout << res << endl;
//            cout << "no match!" << endl;
//            cout << storedPassword.size() << endl;
//            cout << password.size() << endl;
            return "1";
        }
    }
    return "0";
}

/**
 * runs the program
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[]) {

    const int UDP_PORT = 21267;
    const string LOCAL_IP_ADDRESS = "127.0.0.1";
    char buf[52];

    string status = "0";

    // server and client struct
    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;

    memset(&servAddr, 0, sizeof servAddr);

    readInCredFile();

    // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cout << "Error creating socket" << endl;
    }
    else {
        cout << "The ServerC is up and running using UDP on port " + to_string(UDP_PORT) + "." << endl;
    }

    // BIND THE IP ADDRESS AND PORT TO A SOCKET
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, LOCAL_IP_ADDRESS.c_str(), &servAddr.sin_addr);
    bind(sockfd, (sockaddr*)&servAddr, sizeof servAddr);

    // WHILE LOOP TO KEEP THE SERVER ON

    while (true) {
        socklen_t clientAddrLength = sizeof(clientAddr);

        // Clear client structure and receive buffer
        memset(&buf, 0, sizeof buf);
        memset(&clientAddr, 0, sizeof clientAddr);

        int bytesReceieved = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *) &clientAddr, &clientAddrLength);
        if (bytesReceieved < 0) {
            cout << "failed recvform" << endl;
        } else {
            cout << "The ServerC received an authentication request from the Main Server." << endl;
        }
        // Parse for username and password and check against the authmap
        string fromServerM(buf);
        string username = getUsername(fromServerM);
        string password = getPassword(fromServerM);
        status = checkAuthMap(username, password);
        // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
        int sendStatus = sendto(sockfd, status.c_str(), status.size() + 1, 0, (struct sockaddr*) &clientAddr, sizeof clientAddr);
        if (sendStatus < 0) {
            cout << "send failed" << endl;
        }
        else {
            cout << "The ServerC finished sending the response to the Main Server." << endl;
        }
    }
return 0;
}