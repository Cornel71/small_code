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

## Running the Logger

Start the server:

```bash
./command_logger

In a separate terminal (or from a remote machine), send data using nc (Netcat) or telnet:

```bash
echo "STOP_SERVICE" | nc localhost 8080

### Example Output
The logger provides the following terminal output format as of 2026:

```text
Server listening on port 8080...

[CONNECT] 192.168.1.15:54231

Command: START_PROCESS
Command: STATUS_CHECK
[DISCONNECT] Client closed connection.

## Security Considerations

*   **Unencrypted:** This logger transmits data in plain text. Do not use it for sensitive credentials.
*   **Binding:** Currently set to `INADDR_ANY`. For security in 2026, it is recommended to bind to `127.0.0.1` unless external access is required.
*   **Buffer Safety:** Uses `memset` and length-limited `recv` to prevent basic buffer overflow vulnerabilities.
