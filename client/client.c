//2017029743 배병재
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
 
#define BUFFER_SIZE 4096
#define BUFF_SIZE 100 


int main(int argc, char** argv)
{
 
    struct sockaddr_in connectSocket;
    //소켓을 초기화
    memset(&connectSocket, 0, sizeof(connectSocket));
    //소켓의 ip주소와 포트를 설정, 파일 실행시 입력 받는 걸로 설정한다.
    connectSocket.sin_family = AF_INET;
    inet_aton(argv[1], (struct in_addr*) &connectSocket.sin_addr.s_addr);
    connectSocket.sin_port = htons(atoi(argv[2]));
    //소켓 설정
    int connectFD = socket(AF_INET, SOCK_STREAM, 0);
    //바인드 필요없다. 바로 커넥트 시도
    if (connect(connectFD, (struct sockaddr*) &connectSocket, sizeof(connectSocket)) == -1) 
    {
        printf("Can not connect.\n");
        return -1;
    }
    //커넥트 성공시
    else 
    {
        int readBytes, writtenBytes;
        char sendBuffer[BUFFER_SIZE];
        char receiveBuffer[BUFFER_SIZE];
        char *str="quit";
        while (1) 
        {
            //서버에 문자열을 보낸 뒤 서버가 보낸 echo를 받아 출력.
            printf("서버에 보낼 말을 입력하세요 :: ");
            fgets(sendBuffer,BUFF_SIZE,stdin);
            //quit으로 시작하는 문자열을 입력하면 클라이언트를 종료한다.
            if(strncmp(sendBuffer,str,4)==0)break;
            write(connectFD, sendBuffer, strlen(sendBuffer));
            
            readBytes = read(connectFD, receiveBuffer, BUFF_SIZE);
            receiveBuffer[readBytes] = '\0';
            fputs(receiveBuffer, stdout);
            fflush(stdout);
        }
    }
    if(close(connectFD)==-1){
        printf("close error\n");
    }
    else
    {
        printf("client closed\n");
    }
    
    
    return 0;
}    