#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

// 链接 Winsock 库
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8888"
#define DEFAULT_BUFLEN 512

int main() {
    // 1. 初始化 Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    printf("Winsock initialized.\n");

    // 2. 创建监听套接字
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Listen socket created.\n");

    // 3. 准备服务器地址结构体
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)atoi(DEFAULT_PORT)); // 使用 htons 转换端口号
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 监听所有网络接口

    // 4. 绑定套接字
    iResult = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Socket bound to address.\n");

    // 5. 开始监听
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Server is listening on port %s...\n", DEFAULT_PORT);

    // 6. 接受客户端连接 (阻塞)
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Client connected.\n");

    // 关闭监听套接字，因为我们只服务一个客户端
    closesocket(listenSocket);

    // 7. 接收和发送数据
    char recvBuf[DEFAULT_BUFLEN];
    int bytesReceived;

    do {
        bytesReceived = recv(clientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        if (bytesReceived > 0) {
            recvBuf[bytesReceived] = '\0'; // 手动添加字符串结束符
            printf("Received: %s\n", recvBuf);

            char sendBuf[1024];
            // 构造回复字符串
            sprintf(sendBuf, "%s reply from server", recvBuf); 

            // 发送回复
            int iSendResult = send(clientSocket, sendBuf, (int)strlen(sendBuf), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                break;
            }
             printf("Replied: %s\n", sendBuf);
        } else if (bytesReceived == 0) {
            printf("Connection closing...\n");
        } else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            break;
        }
    } while (bytesReceived > 0);

    // 8. 清理
    closesocket(clientSocket);
    WSACleanup();
    printf("Server shut down.\n");

    return 0;
}