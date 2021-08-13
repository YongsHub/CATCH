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

    if (clientSocket == INVALID_SOCKET) { // 클라이언트 소켓 생성에 실패하면 에러메시지를 출력하고 함수 종료
        printf("클라이언트 소켓을 생성하는데 실패 했습니다\n");
        return 0;
    }
    printf("클라이언트 소켓을 생성 했습니다.\n");

    printf("접속할 주소 입력:");
    scanf("%s", str);

    for (i = 0; i < strlen(str); i++) {
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')) {
            printf("주소에 문자가 들어갔습니다.\n");
            return 0;
        }
    }

    // 서버의 주소 정보를 저장할 구조체 serverAddress 선언
    SOCKADDR_IN serverAddress;

    // 구조체를 0으로 초기화함
    ZeroMemory(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_addr.s_addr = inet_addr(str); // 서버의 IP 대입
    serverAddress.sin_family = AF_INET; // 주소의 종류 대입
    serverAddress.sin_port = htons(PORT);   // 서버의 포트번호 대입

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); // 서버에 접속

    printf("야구 게임을 시작합니다.\n");
    int count = 0;  // 숫자 야구 게임을 10번 진행하기 위한 변수

    while (1) { //스트라익이 3이될때 까지 반복
        int clientNumber[3];
        int clientPoint[1];
        printf("세 수를 입력해 주세요(ex: 1 2 3)\n-->");

        scanf("%d %d %d", &clientNumber[0], &clientNumber[1], &clientNumber[2]);

        // Network Byte 정렬 방식으로 데이터 정렬
        for (int i = 0; i < 3; i++)
            clientNumber[i] = htonl(clientNumber[i]);
        
        send(clientSocket, (char*)clientNumber, sizeof(clientNumber), 0);   // 입력된 세개의 숫자 서버로 전송

        int score[2];   //서버에서 전달된 스크라이크와 볼의 개수를 저장할 배열
        recv(clientSocket, (char*)score, sizeof(score), 0); //서버에서 전달한 스트라이크와 볼의 개수를 읽어 들임

        // 네트워크 바이트 정렬된 데이터를 호스트 바이트 정렬한 후에 각 변수에 대입
        int strike = ntohl(score[0]);
        int ball = ntohl(score[1]);
        printf(": %d 스트라이크   %d볼\n\n", strike, ball);

        // 3 스트라이크이면 프로그램 종료
        if (strike == 3 && count < 10) {
            recv(clientSocket, (char*)clientPoint, sizeof(clientPoint), 0);
            int point = ntohl(clientPoint[0]);

            printf("플레이어 승리\n\n");
            printf("100점을 획득하셨습니다. 플레이어의 점수는 %d점 입니다!\n", point);
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
        if (count >= 10) {  // 게임 진행 횟수가 10번을 넘어갈 때
            printf("플레이어 패배\n\n");
            if (YN()) {     // 게임 진행할지 종료할지를 결정하기 위한 조건문
                count = 0;
                clientNumber[0] = 0;
                clientNumber[1] = 0;
                clientNumber[2] = 0;
                send(clientSocket, (char*)clientNumber, sizeof(clientNumber), 0); // 새로 배정된 숫자를 client에게 전송한다.
                continue;
            }
            else
                break;
        }
        count++; // 10번이상 진행시, 패배를 결정하기 위한 변수

    }
    closesocket(clientSocket);
}

// 게임을 계속 할지, 종료할지 물어보기 위한 함수
int YN() {
    char yn[5];

    while (1)
    {
        printf("계속 하시겠습니까(y/n)?");
        scanf("%s", yn);
        if (yn[0] == 'y' || yn[0] == 'Y') {
            return 1;
        }
        else if (yn[0] == 'n' || yn[0] == 'N') {
            return 0;
        }
        else {
            printf("잘못 입력하였습니다. 다시 입력하세요!\n");
        }
    }
}