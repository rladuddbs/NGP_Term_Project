#include "Common.h"
#define SERVERPORT 9000
#define BUFSIZE    50

// Ŭ���̾�Ʈ�� ����Ű �� ó�� �Լ�
DWORD WINAPI ProcessClient(LPVOID arg)
{
    int retval;
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    int addrlen = sizeof(clientaddr);

    // Ŭ���̾�Ʈ ���� ���
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("[TCP/%s:%d] Ŭ���̾�Ʈ ó�� ����\n", addr, ntohs(clientaddr.sin_port));

    // ����Ű �� ����
    char buf[BUFSIZE];
    int dir;
    while (1) {
        retval = recv(client_sock, (char*)&dir, sizeof(int), MSG_WAITALL); // ����Ű ������ ����
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0) {
            break; // ���� ����
        }

        buf[retval] = '\0'; // ������ ���ڿ� ���� ó��
        printf("[TCP/%s:%d] ����Ű �Է°�: %d\r", addr, ntohs(clientaddr.sin_port), dir);

        // "exit" �Է� �� ���� ����
        if (strcmp(buf, "exit") == 0) {
            printf("[TCP/%s:%d] Ŭ���̾�Ʈ ���� ���� ��û\n", addr, ntohs(clientaddr.sin_port));
            break;
        }
    }

    // ���� �ݱ�
    closesocket(client_sock);
    printf("[TCP/%s:%d] Ŭ���̾�Ʈ ó�� ����\n", addr, ntohs(clientaddr.sin_port));
    return 0;
}

int main(int argc, char* argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    HANDLE hThread;

    printf("������ ����Ű �� ���� ��� ���Դϴ�...\n");

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // ������ Ŭ���̾�Ʈ ���� ���
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));

        // ������ �����Ͽ� Ŭ���̾�Ʈ ó��
        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
        }
        else {
            CloseHandle(hThread);
        }
    }

    // ���� �ݱ�
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}