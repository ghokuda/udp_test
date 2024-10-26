#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1472//1024
#define NUM_SENDS 10

int main(int argc, char *argv[]) {    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <output_file> <output_file_time>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    int recv_cnt;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    const char *output_file = argv[1];
    const char *output_file_time = argv[2];
    
    // ソケットを作成
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // アドレス再利用オプションの有効化
    int reuse_enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_enable, sizeof(reuse_enable)) < 0) {
        perror("Failed to set reuse address option");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // サーバアドレスの設定
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // バインド
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is running on port %d\n", PORT);

    // ファイルをオープン
    FILE *data_file = fopen(output_file, "a");
    FILE *time_file = fopen(output_file_time, "a");
    if (!data_file || !time_file) {
        perror("File opening failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // クライアントからのメッセージを受信
    recv_cnt = 0;
    while (1) {
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);

        // データをファイルに書き出し
        fprintf(data_file, "%s\n", buffer);

        // 現在の時刻を取得
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        long milliseconds = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

        // 時刻をファイルに書き出し
        fprintf(time_file, "%ld\n", milliseconds);

        // 受信回数をカウント
        recv_cnt++;
        if(recv_cnt >= NUM_SENDS){
            break;
        }
    }

    // ファイルを閉じる
    fclose(data_file);
    fclose(time_file);
    close(sockfd);
    printf("Server closed.\n");

    return 0;
}
