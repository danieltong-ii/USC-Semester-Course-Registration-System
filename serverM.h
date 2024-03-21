// TONGD
// EE450
// Created by Daniel Tong on 11/26/22.

#ifndef SOCKET_SERVERM_H
#define SOCKET_SERVERM_H


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

class serverM {
public:
    void encryptText(char *text, size_t size);
    void printmessages();
};


#endif //SOCKET_SERVERM_H
