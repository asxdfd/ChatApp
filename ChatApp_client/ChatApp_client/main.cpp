#include <iostream>
#include <winsock.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void recvThread(SOCKET& server)
{
	while (1)
	{
		char recv_buf[1024];
		int recv_len = recv(server, recv_buf, sizeof(recv_buf), 0);
		if (recv_len < 0) {
			cout << "recv error." << endl;
			return;
		}
		else {
			cout << recv_buf << endl;
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
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(1080);

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(server, (sockaddr *)&server_addr, sizeof(sockaddr)) == SOCKET_ERROR) {
		cout << "connect error." << endl;
		exit(1);
	}
	else {
		cout << "connect succeed." << endl;
	}
	
	thread recv_thread(recvThread, ref(server));
	recv_thread.detach();

	while (1) {
		char send_buf[1024];
		cin >> send_buf;
		int send_len = send(server, send_buf, sizeof(send_buf), 0);
		if (send_len < 0) {
			cout << "send error." << endl;
			break;
		}
	}

	closesocket(server);
	WSACleanup();

	return 0;
}