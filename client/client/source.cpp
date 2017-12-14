
#include "Client.h"

#define PORT 1111
#define LocalHost "127.0.0.1"

//send int to tell lenght of sting
//send string
char answer;
std::string HostIP;
int Port;

int main()
{
	begin1:
	printf("Use localHost (Y/N): ");
	std::cin >> answer;
	if (answer == 'Y' || answer == 'y')
	{
		HostIP = LocalHost;
	}
	else if (answer == 'N' || answer == 'n')
	{
		printf("Give Host IP: ");
		std::cin >> HostIP;
	}
	else
	{
		printf("Unknown answer, try again...\n\n");
		answer = '\0';
		goto begin1;
	}
	//-------------------------------------------------
	begin2:
	printf("Use custom port (Y/N):");
	std::cin >> answer;
	if (answer == 'Y' || answer == 'y')
	{
		printf("Give Host Port: ");
		std::cin >> Port;
	}
	else if (answer == 'N' || answer == 'n')
	{
		Port = PORT;
	}
	else
	{
		printf("Unknown answer, try again...\n\n");
		answer = '\0';
		goto begin2;
	}
	//-------------------------------------------------

	Client myClient(HostIP, Port);

	if (!myClient.Connect())
	{
		printf("Failed to connect server...\n");
		system("pause");
		return 1;
	}
	
	std::string userInput;
	while (true)
	{
		std::getline(std::cin, userInput);
		if (!myClient.SendString(userInput))
			break;
		Sleep(10);
	}

	system("pause");
	return 0;
}