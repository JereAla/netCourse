
#include "Server.h"

#define PORT 1111

char answer;
bool publicBroadcast = false;
int Port;
int main()
{
	begin:
	printf("Use public broadcast (Y/N): ");
	std::cin >> answer;
	if (answer == 'Y' || answer == 'y')
	{
		publicBroadcast = true;
	}
	else if (answer == 'N' || answer == 'n'){}
	else
	{
		printf("Unknown answer, try again...\n\n");
		answer = '\0';
		goto begin;
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

	Server myServer(Port, publicBroadcast);

	for (int i = 0; i < 100; i++)
	{
		myServer.ListenNewConnection();
	}
	
	system("pause");
	return 0;
}