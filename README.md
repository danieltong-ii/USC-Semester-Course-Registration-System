# Unified Course Query System (UCQS)
### A Network Socket Programming Project

## Introduction

UCQS is a networked application that enables users to query information about courses across different departments from a single interface. At its core, the project allows for the authentication of users and then facilitates the querying of course details such as professor names, course credits, and class schedules. The system interacts with department-specific servers to fetch the requested information, presenting it to the user in a unified manner.

### Educational Context

This project serves as the final project for the University Southern California EE450: Introduction to Computer Networks C.S. Masters Degree course. It's designed to provide practical experience with network programming, client-server architecture, and the TCP/IP protocol suite. The Unified Course Query System (UCQS) specifically applies theoretical concepts from computer networking, such as socket programming, understanding the differences between TCP and UDP protocols, and designing distributed systems. By creating a system that requires communication across multiple servers to fetch and authenticate course information, it demonstrates how different layers of the network stack interact to facilitate communication over a network. This hands-on project offers students an opportunity to explore these concepts in a real-world scenario, solidifying their understanding of key networking principles.

## Technical Details

### Technologies used:

- **Programming Languages:** C++
- **Networking:** TCP/IP for client-server communication, UDP for server-server communication.

### Architecture overview:

The UCQS architecture comprises a main server (serverM), a client application, and department-specific servers (serverCS for Computer Science, serverEE for Electrical Engineering, and serverC for authentication). The client communicates with serverM via TCP. ServerM then uses UDP to interact with serverC for authentication and either serverCS or serverEE to fetch course details, based on the user's query.

### Features

- **User Authentication:** Secure login system to verify the identity of users querying the system.
- **Cross-Department Query Capability:** Allows users to inquire about courses from the Computer Science and Electrical Engineering departments, with potential to expand to more departments.
- **Flexible Query Options:** Users can request specific details about courses, including credits, professor names, days of classes, and course names.
- **Efficient Server Communication:** Utilizes UDP for fast server-to-server communication and TCP for reliable client-server interaction.

## Installation and Usage

1. Compile all `.cpp` files into executable binaries, ensuring you have a C++ compiler installed.
2. Start the department-specific servers (`serverCS`, `serverEE`, and `serverC`) in separate terminal windows or sessions.
3. Launch the main server (`serverM`).
4. Run the client application.
