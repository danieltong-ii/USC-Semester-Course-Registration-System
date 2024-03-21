// TONGD
// EE450
// Created by Daniel Tong on 11/26/22.

#ifndef SOCKET_SERVEREE_H
#define SOCKET_SERVEREE_H

#include <iostream>
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

using namespace std;


class serverEE {

private:
    string code;
public:
    void setCode(const string &code);

    void setCredits(const string &credits);

    void setProfessor(const string &professor);

    void setDays(const string &days);

    void setCourseName(const string &courseName);

private:
    string credits;
    string professor;
    string days;
    string courseName;

public:
    const string &getCode() const;

    const string &getCredits() const;

    const string &getProfessor() const;

    const string &getDays() const;

    const string &getCourseName() const;
};

#endif //SOCKET_SERVEREE_H
