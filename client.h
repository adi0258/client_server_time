
#include <winsock2.h>
#include <string>

// Function to create a socket and bind to a server.
void createSocket(SOCKET& Socket, sockaddr_in& Server);

// Function to close the socket properly.
void closeSocket(SOCKET& Socket);

// Function to get a command string based on a choice number.
std::string getCommandForChoice(int choice);

// Function to show the menu and get the time format choice.
std::string TimeMenu();

// Function to request time from the server with a specific command.
void RequestTimeFromServer(std::string command);

// Function to estimate average delay between client and server (Request 4).
void AVGDelayEstimation();

// Function to measure average round-trip time (RTT) (Request 5).
void AVGMeasureRTT();

