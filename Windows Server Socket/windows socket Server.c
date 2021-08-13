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

unsigned threadID;  // 쓰레드의 ID 를 저장할 변수

int clientCnt = 0;  // 접속한 client 수
SOCKET clientArray[MAX_CLNT];  // 접속한 client 배열
HANDLE Mutalisk;  // Mutex 

int main() {
    int clntAdrSize;
    WSADATA wsadata;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddress, clintAddress;

    // winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        ErrorHandling("WSAStartup() error!");

    // Mutex 생성
    Mutalisk = CreateMutex(NULL, FALSE, NULL);
    // Server Socket 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    printf("서버 소켓을 생성 했습니다.\n");

    ZeroMemory(&serverAddress, 0, sizeof(serverAddress)); // serverAddress 구조체를 0으로 초기화
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);	    // serverAddress에 IP 대입
    serverAddress.sin_family = AF_INET;		    // 주소 타입 설정
    serverAddress.sin_port = htons(PORT);		    // 클라이언트와 메시지를 주고 받을 포트 번호

     // 서버 소켓에 주소정보 설정
    if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    // 클라이언트의 접속을 기다리는 대기모드로 전환
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    printf("클라이언트의 접속을 기다립니다......\n");

    srand((unsigned)time(NULL));    // 난수를 생성하기 위한 초기화

    while (1) {
        clntAdrSize = sizeof(clintAddress);

        // 클라이언트의 접속을 accept()하고 클라이언트와 데이터를 주고받는 클라이언트 소켓생성
        clientSocket = accept(serverSocket, (SOCKADDR*)&clintAddress, &clntAdrSize);

        WaitForSingleObject(Mutalisk, INFINITE);
        clientArray[clientCnt++] = clientSocket;
        ReleaseMutex(Mutalisk);

        // 멀티 쓰레드로 숫자야구 게임을 시작하고, 함수의 인자로 clientSocket을 전달함
        unsigned long hThread = _beginthreadex(NULL, 0, playBaseballGame, (void*)clientSocket, 0, &threadID);
        printf("클라이언트가 접속 했습니다.\n");
        printf("접속한 클라이언트 IP: %s \n", inet_ntoa(clintAddress.sin_addr));
        printf("야구 게임 시작! \n");
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;;
}

/* ㅎㅏ ㅅ  ㄱ  ㅎㅕ
    ㅁ  ㅜ  ㅜ   ㄴ  */

void ErrorHandling(char* msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
void initBaseBall(int solutionNumber[]) {   // 숫자야구 문제 초기화
    do {
        // 난수를 생성 후 난수를 10으로 나눈 나머지를 대입
        solutionNumber[0] = rand() % 10;
        solutionNumber[1] = rand() % 10;
        solutionNumber[2] = rand() % 10;
    }
    // 배열에 같은 수가 없을 때까지 반복함
    while ((solutionNumber[0] == solutionNumber[1]) | (solutionNumber[1] == solutionNumber[2]) | (solutionNumber[2] == solutionNumber[0]));
    printf("서버 숫자 --> %d  %d  %d\n", solutionNumber[0], solutionNumber[1], solutionNumber[2]);
}

unsigned __stdcall  playBaseballGame(void* arg) {
    int solutionNumber[3];
    initBaseBall(solutionNumber);   // 숫자야구 게임 초기화

    int clientSocket = (int)arg;
    int score = 0;   // 점수를 제공하기 위한 변수

    // Strike가 3이 되면 무한루프 탈출
    while (1) {
        int clientAnswerNumber[3];  // 클라이언트가 입력한 세 개의 숫자를 대입할 배열
        int scoringData[2];         // Server가 답안지를 채점하여 client에게 보낼 data 배열
        int toClientPoint[1];       // 정답 시, 클라이언트에게 부여(전송)할 점수(100점)를 저장할 배열
        int changeGame = 1;

        // 클라이언트가 입력한 세 개의 숫자를 읽어 들여서 clientAnswerNumber에 대입
        int ret = recv(clientSocket, (char*)clientAnswerNumber, sizeof(clientAnswerNumber), 0);

        // #1. 클라이언트가 게임 종료를 원할 때, 0 전송
        if (ret <= 0) {
            printf("클라이언트와 접속이 종료됐습니다.\n");
            break;
        }
        // #2. 클라이언트가 게임 재생성을 원할 때, 0 0 0 전송
        if ((clientAnswerNumber[0] == 0 && clientAnswerNumber[1] == 0) && (clientAnswerNumber[1] == 0 && clientAnswerNumber[2] == 0)) {
            initBaseBall(solutionNumber);
            continue;
        }

        // Network Byte 정렬된 숫자를 Host Byte로 재정렬
        for (int index = 0; index < 3; index++)
            clientAnswerNumber[index] = ntohl(clientAnswerNumber[index]);

        // 클라이언트가 입력한 수를 출력
        printf("클라이언트가 입력한 수 --> %d  %d  %d\n", clientAnswerNumber[0], clientAnswerNumber[1], clientAnswerNumber[2]);
        int strike = 0;   // Strike의 수를 대입할 변수
        int ball = 0;     // Ball의 수를 대입할 변수

        for (int i = 0; i < 3; i++) {
            // 서버의 숫자가 저장된 solutionNumber에서 i번째 숫자를 꺼내서 oneSolNumber에 대입
            int oneSolNumber = solutionNumber[i];
            for (int j = 0; j < 3; j++) {
                // 클라이언트에서 전송한 숫자가 저장된 clientAnswerNumber의 j번째 숫자를 꺼내서 oneClientNumber 에 대입
                int oneClientNumber = clientAnswerNumber[j];

                if (oneSolNumber == oneClientNumber) {
                    if (i == j)
                        strike++;  // 위치와 숫자 모두 같을 때 Strike 1증가
                    else
                        ball++;    // 숫자만 같을 때 Ball 1증가
                }   // if
            }   // for_j
        }   // for_i

        printf("%d 스트라이크  %d 볼\n\n", strike, ball);

        // 채점 data를 클라이언트로 전송하기 위해서 네트워크 바이트로 정렬 후 toClientScore에 대입
        scoringData[0] = htonl(strike);
        scoringData[1] = htonl(ball);

        send(clientSocket, (char*)scoringData, sizeof(scoringData), 0);

        // 정답 시 게임 종료
        if (strike == 3) {
            score += 100;
            toClientPoint[0] = htonl(score);
            send(clientSocket, (char*)toClientPoint, sizeof(toClientPoint), 0);
        }

        WaitForSingleObject(Mutalisk, INFINITE);
        for (int k = 0; k < clientCnt; k++) {  // close한 소켓을 배열에서 제거
            if (clientSocket == clientArray[k]) {
                while (k++ < clientCnt - 1)
                    clientArray[k] = clientArray[k + 1];
                break;
            }
        }
        clientCnt--;
        ReleaseMutex(Mutalisk);
    }  // while
    closesocket(clientSocket);  //클라이언트 소켓 종료
    return 0;
}