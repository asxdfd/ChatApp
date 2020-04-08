#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void clientThread(SOCKET& src, SOCKET& dst, const int id)
{
	while (1)
	{
		char recv_buf[1024];
		memset(recv_buf, 0, 1024);
		int size = recv(src, recv_buf, sizeof(recv_buf), 0);
		if (size <= 0)
		{
			cout << "client" << id << " disconnected." << endl;
			return;
		}
		else
		{
			cout << "client" << id << " send: " << recv_buf << endl;
			string recv_str = recv_buf;
			string send_str = "client" + to_string(id) + ": " + recv_str;
			send(dst, send_str.c_str(), sizeof(send_str), 0);
		}
	}
}

int main()
{
	int Ret;
	WSADATA wsaData;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cout << "WSAStartup() failed with error " << Ret << endl;
		WSACleanup();
	}
	
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1080);

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(server, (sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		cout << "bind error." << endl;
		exit(1);
	}
	if (listen(server, SOMAXCONN) < 0)
	{
		cout << "listen error." << endl;
		exit(1);
	}

	sockaddr_in client_addr;
	int len = sizeof(client_addr);
	SOCKET client1 = accept(server, (sockaddr*)&client_addr, &len);
	cout << "client1 connected." << endl;

	SOCKET client2 = accept(server, (sockaddr*)&client_addr, &len);
	cout << "client2 connected." << endl;

	thread client1_thread(clientThread, ref(client1), ref(client2), 1);
	thread client2_thread(clientThread, ref(client2), ref(client1), 2);
	client1_thread.join();
	client2_thread.join();


	closesocket(server);
	closesocket(client1);
	closesocket(client2);
	WSACleanup();

	return 0;
}