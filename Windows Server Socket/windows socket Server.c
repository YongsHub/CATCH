#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib,"wsock32.lib")
#define PORT 9161
#define MAX_CLNT 100

void ErrorHandling(char* msg);
void initBaseBall();
unsigned __stdcall  playBaseballGame(void* arg);
void SendMsg(char* msg, int len);

unsigned threadID;  // �������� ID �� ������ ����

int clientCnt = 0;  // ������ client ��
SOCKET clientArray[MAX_CLNT];  // ������ client �迭
HANDLE Mutalisk;  // Mutex 

int main() {
    int clntAdrSize;
    WSADATA wsadata;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddress, clintAddress;

    // winsock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        ErrorHandling("WSAStartup() error!");

    // Mutex ����
    Mutalisk = CreateMutex(NULL, FALSE, NULL);
    // Server Socket ����
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    printf("���� ������ ���� �߽��ϴ�.\n");

    ZeroMemory(&serverAddress, 0, sizeof(serverAddress)); // serverAddress ����ü�� 0���� �ʱ�ȭ
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	    // serverAddress�� IP ����
    serverAddress.sin_family = AF_INET;		    // �ּ� Ÿ�� ����
    serverAddress.sin_port = htons(PORT);		    // Ŭ���̾�Ʈ�� �޽����� �ְ� ���� ��Ʈ ��ȣ

     // ���� ���Ͽ� �ּ����� ����
    if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    // Ŭ���̾�Ʈ�� ������ ��ٸ��� ������ ��ȯ
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    printf("Ŭ���̾�Ʈ�� ������ ��ٸ��ϴ�......\n");

    srand((unsigned)time(NULL));    // ������ �����ϱ� ���� �ʱ�ȭ

    while (1) {
        clntAdrSize = sizeof(clintAddress);

        // Ŭ���̾�Ʈ�� ������ accept()�ϰ� Ŭ���̾�Ʈ�� �����͸� �ְ�޴� Ŭ���̾�Ʈ ���ϻ���
        clientSocket = accept(serverSocket, (SOCKADDR*)&clintAddress, &clntAdrSize);

        WaitForSingleObject(Mutalisk, INFINITE);
        clientArray[clientCnt++] = clientSocket;
        ReleaseMutex(Mutalisk);

        // ��Ƽ ������� ���ھ߱� ������ �����ϰ�, �Լ��� ���ڷ� clientSocket�� ������
        unsigned long hThread = _beginthreadex(NULL, 0, playBaseballGame, (void*)clientSocket, 0, &threadID);
        printf("Ŭ���̾�Ʈ�� ���� �߽��ϴ�.\n");
        printf("������ Ŭ���̾�Ʈ IP: %s \n", inet_ntoa(clintAddress.sin_addr));
        printf("�߱� ���� ����! \n");
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;;
}

/* ���� ��  ��  ����
    ��  ��  ��   ��  */

void ErrorHandling(char* msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
void initBaseBall(int solutionNumber[]) {   // ���ھ߱� ���� �ʱ�ȭ
    do {
        // ������ ���� �� ������ 10���� ���� �������� ����
        solutionNumber[0] = rand() % 10;
        solutionNumber[1] = rand() % 10;
        solutionNumber[2] = rand() % 10;
    }
    // �迭�� ���� ���� ���� ������ �ݺ���
    while ((solutionNumber[0] == solutionNumber[1]) | (solutionNumber[1] == solutionNumber[2]) | (solutionNumber[2] == solutionNumber[0]));
    printf("���� ���� --> %d  %d  %d\n", solutionNumber[0], solutionNumber[1], solutionNumber[2]);
}

unsigned __stdcall  playBaseballGame(void* arg) {
    int solutionNumber[3];
    initBaseBall(solutionNumber);   // ���ھ߱� ���� �ʱ�ȭ

    int clientSocket = (int)arg;
    int score = 0;   // ������ �����ϱ� ���� ����

    // Strike�� 3�� �Ǹ� ���ѷ��� Ż��
    while (1) {
        int clientAnswerNumber[3];  // Ŭ���̾�Ʈ�� �Է��� �� ���� ���ڸ� ������ �迭
        int scoringData[2];         // Server�� ������� ä���Ͽ� client���� ���� data �迭
        int toClientPoint[1];       // ���� ��, Ŭ���̾�Ʈ���� �ο�(����)�� ����(100��)�� ������ �迭
        int changeGame = 1;

        // Ŭ���̾�Ʈ�� �Է��� �� ���� ���ڸ� �о� �鿩�� clientAnswerNumber�� ����
        int ret = recv(clientSocket, (char*)clientAnswerNumber, sizeof(clientAnswerNumber), 0);

        // #1. Ŭ���̾�Ʈ�� ���� ���Ḧ ���� ��, 0 ����
        if (ret <= 0) {
            printf("Ŭ���̾�Ʈ�� ������ ����ƽ��ϴ�.\n");
            break;
        }
        // #2. Ŭ���̾�Ʈ�� ���� ������� ���� ��, 0 0 0 ����
        if ((clientAnswerNumber[0] == 0 && clientAnswerNumber[1] == 0) && (clientAnswerNumber[1] == 0 && clientAnswerNumber[2] == 0)) {
            initBaseBall(solutionNumber);
            continue;
        }

        // Network Byte ���ĵ� ���ڸ� Host Byte�� ������
        for (int index = 0; index < 3; index++)
            clientAnswerNumber[index] = ntohl(clientAnswerNumber[index]);

        // Ŭ���̾�Ʈ�� �Է��� ���� ���
        printf("Ŭ���̾�Ʈ�� �Է��� �� --> %d  %d  %d\n", clientAnswerNumber[0], clientAnswerNumber[1], clientAnswerNumber[2]);
        int strike = 0;   // Strike�� ���� ������ ����
        int ball = 0;     // Ball�� ���� ������ ����

        for (int i = 0; i < 3; i++) {
            // ������ ���ڰ� ����� solutionNumber���� i��° ���ڸ� ������ oneSolNumber�� ����
            int oneSolNumber = solutionNumber[i];
            for (int j = 0; j < 3; j++) {
                // Ŭ���̾�Ʈ���� ������ ���ڰ� ����� clientAnswerNumber�� j��° ���ڸ� ������ oneClientNumber �� ����
                int oneClientNumber = clientAnswerNumber[j];

                if (oneSolNumber == oneClientNumber) {
                    if (i == j)
                        strike++;  // ��ġ�� ���� ��� ���� �� Strike 1����
                    else
                        ball++;    // ���ڸ� ���� �� Ball 1����
                }   // if
            }   // for_j
        }   // for_i

        printf("%d ��Ʈ����ũ  %d ��\n\n", strike, ball);

        // ä�� data�� Ŭ���̾�Ʈ�� �����ϱ� ���ؼ� ��Ʈ��ũ ����Ʈ�� ���� �� toClientScore�� ����
        scoringData[0] = htonl(strike);
        scoringData[1] = htonl(ball);

        send(clientSocket, (char*)scoringData, sizeof(scoringData), 0);

        // ���� �� ���� ����
        if (strike == 3) {
            score += 100;
            toClientPoint[0] = htonl(score);
            send(clientSocket, (char*)toClientPoint, sizeof(toClientPoint), 0);
        }

        WaitForSingleObject(Mutalisk, INFINITE);
        for (int k = 0; k < clientCnt; k++) {  // close�� ������ �迭���� ����
            if (clientSocket == clientArray[k]) {
                while (k++ < clientCnt - 1)
                    clientArray[k] = clientArray[k + 1];
                break;
            }
        }
        clientCnt--;
        ReleaseMutex(Mutalisk);
    }  // while
    closesocket(clientSocket);  //Ŭ���̾�Ʈ ���� ����
    return 0;
}