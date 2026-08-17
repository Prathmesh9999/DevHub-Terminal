# DevHub v1.0

> A terminal-based developer collaboration platform built with C and Modern C++.

## 📌 Project Description

DevHub is a terminal-based client-server application designed to help developers communicate and collaborate across multiple computers.

It provides a centralized server through which multiple DevHub clients can connect and perform developer-oriented tasks such as chatting, sharing files, accessing a remote workspace, building and running projects remotely, and monitoring system resources.

The project is being developed primarily as a C/C++ engineering project to demonstrate networking, multithreading, systems programming, object-oriented programming, file handling, and modular software architecture.

**Platform:** Windows
**Languages:** C + Modern C++17

---

## ✨ Features

### User Management

* User registration
* User login/logout
* Session management
* Online users/devices

### Networking

* TCP client-server communication
* Multiple simultaneous clients
* Packet-based communication
* Request routing

### Chat

* Global chat
* Private messaging
* Online user status

### File Sharing

* File upload
* File download
* File sharing between clients
* Chunk-based transfer
* Transfer progress

### Remote Workspace

* Connect to another DevHub device
* List directories
* Change directories
* Show current directory
* Upload files
* Download files

### Remote Development

* Remote project build
* Remote program execution
* Capture build/run output

### System Dashboard

* CPU usage
* RAM usage
* Disk usage
* Basic machine information

### Supporting Systems

* Logging
* Configuration management
* Terminal command parser
* Error handling

---

## 🛠️ Technologies

| Technology        | Purpose                                                    |
| ----------------- | ---------------------------------------------------------- |
| C                 | Low-level networking, packets, file transfer and utilities |
| C++17             | Application logic, OOP and management components           |
| TCP/IP            | Client-server communication                                |
| Winsock2          | Windows socket programming                                 |
| `std::thread`     | Multithreading                                             |
| `std::mutex`      | Thread synchronization                                     |
| `std::filesystem` | File and directory operations                              |
| Windows API       | System information and process management                  |
| CMake             | Build system                                               |
| Git               | Version control                                            |
| GitHub            | Repository hosting                                         |
| Windows Terminal  | Application interface                                      |

---

## 📁 Project Structure

```text
DevHub/
│
├── core/
│   ├── auth/
│   ├── chat/
│   ├── workspace/
│   ├── dashboard/
│   └── commands/
│
├── networking/
│
├── common/
│
├── logger/
│
├── client/
│
├── server/
│
├── docs/
│
├── tests/
│
├── config/
│
├── storage/
│
├── logs/
│
├── scripts/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
└── .gitignore
```

### Architecture

```text
User
 │
 ▼
Terminal UI
 │
 ▼
Command Parser
 │
 ▼
Application Layer
 │
 ▼
Networking Layer
 │
 ▼
DevHub Server
 │
 ▼
Request Dispatcher
 │
 ▼
Target DevHub Client
 │
 ├── Chat
 ├── File Transfer
 ├── Remote Workspace
 ├── Build/Run
 └── Dashboard
```

---

## 🗺️ Roadmap

### Phase 1 — Foundation

* Project structure
* CMake setup
* Git repository
* Logger
* Configuration system

### Phase 2 — Networking

* TCP server
* TCP client
* Packet protocol
* Multiple clients
* Connection management

### Phase 3 — User & Communication

* Registration/login
* Sessions
* Global chat
* Private messaging
* Online users

### Phase 4 — File Sharing

* Upload
* Download
* Chunk transfer
* Progress tracking

### Phase 5 — Remote Workspace

* Connect to remote device
* Directory operations
* Remote upload/download

### Phase 6 — Remote Development

* Remote build
* Remote run
* Output streaming

### Phase 7 — Monitoring & Polish

* CPU/RAM/Disk dashboard
* Logging
* Error handling
* Testing
* Documentation

### Version 1.0

* Complete feature integration
* Testing
* Final documentation
* Course project submission
* GitHub release

---

## 🎯 Project Goals

DevHub is being developed to demonstrate practical knowledge of:

* C programming
* Modern C++
* Socket programming
* TCP/IP networking
* Multithreading
* Synchronization
* File systems
* Process management
* Object-oriented design
* Client-server architecture
* Software engineering

---

## 🚧 Current Status

**Version:** 1.0 — In Development

The project is currently in the planning and setup stage.
