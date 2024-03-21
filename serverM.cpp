// TONGD
// EE450
// Created by Daniel Tong on 11/26/22.

#include "serverM.h"

#include <iostream>
#include "serverC.h"

using namespace std;

//   Global Variables

// socketAddressStructures
struct sockaddr_in serverAddr;
struct sockaddr_in serverC;
struct sockaddr_in serverEE;
struct sockaddr_in serverCS;
// with details from the recv
struct sockaddr_in serverCAddr;
struct sockaddr_storage clientAddr;

// Port numbers and IP addresses
const int TCP_PORT = 25267;
const int UDP_PORT = 21267;
const int UDP_CS_PORT = 22267;
const int UDP_EE_PORT = 23267;
const int UDP_M_PORT = 24267;
const string SERVER_IP_ADDRESS = "127.0.0.1";

// sockets file descriptors
int childSockfd;
int sockTCP;
int sockUDP;
int sockEE_UDP;
int sockCS_UDP;
socklen_t sin_size;
const int BUF_SIZE = 1024;

// Other strings
string packetToServerC;
string authStatus;
string user;
string departmentCode;

int attempts;


// Other Magic Numbers
const int CHAR_LIMIT = 50;

void encryptText(char* str, size_t length) {

    for (int i = 0; i < length; i++) {
        if (isdigit(str[i])) {
            int currNum = str[i] - '0';
            currNum = currNum + 4;
            if (currNum > 9) {
                str[i] = '0' + (currNum - 10);
            }
            else {
                str[i] = '0' + currNum;
            }
        }
        else if (isalpha(str[i])) {
            if (int(str[i]) >= 87 && int(str[i] <= 90))  {
                str[i] = str[i] - 22;
            } else if (int(str[i]) >= 119 && int(str[i] <= 122)) {
                str[i] = str[i] - 22;
            }
            else {
                str[i] = str[i] + 4;
            }
        }
    }
}

bool errorFound(int status, string step) {
    if (status < 0) {
        cout << "ERROR: An ERROR occurred during the " << step << " phase. Please advise." << endl;
        return true;
    }
    else {
        return false;
    }
}

string CreateUDPPortAndAuthenticateUserWithServerC() {
    // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
    memset(&serverC, 0, sizeof serverC);
    serverC.sin_family = AF_INET;
    serverC.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, SERVER_IP_ADDRESS.c_str(), &serverC.sin_addr);
    sockUDP = socket(AF_INET, SOCK_DGRAM, 0);

    bind(sockUDP, (sockaddr*)&serverC, sizeof serverC);

    int bytesReceieved;
    char buf[BUF_SIZE];

    memset(&serverCAddr, 0, sizeof serverCAddr);
    socklen_t serverCAddrLen = sizeof(serverCAddr);


    int sendStatus = sendto(sockUDP, packetToServerC.c_str(), packetToServerC.size() + 1, 0, (struct sockaddr*) &serverC, sizeof serverC);

    if (!errorFound(sendStatus, "UDP sending")) {
        cout << "The main server sent an authentication request to serverC." << endl;
    }

    memset(&buf, 0, sizeof buf);
    bytesReceieved = recvfrom(sockUDP, buf, sizeof buf, 0, (struct sockaddr*) &serverCAddr, &serverCAddrLen);
    // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
    int getsock_check = getsockname(sockUDP, (struct sockaddr*)&serverCAddr, (socklen_t *)&serverCAddrLen);
    if (getsock_check == -1) {
        perror("getsockname");
        exit(1);
    }

    if (!errorFound(bytesReceieved, "receiving from server C")) {
        cout << "The main server received the result of the authentication request from ServerC using UDP over port " + to_string(UDP_M_PORT) + "." << endl;
        string authResult(buf);
        return authResult;
    }
    return "0";
}

string getCourseCode(string s){
    int pos = 0;
    const string delimiterChar = ",";
    pos = s.find(delimiterChar);
    return s.substr(0,pos);
}

