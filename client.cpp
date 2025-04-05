#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <windows.h>
#include <ctime>
#include <chrono>
#include "client.h"
#define COLORED
#define TIME_PORT	27015

void main()
{
	// Initialize Winsock(Windows Sockets).
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	while (true)
	{
		string TimeFormat = TimeMenu();
		if (TimeFormat == "Exit")
			break;
		RequestTimeFromServer(TimeFormat);
	}
	WSACleanup();
	system("pause");

}
void createSocket(SOCKET& Socket, sockaddr_in& Server)
{
	// Create a socket and connect to an internet address.
	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		return;
	}
	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	
	Socket = connSocket;
	Server = server;
}
void closeSocket(SOCKET& Socket)
{
	// Close the socket.
	if (SOCKET_ERROR == closesocket(Socket))
	{
		cout << "Time Client: Error at closesocket(): " << WSAGetLastError() << endl;
	}
	Socket = INVALID_SOCKET;
}
string getCommandForChoice(int choice) {
	switch (choice) {
	case 1: return "GetTime";
	case 2: return "GetTimeWithoutDate";
	case 3: return "GetTimeSinceEpoch";
	case 4: return "GetClientToServerDelayEstimation";
	case 5: return "MeasureRTT";
	case 6: return "GetTimeWithoutDateOrSeconds";
	case 7: return "GetYear";
	case 8: return "GetMonthAndDay";
	case 9: return "GetSecondsSinceBeginingOfMonth";
	case 10: return "GetWeekOfYear";
	case 11: return "GetDaylightSavings";
	case 12: return "GetTimeWithoutDateInCity";
	case 13: return "MeasureTimeLap";
	case 14: return "Exit";
	default: return "Invalid";
	}
}
string TimeMenu()
{
	cout << "press the number of time format:\n\n1. GetTime: year,month,day,hour,minute,second\n\n2. GetTimeWithoutDate: hour,minute,second\n\n3. GetTimeSinceEpoch: seconds since 1/1/1970\n\n4. GetClientToServerDelayEstimation \n\n5. MeasureRTT: round trip time \n\n6. GetTimeWithoutDateOrSeconds: hour,minute\n\n7. GetYear \n\n8. GetMonthAndDay: month,day\n\n9. GetSecondsSinceBeginingOfMonth: second\n\n10. GetWeekOfYear: week\n\n11. GetDaylightSavings: 1 for summer clock, 0 for winter clock\n\n12. GetTimeWithoutDateInCity: hour,minute,second in cities\n\n13. MeasureTimeLap: seconds since last MeasureTimeLap request.";
	int choice;
	cin >> choice;
	if (choice >= 1 && choice <= 14 && choice != 12&&choice !=4&&choice!=5)
	{
		return getCommandForChoice(choice);
	}
	else if(choice==4){
		AVGDelayEstimation();
		return " ";
	}
	else if (choice == 5) {
		AVGMeasureRTT();
		return " ";
	}
	else if (choice == 12)
	{
		cout << "Enter the city name (Doha (Qatar), Prague (Czech Republic), NewYork (USA), Berlin (Germany), Other (universal time- UTC)): ";
		string city;
		cin >> city;
		return "GetTimeWithoutDateInCity:" + city;
	}
	else if (choice == 14)
	{
		return "Exit";
	}
	else
	{
		cout << "Invalid choice, please try again.\n";
		return TimeMenu();
	}
}
void RequestTimeFromServer(string command)
{
	if (command == " ")
		return;
	SOCKET connSocket;
	sockaddr_in server;
	createSocket(connSocket, server);

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	const char* sendBuff = command.c_str();
	char recvBuff[255]; 
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closeSocket(connSocket);
		WSACleanup();
		return;
	}
#ifdef COLORED
	cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << "\033[35m" << sendBuff << "\033[0m" << "\" message.\n";
#else
	cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
#endif //COLORED

	// Gets the server's answer using simple recieve (no need to hold the server's address).
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closeSocket(connSocket);
		WSACleanup();
		return;
	}
	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string

#ifdef COLORED
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << "\033[36m" << recvBuff << "\033[0m" << "\" message.\n";
#else
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
#endif //COLORED

}
void AVGDelayEstimation() // request 4
{
	SOCKET connSocket;
	sockaddr_in server;
	createSocket(connSocket, server);

	for (int i = 0; i < 100; i++)
	{
		int bytesSent = 0;
		int bytesRecv = 0;
		string command = getCommandForChoice(1);
		const char* sendBuff = command.c_str();
		char recvBuff[255];
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
		if (bytesSent == SOCKET_ERROR)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closeSocket(connSocket);
			return;
		}
	}
	int timePassed = 0;
	DWORD startTime = GetTickCount();
	cout << "Time Client: Start Time: " << startTime << " ms.\n";
	for (int i = 0; i < 100; i++)
	{
		char recvBuff[255];
		int clientAddrLen = sizeof(server);
		int bytesRecv = recvfrom(connSocket, recvBuff, 255, 0, (SOCKADDR*)&server, &clientAddrLen);
		if (bytesRecv == SOCKET_ERROR)
		{
			cout << "Time Client: Error at recvfrom(): " << WSAGetLastError() << endl;
			closeSocket(connSocket);
			return;
		}

		recvBuff[bytesRecv] = '\0'; // Null-terminate the received message
#ifdef COLORED
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << "\033[36m" << recvBuff << "\033[0m" << "\" message.\n";
#else
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
#endif //COLORED
	}

	DWORD endTime = GetTickCount();
	cout << "Time Client: End Time: " << endTime << " ms.\n";
	timePassed = static_cast<int>(endTime - startTime);
	cout << "Time Client: Delay estimation: " << timePassed / 100 << " ms.\n";

	closeSocket(connSocket);
}
void AVGMeasureRTT() // request 5
{
	SOCKET connSocket;
	sockaddr_in server;
	createSocket(connSocket, server);

	int timePassed = 0;
	DWORD startTime = GetTickCount();
	cout << "Time Client: Start Time: " << startTime << " ms.\n";
	for (int i = 0; i < 100; i++)
	{
		int bytesSent = 0;
		string command = getCommandForChoice(1);
		const char* sendBuff = command.c_str();
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
		if (bytesSent == SOCKET_ERROR)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closeSocket(connSocket);
			return;
		}
		char recvBuff[255];
		int clientAddrLen = sizeof(server);
		int bytesRecv = recvfrom(connSocket, recvBuff, 255, 0, (SOCKADDR*)&server, &clientAddrLen);
		if (bytesRecv == SOCKET_ERROR)
		{
			cout << "Time Client: Error at recvfrom(): " << WSAGetLastError() << endl;
			closeSocket(connSocket);
			return;
		}

		recvBuff[bytesRecv] = '\0'; // Null-terminate the received message
#ifdef COLORED
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << "\033[36m" << recvBuff << "\033[0m" << "\" message.\n";
#else
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
#endif //COLORED
	}

	DWORD endTime = GetTickCount();
	cout << "Time Client: End Time: " << endTime << " ms.\n";
	timePassed = static_cast<int>(endTime - startTime);
	closeSocket(connSocket);
	cout << "Time Client: Average RTT: " << timePassed / 100 << " ms.\n";
}