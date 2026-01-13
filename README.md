# C++ Socket Command Logger (2026)

A lightweight, real-time network listener designed to capture and log commands sent over TCP/IP. This utility acts as a simple monitoring sink for distributed systems or remote command logging.

## 🚀 Features

*   **TCP/IP Listener:** Binds to a specified port and listens for incoming connections.
*   **Real-time Capture:** Stream-based logging of commands received from remote clients.
*   **Client Tracking:** Automatically logs the IP address and source port of every connecting client using `inet_ntop`.
*   **Resource Efficient:** Uses standard POSIX sockets and minimal memory buffering.

## 🛠 Getting Started

### Prerequisites
*   **OS:** Linux, macOS, or Windows (via WSL2).
*   **Compiler:** GCC 11+ or Clang 14+.
*   **Network:** Ability to bind to port 8080 (or your configured port).

### Installation & Compilation
1. Save the source code as `logger.cpp`.
2. Compile using your preferred C++ compiler:

```bash
g++ logger.cpp -o command_logger