string getCategoryName(string s){
    int pos = 0;
    string const delimiterChar = ",";
    pos = s.find(delimiterChar);
    return s.substr(pos + 1);
}

string getUsername(string s) {
    int pos = 0;
    string const delimiterChar = " ";
    pos = s.find(delimiterChar);
    return s.substr(0,pos);
}

string getDepartment(string s) {
    return s.substr(0,2);
}

void CreateTCPPort(){
    // CREATE A SOCKET | USED PARTS OF BEEJ'S SOCKET PROGRAMMING GUIDE
    sockTCP = socket(AF_INET, SOCK_STREAM, 0);
    errorFound(sockTCP, "socket creation");

    // FILL OUT ADDRSTRUCT
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP_ADDRESS.c_str(), &serverAddr.sin_addr);

    // BIND THE IP ADDRESS AND PORT TO A SOCKET
    bind(sockTCP, (struct sockaddr*)&serverAddr, sizeof serverAddr);

    int listenStatus = listen(sockTCP, 10);
    errorFound(listenStatus, "listening");

    //waiting for a connection
    printf("The main server is up and running\n");
}

void AcceptClient() {
    attempts = 3;
    sin_size = sizeof clientAddr;
    childSockfd = accept(sockTCP, (struct sockaddr*) &clientAddr, &sin_size);
    errorFound(childSockfd, "child socket creation");
}


void ReceiveLoginDetailsFromClient() {

    //USED PARTS OF BEEJ'S SOCKET PROGRAMMING GUIDE
    int usernameBytesReceived;

    char userBuff[BUF_SIZE];
    memset(&userBuff, 0, sizeof userBuff);

    usernameBytesReceived = recv(childSockfd, userBuff, CHAR_LIMIT, 0);

    // Resized array with just the right amount of bytes
    char refinedUserAndPass[usernameBytesReceived];
    memset(&refinedUserAndPass, 0, sizeof refinedUserAndPass);

    copy(userBuff, userBuff + usernameBytesReceived, refinedUserAndPass);
    // Convert to string so that we can print out confirmation
    string userAndPassString(refinedUserAndPass);
    user = getUsername(userAndPassString);


    cout << "The main server received the authentication for "
            + user + " using TCP over port "
            + to_string(TCP_PORT) + "." << endl;

    // ENCRYPT
    encryptText(refinedUserAndPass, usernameBytesReceived);
    string encryptedUserAndPass(refinedUserAndPass);
    packetToServerC = encryptedUserAndPass;
}


void SendAuthStatusBackToClient() {
    int sendStatus = send(childSockfd, authStatus.c_str(), authStatus.size() + 1, 0);
    if (!errorFound(sendStatus, "send auth back to client")) {
        cout << "The main server sent the authentication result to the client." << endl;
    }
}

string ReceiveCourseQuery() {
    int bytesRevd;
    char userBuff[BUF_SIZE];
    string courseCode;

    memset(&userBuff, 0, sizeof userBuff);
    bytesRevd = recv(childSockfd, userBuff, CHAR_LIMIT, 0);

    char refinedRequest[bytesRevd - 1];
    copy(userBuff, userBuff + bytesRevd, refinedRequest);
    string refinedReqString(refinedRequest);

    courseCode = getCourseCode(refinedReqString);
    departmentCode = getDepartment(courseCode);

//    cout << "course: " << courseCode << courseCode.size() << endl;

    if (!errorFound(bytesRevd, "receiving course query")) {
        cout << "The main server received from " + user + " to query course "
        + courseCode + " about " + getCategoryName(refinedReqString)
        + " using TCP over port " + to_string(UDP_M_PORT) << "." << endl;
    }
    return refinedReqString;
}

void sendQueryResultBackToClient(string responseFromDepartment) {
    int sendStatus = send(childSockfd, responseFromDepartment.c_str(), responseFromDepartment.size() + 1, 0);
    if (!errorFound(sendStatus, "send auth back to client")) {
        cout << "The main server sent the query information to the client." << endl;
    }
}

