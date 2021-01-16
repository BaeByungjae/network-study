//2017029743_배병재
 
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
//소켓 프로그래밍에 사용될 헤더파일 선언
 
# define LISTEN_QUEUE_SIZE 5
 
void childHandler(int signal)
{
    
    int status;
    pid_t spid;
 
    // -1 :: 어떠한 child process모두 다 받는다. WNOHANG :: 자식 프로세스가 종료되지 않아도 부모는 자신할 일 한다.
    //종료된 자식프로세스의 pid를 출력한다.
    while((spid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("자식프로세스 종료 \n");
        printf("PID         : %d\n", spid);
    }
}
 
int main(int argc,char* argv[]) {
    
    //childHandler 함수가 SIGCHLD 시그널을 처리할 수 있도록 시그널 설치
 
    signal(SIGCHLD, (void *)childHandler);    
 
    struct sockaddr_in servaddr;
    //서버 소켓의 기본적인 세팅을 한다.
    memset(&servaddr, 0, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    int listenFD = socket(AF_INET, SOCK_STREAM, 0);
    int connectFD;
 
    ssize_t receivedBytes;
    char readBuff[BUFFER_SIZE];
    char sendBuff[BUFFER_SIZE];
    pid_t pid;
 
    //소켓에 주소를 bind한다.
    if (bind(listenFD, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        printf("Can not bind.\n");
        return -1;
    }
    //client를 기다리는 대기열을 만든다 5칸을 만든다.
    if (listen(listenFD, LISTEN_QUEUE_SIZE) == -1) {
        printf("Listen fail.\n");
        return -1;
    }
 
    printf("Waiting for clients...\n");
 
    while (1) 
    {   
        char temp[20];
        //부모프로세스는 클라이언트의 접속을 받는다.
        struct sockaddr_in connectSocket, peerSocket;
 
        socklen_t connectSocketLength = sizeof(connectSocket);
 
        while((connectFD = accept(listenFD, (struct sockaddr*)&connectSocket, (socklen_t *)&connectSocketLength)) >= 0)
        {
            getpeername(connectFD, (struct sockaddr*)&peerSocket, &connectSocketLength);
 
            char clntname[sizeof(peerSocket.sin_addr) + 1] = { 0 };
            sprintf(clntname, "%s", inet_ntoa(peerSocket.sin_addr));
            inet_ntop(AF_INET,&peerSocket.sin_addr.s_addr,temp,sizeof(temp));
            // 접속이 안되었을 때는 출력 x
            if(strcmp(temp,"0.0.0.0") != 0)
                printf("Server: Client %s is connected\n", temp);
        
            if (connectFD < 0)
            {
                printf("Server: accept failed\n");
                exit(0);
            }
            // 클라이언트가 접속할 때마다 fork를 통해 child process를 생성해 echo를 발생.
            // 자식 프로세스는 클라이언트와 연결이 되어 작업을 한다.
            pid = fork();
 
            // 자식 서버일 때
            if(pid == 0)
            {    
                // 리스닝 소켓은 닫아준다.
                close(listenFD);
 
                ssize_t receivedBytes;
 
                // read할 값이 있다면 계속 읽어들인다.
                while((receivedBytes = read(connectFD, readBuff, BUFF_SIZE)) > 0)
                {                
                    readBuff[receivedBytes] = '\0';
                    fputs(readBuff, stdout);
                    fflush(stdout);
                    sprintf(sendBuff,"%s",readBuff);
                    write(connectFD, sendBuff, strlen(sendBuff));
                }
                
                // 클라이언트가 종료되면 해당 자식 프로세스의 커넥팅 소켓도 종료
                if(close(connectFD)==-1){
                    printf("Client %s closed error\n",temp);
                }
                else printf("Server: Client %s is closed\n", temp);
                return 0; // 혹은 exit(0); 이용
    
            }
 
            // 부모 서버라면 커넥팅 소켓을 닫아준다. 
            else
                close(connectFD);
        }
        
    }
    if(close(listenFD)==-1){
        printf("server closed error\n");
    }
    else{
        printf("server closed\n");
    }
 
    return 0;
}
