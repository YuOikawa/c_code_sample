#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <unistd.h>

#define BUF_LEN 256

int main(int argc, char **argv) {

    // ソケットのためのファイルディスクリプタ
    int s;

    // IPアドレスの解決
    struct addrinfo hints, *res;
    struct in_addr addr;
    int err;

    // サーバに送るHTTPプロトコル用バッファ
    char send_buf[BUF_LEN];

    //char host[BUF_LEN] = "css-eblog.com";
    //char path[BUF_LEN] = "/";
    //char query[BUF_LEN] = "";
    //char port[BUF_LEN] = "80";
    char host[BUF_LEN] = "localhost";
    char path[BUF_LEN] = "/solr/#";
    char query[BUF_LEN] = "";
    char port[BUF_LEN] = "8983";

    printf("http://%s%s%s を取得します。\n\n", host, path, query);

    // 0クリア
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family   = AF_INET;
    char *getport  = port;

    if ((err = getaddrinfo(host, getport, &hints, &res)) != 0) {
        printf("error %d\n", err);
        return 1;
    }

    // ソケット生成
    if ((s = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        fprintf(stderr, "ソケットの生成に失敗しました。\n");
        return 1;
    }

    // サーバに接続
    if (connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        fprintf(stderr, "connectに失敗しました。\n");
        return 1;
    }

    // HTTPプロトコルの開始 ＆サーバに送信
    sprintf(send_buf, "GET %s%s HTTP/1.0\r\n", path, query);
    write(s, send_buf, strlen(send_buf));

    sprintf(send_buf, "Host: %s:%d\r\n", host, port);
    write(s, send_buf, strlen(send_buf));

    sprintf(send_buf, "\r\n");
    write(s, send_buf, strlen(send_buf));

    // 受信が終わるまでループ
    while(1) {
        char buf[BUF_LEN];
        int read_size;
        read_size = read(s, buf, BUF_LEN);

        if (read_size > 0) {
            write(1, buf, read_size);
        }
        else {
            break;
        }
    }

    // ソケットを閉じる
    close(s);

    return 0;
}