void SendRequestToEEDepartment(string courseRequest) {
    char buf[BUF_SIZE];
    int bytesReceieved;
    // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
    memset(&serverEE, 0, sizeof serverEE);
    serverEE.sin_family = AF_INET;
    serverEE.sin_port = htons(UDP_EE_PORT);
    inet_pton(AF_INET, SERVER_IP_ADDRESS.c_str(), &serverEE.sin_addr);
    sockEE_UDP = socket(AF_INET, SOCK_DGRAM, 0);

    bind(sockEE_UDP, (sockaddr*)&serverEE, sizeof serverEE);

    struct sockaddr_in recvServerEE;
    memset(&recvServerEE, 0, sizeof recvServerEE);
    socklen_t recvServerEELen = sizeof(recvServerEE);

    int sendStatus = sendto(sockEE_UDP, courseRequest.c_str(), courseRequest.size() + 1, 0, (struct sockaddr*) &serverEE, sizeof serverEE);

    if (!errorFound(sendStatus, "UDP sending")) {
        cout << "The main server sent a request to serverEE." << endl;
    }

    memset(&buf, 0, sizeof buf);
    bytesReceieved = recvfrom(sockEE_UDP, buf, sizeof buf, 0, (struct sockaddr*) &recvServerEE, &recvServerEELen);
    if (!errorFound(bytesReceieved, "receiving from server EE")) {
        cout << "The main server received the response from serverEE using UDP over port "
        + to_string(UDP_M_PORT) + "." << endl;

        string responseFromEE(buf);
        sendQueryResultBackToClient(responseFromEE);
    }
}

void SendRequestToCSDepartment(string courseRequest) {
    char buf[BUF_SIZE];
    int bytesReceieved;

    // USED PARTS OF CODE FROM BEEJ'S SOCKET PROGRAMMING GUIDE
    memset(&serverCS, 0, sizeof serverCS);
    serverCS.sin_family = AF_INET;
    serverCS.sin_port = htons(UDP_CS_PORT);
    inet_pton(AF_INET, SERVER_IP_ADDRESS.c_str(), &serverCS.sin_addr);
    sockCS_UDP = socket(AF_INET, SOCK_DGRAM, 0);

    bind(sockCS_UDP, (sockaddr*)&serverCS, sizeof serverCS);

    struct sockaddr_in recvServerCS;
    memset(&recvServerCS, 0, sizeof recvServerCS);
    socklen_t recvServerCSLen = sizeof(recvServerCS);

    int sendStatus = sendto(sockCS_UDP, courseRequest.c_str(), courseRequest.size() + 1, 0, (struct sockaddr*) &serverCS, sizeof serverCS);

    if (!errorFound(sendStatus, "UDP sending")) {
        cout << "The main server sent a request to serverCS." << endl;
    }

    memset(&buf, 0, sizeof buf);
    bytesReceieved = recvfrom(sockCS_UDP, buf, sizeof buf, 0, (struct sockaddr*) &recvServerCS, &recvServerCSLen);
    if (!errorFound(bytesReceieved, "receiving from server CS")) {
        cout << "The main server received the result of the authentication request from ServerCS using UDP over port " + to_string(UDP_M_PORT) + "." << endl;

        string responseFromCS(buf);
        sendQueryResultBackToClient(responseFromCS);
    }
}

int main() {
    string courseRequest;
    // Create TCP Port and start listening
    CreateTCPPort();
    // Accept client's connection request
    AcceptClient();
    // Collaborate with ServerC in authenticating the client
    while (authStatus != "2") {
        ReceiveLoginDetailsFromClient();
        // Send Auth details to ServerC to check off
        authStatus = CreateUDPPortAndAuthenticateUserWithServerC();
        SendAuthStatusBackToClient();
    }

    // EXIT THE AUTH LOOP AND START ANSWERING QUERIES
    while (true) {
        courseRequest = ReceiveCourseQuery();
        if (departmentCode == "EE") {
            SendRequestToEEDepartment(courseRequest);
        } else if (departmentCode == "CS") {
            SendRequestToCSDepartment(courseRequest);
        }
    }
    return 0;
}
