#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
//#define SERVER_IP "255.255.255.255"
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1472//1024
#define NUM_SENDS 10
#define INTERVAL_MS 500 // 500ミリ秒間隔で送信

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // ソケットを作成
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // ブロードキャストオプションの有効化
    int broadcast_enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        perror("Failed to set broadcast option");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
 
     // サーバアドレスの設定
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 入力ファイルを開く
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("File opening failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // データを送信
    for (int i = 0; i < NUM_SENDS; i++) {
        // ファイルからデータを読み込む
        if (fgets(buffer, BUFFER_SIZE, file) != NULL) {
            // 改行文字を取り除く
            buffer[strcspn(buffer, "\n")] = 0;

            // メッセージをサーバに送信
            sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
            printf("Sent: %s\n", buffer);

            // 定周期で送信 (ミリ秒をマイクロ秒に変換)
            usleep(INTERVAL_MS * 1000); // msをμsに変換
        } else {
            printf("Not enough data in the file. Stopping early.\n");
            break; // ファイルの終わりに達した場合
        }
    }

    // ファイルを閉じる
    fclose(file);
    close(sockfd);
    printf("Client closed after sending messages.\n");
    return 0;
}
