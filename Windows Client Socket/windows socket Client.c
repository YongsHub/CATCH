#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib,"wsock32.lib")

#define PORT 9161

int YN();

int main() {
    WSADATA wsadata;
    char str[256];
    int i;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET) { // Ŭ���̾�Ʈ ���� ������ �����ϸ� �����޽����� ����ϰ� �Լ� ����
        printf("Ŭ���̾�Ʈ ������ �����ϴµ� ���� �߽��ϴ�\n");
        return 0;
    }
    printf("Ŭ���̾�Ʈ ������ ���� �߽��ϴ�.\n");

    printf("������ �ּ� �Է�:");
    scanf("%s", str);

    for (i = 0; i < strlen(str); i++) {
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')) {
            printf("�ּҿ� ���ڰ� �����ϴ�.\n");
            return 0;
        }
    }

    // ������ �ּ� ������ ������ ����ü serverAddress ����
    SOCKADDR_IN serverAddress;

    // ����ü�� 0���� �ʱ�ȭ��
    ZeroMemory(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_addr.s_addr = inet_addr(str); // ������ IP ����
    serverAddress.sin_family = AF_INET; // �ּ��� ���� ����
    serverAddress.sin_port = htons(PORT);   // ������ ��Ʈ��ȣ ����

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); // ������ ����

    printf("�߱� ������ �����մϴ�.\n");
    int count = 0;  // ���� �߱� ������ 10�� �����ϱ� ���� ����

    while (1) { //��Ʈ������ 3�̵ɶ� ���� �ݺ�
        int clientNumber[3];
        int clientPoint[1];
        printf("�� ���� �Է��� �ּ���(ex: 1 2 3)\n-->");

        scanf("%d %d %d", &clientNumber[0], &clientNumber[1], &clientNumber[2]);

        // Network Byte ���� ������� ������ ����
        for (int i = 0; i < 3; i++)
            clientNumber[i] = htonl(clientNumber[i]);
        
        send(clientSocket, (char*)clientNumber, sizeof(clientNumber), 0);   // �Էµ� ������ ���� ������ ����

        int score[2];   //�������� ���޵� ��ũ����ũ�� ���� ������ ������ �迭
        recv(clientSocket, (char*)score, sizeof(score), 0); //�������� ������ ��Ʈ����ũ�� ���� ������ �о� ����

        // ��Ʈ��ũ ����Ʈ ���ĵ� �����͸� ȣ��Ʈ ����Ʈ ������ �Ŀ� �� ������ ����
        int strike = ntohl(score[0]);
        int ball = ntohl(score[1]);
        printf(": %d ��Ʈ����ũ   %d��\n\n", strike, ball);

        // 3 ��Ʈ����ũ�̸� ���α׷� ����
        if (strike == 3 && count < 10) {
            recv(clientSocket, (char*)clientPoint, sizeof(clientPoint), 0);
            int point = ntohl(clientPoint[0]);

            printf("�÷��̾� �¸�\n\n");
            printf("100���� ȹ���ϼ̽��ϴ�. �÷��̾��� ������ %d�� �Դϴ�!\n", point);
            if (YN()) {
                count = 0;
                clientNumber[0] = 0;
                clientNumber[1] = 0;
                clientNumber[2] = 0;
                send(clientSocket, (char*)clientNumber, sizeof(clientNumber), 0);
                continue;
            }
            else
                break;
        }
        if (count >= 10) {  // ���� ���� Ƚ���� 10���� �Ѿ ��
            printf("�÷��̾� �й�\n\n");
            if (YN()) {     // ���� �������� ���������� �����ϱ� ���� ���ǹ�
                count = 0;
                clientNumber[0] = 0;
                clientNumber[1] = 0;
                clientNumber[2] = 0;
                send(clientSocket, (char*)clientNumber, sizeof(clientNumber), 0); // ���� ������ ���ڸ� client���� �����Ѵ�.
                continue;
            }
            else
                break;
        }
        count++; // 10���̻� �����, �й踦 �����ϱ� ���� ����

    }
    closesocket(clientSocket);
}

// ������ ��� ����, �������� ����� ���� �Լ�
int YN() {
    char yn[5];

    while (1)
    {
        printf("��� �Ͻðڽ��ϱ�(y/n)?");
        scanf("%s", yn);
        if (yn[0] == 'y' || yn[0] == 'Y') {
            return 1;
        }
        else if (yn[0] == 'n' || yn[0] == 'N') {
            return 0;
        }
        else {
            printf("�߸� �Է��Ͽ����ϴ�. �ٽ� �Է��ϼ���!\n");
        }
    }
}