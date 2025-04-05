#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <map>

#define TIME_PORT 27015

string handleCommand(const string& command);

int main()
{
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return 1;
	}

	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(TIME_PORT);

	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return 1;
	}

	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char recvBuff[255];
	char sendBuff[255];

	cout << "Time Server: Waiting for clients' requests...\n";

	time_t lastMeasureTimeLap = time(0); // Used for MeasureTimeLap

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			break;
		}

		recvBuff[bytesRecv] = '\0';
		string command(recvBuff);
		cout << "Time Server: Received \"" << command << "\"\n";

		// Special handling for MeasureTimeLap
		if (command == "MeasureTimeLap") {
			time_t now = time(0);
			long secondsPassed = long(difftime(now, lastMeasureTimeLap));
			lastMeasureTimeLap = now;
			sprintf(sendBuff, "Seconds since last MeasureTimeLap: %ld", secondsPassed);
		}
		else {
			string response = handleCommand(command);
			strcpy(sendBuff, response.c_str());
		}

		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			break;
		}

		cout << "Time Server: Sent \"" << sendBuff << "\" (" << bytesSent << " bytes)\n";
	}

	closesocket(m_socket);
	WSACleanup();
	cout << "Time Server: Shutting down.\n";
	return 0;
}

// Helper function for handling different time-related commands
string handleCommand(const string& command)
{
	time_t now = time(0);
	tm* localTime = localtime(&now);
	char buffer[128];

	if (command == "GetTime") {
		strftime(buffer, sizeof(buffer), "Current time: %Y-%m-%d %H:%M:%S", localTime);
		return buffer;
	}
	else if (command == "GetTimeWithoutDate") {
		strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);
		return buffer;
	}
	else if (command == "GetTimeSinceEpoch") {
		sprintf(buffer, "%ld", (long)now);
		return buffer;
	}
	else if (command == "GetTimeWithoutDateOrSeconds") {
		strftime(buffer, sizeof(buffer), "%H:%M", localTime);
		return buffer;
	}
	else if (command == "GetYear") {
		sprintf(buffer, "%d", 1900 + localTime->tm_year);
		return buffer;
	}
	else if (command == "GetMonthAndDay") {
		strftime(buffer, sizeof(buffer), "%m-%d", localTime);
		return buffer;
	}
	else if (command == "GetSecondsSinceBeginingOfMonth") {
		tm startOfMonth = *localTime;
		startOfMonth.tm_mday = 1;
		startOfMonth.tm_hour = 0;
		startOfMonth.tm_min = 0;
		startOfMonth.tm_sec = 0;
		time_t monthStart = mktime(&startOfMonth);
		sprintf(buffer, "%ld", (long)(difftime(now, monthStart)));
		return buffer;
	}
	else if (command == "GetWeekOfYear") {
		strftime(buffer, sizeof(buffer), "Week #: %U", localTime);
		return buffer;
	}
	else if (command == "GetDaylightSavings") {
		return localTime->tm_isdst > 0 ? "1 (DST in effect)" : "0 (Standard time)";
	}
	else if (command.rfind("GetTimeWithoutDateInCity:", 0) == 0) {
		// Extract city name
		string city = command.substr(25); // length of "GetTimeWithoutDateInCity:"

		// Map of cities with UTC offsets in hours
		map<string, int> cityOffsets = {
			{"Doha", 3},
			{"Prague", 2},
			{"NewYork", -4},
			{"Berlin", 2},
			{"Other", 0}
		};

		// Remove spaces and normalize city input (e.g., New York -> NewYork)
		for (auto& ch : city) {
			if (ch == ' ') ch = '\0';
		}

		int offset = 0;
		if (cityOffsets.find(city) != cityOffsets.end())
			offset = cityOffsets[city];
		else
			offset = 0; // Default to UTC

		// Calculate city time
		time_t cityTime = now + offset * 3600;
		tm* adjustedTime = gmtime(&cityTime);
		snprintf(buffer, sizeof(buffer), "Time in %s: %02d:%02d:%02d", city.c_str(), adjustedTime->tm_hour, adjustedTime->tm_min, adjustedTime->tm_sec);
		return buffer;
	}
	else {
		return "Invalid command.";
	}
}
