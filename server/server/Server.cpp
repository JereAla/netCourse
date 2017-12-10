#include "Server.h"

Server::Server(int _port, bool _publicBroadcast)
{
	WSADATA wsaData;
	WORD dllVersion = MAKEWORD(2, 1);
	if (WSAStartup(dllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "ERROR :)", MB_OK | MB_ICONERROR);
		exit(1);
	}
	
	if (_publicBroadcast)
		addr.sin_addr.s_addr = htonl(INADDR_ANY);		//public broadcast
	else
		addr.sin_addr.s_addr = inet_addr(LocalHost);		//local broadcast(host ip)

	addr.sin_port = htons(_port);
	addr.sin_family = AF_INET; //IPv4 socket

	socketListen = socket(AF_INET, SOCK_STREAM, NULL);	//create socket to listen for new connections
	
	if (bind(socketListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)		//bind address to socket
	{
		printf("Bind failure: %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	if (listen(socketListen, SOMAXCONN) == SOCKET_ERROR)			//place socket to listen incoming connection
	{
		printf("Failed to listen socket: %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	Serverptr = this;
}
bool Server::ListenNewConnection()
{
	SOCKET newConnection;									//hold client's connection in the socket
	newConnection = accept(socketListen, (SOCKADDR*)&addr, &addrlen);	//accept a new connection

	if (newConnection == 0)
	{
		printf("failed to accept the client's connection.\n");
		return false;
	}
	else
	{
		printf("Client connected\n");
		connections[totalConnections] = newConnection;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(totalConnections), NULL, NULL);

		std::string logMessage = "Welcome to server!\n";
		_SendString(totalConnections, logMessage);

		totalConnections++;
		return true;
	}
}
bool Server::_ProcessPacket(int _id, Packet _packetType)
{
	switch (_packetType)
	{
	case P_ChatMessage:
	{
		std::string message;
		if (!_GetString(_id, message))
			return false;

		for (int i = 0; i < totalConnections; i++)
		{
			if (i == _id)
				continue;

			if (!_SendString(i, message))
				printf("Failed to send message from client ID: %d to client ID: %d\n", _id, i);
		}
		break;
	}
	default:
		printf("Unrecognized packet: %d\n", _packetType);
		break;
	}
	printf("Processed packet from client ID: %d\n", _id);
	return true;
}
void Server::ClientHandlerThread(int _id)
{
	Packet packetType;
	while (true)
	{
		if (!Serverptr->_GetPacketType(_id, packetType))
			break;

		if (!Serverptr->_ProcessPacket(_id, packetType))		//if packet not handled
			break;							//break loop (this should be changed if packet gets lost)
	}
	printf("Lost connetion to client ID: %d\n", _id);
	closesocket(Serverptr->connections[_id]);		//close connection socket
}

bool Server::_SendAll(int _id, char* _data, int _totalBytes)
{
	int bytesSend = 0;
	while (bytesSend < _totalBytes)
	{
		int returnCheck = send(connections[_id], _data + bytesSend, _totalBytes - bytesSend, NULL);
		if (returnCheck == SOCKET_ERROR)
			return false;
		bytesSend += returnCheck;
	}
	return true;
}
bool Server::_RecvAll(int _id, char* _data, int _totalBytes)
{
	int bytesReceived = 0;
	while (bytesReceived <_totalBytes)
	{
		int returnCheck = recv(connections[_id], _data + bytesReceived, _totalBytes - bytesReceived, NULL);
		if (returnCheck == SOCKET_ERROR)
			return false;
		bytesReceived += returnCheck;
	}
	return true;
}
bool Server::_SendInt(int _id, int _int)
{
	//int ReturnCheck = send(connections[_id], (char*)&_int, sizeof(int), NULL);
	//if (ReturnCheck == SOCKET_ERROR)
	//	return false;
	if (!_SendAll(_id, (char*)&_int, sizeof(int)))
		return false;
	return true;
}
bool Server::_GetInt(int _id, int& _int)
{
	//int ReturnCheck = recv(connections[_id], (char*)&_int, sizeof(int), NULL);
	//if (ReturnCheck == SOCKET_ERROR)
	//	return false;
	if (!_RecvAll(_id, (char*)&_int, sizeof(int)))
		return false;
	return true;
}
bool Server::_SendPacketType(int _id, Packet _packetType)
{
	//int ReturnCheck = send(connections[_id], (char*)&_packetType, sizeof(Packet), NULL);
	//if (ReturnCheck == SOCKET_ERROR)
	//	return false;
	if (!_SendAll(_id, (char*)&_packetType, sizeof(Packet)))
		return false;
	return true;
}
bool Server::_GetPacketType(int _id, Packet& _packetType)
{
	//int ReturnCheck = recv(connections[_id], (char*)&_packetType, sizeof(Packet), NULL);
	//if (ReturnCheck == SOCKET_ERROR)
	//	return false;
	if (!_RecvAll(_id, (char*)&_packetType, sizeof(Packet)))
		return false;
	return true;
}
bool Server::_SendString(int _id, std::string& _string)
{
	if (!_SendPacketType(_id, P_ChatMessage))
		return false;

	int bufferLength = _string.size();
	if (!_SendInt(_id, bufferLength))
		return false;
	//int ReturnCheck = send(connections[_id], _string.c_str(), bufferLength, NULL);
	//if (ReturnCheck == SOCKET_ERROR)
	//	return false;
	if (!_SendAll(_id, (char*)_string.c_str(), bufferLength))
		return false;
	return true;
}
bool Server::_GetString(int _id, std::string& _string)
{
	int bufferlength;
	if (!_GetInt(_id, bufferlength))
		return false;

	char* buffer = new char[bufferlength + 1];
	buffer[bufferlength] = '\0';
	//comment lines below when freeing commented code below--v
	//int ReturnCheck = recv(connections[_id], buffer, bufferlength, NULL);
	//_string = buffer;
	//delete[] buffer;
	//if (ReturnCheck == SOCKET_ERROR)
	//	return false;
	//--------------------------------------------------------------
	if (!_RecvAll(_id, buffer, bufferlength))
		return false;
	_string = buffer;
	delete[] buffer;
	//_____________________________________________________________
	return true;
}