#ifndef Server_H
#define Server_H
//#pragma once
//if using VS15 release comment below--v
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

#include <WinSock2.h>
#include <fstream>
#include <iostream>
#include <stdint.h>  //use int32_t
#include <string>

#define LocalHost "127.0.0.1"

enum Packet
{
	P_ChatMessage,
	P_Test
};

class Server
{
public:
	Server(int _port, bool _publicBroadcast = false);
	//~Server();
	bool ListenNewConnection();

private:
	bool _ProcessPacket(int _id, Packet _packetType);
	static void ClientHandlerThread(int _id);
	void _GetServerIP();

	bool _SendAll(int _id, char* _data, int _totalBytes);
	bool _RecvAll(int _id, char* _data, int _totalBytes);
	bool _Sendint32_t(int _id, int32_t _int32_t);
	bool _Getint32_t(int _id, int32_t& _int32_t);
	bool _SendPacketType(int _id, Packet _packetType);
	bool _GetPacketType(int _id, Packet& _packetType);
	bool _SendString(int _id, std::string& _string);
	bool _GetString(int _id, std::string& _string);

	SOCKET connections[100];
	SOCKADDR_IN addr;
	SOCKET socketListen;
	int totalConnections = 0;
	int addrlen = sizeof(addr);
};

static Server* Serverptr;
#endif