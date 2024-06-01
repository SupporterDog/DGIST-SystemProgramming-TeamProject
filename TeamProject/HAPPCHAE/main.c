#include "all_header.h"

int main(int argc, char *argv[]) {//MAIN THREAD
    //===================SETIING FOR SERVER CONNECTION===================
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    const char* server_ip = argv[1];
    int server_port = atoi(argv[2]);

    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("Connected to server %s:%d\n", server_ip, server_port);
    //-------------------------SERVER CONNECTION CPMPLETE--------------




    //============THREAD1 FOR RECEIVING DATA FROM SERVER: update DGIST--> MAP and ENEMY=============
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiveUpdates, NULL);
    pthread_join(recv_thread, NULL);

    //============THread2 for CAR_RUN===============================================================
    //pthread_t run_thread;
    //pthread_create(&run_thread,NULL,,NULL);

    //===========Thread3 for claculating MAX_POINT and PATH=========================================
    pthread_t alg_thread;
    pthread_create(&alg_thread,NULL,Run_Algorithm,NULL);
    pthread_join(alg_thread, NULL);


    //============THREAD for QR_CODE RECEVING AND DATA SEND
    pthread_t qr_thread;
    pthread_create(&qr_thread,NULL,decodeQRCodeThread,NULL);
    pthread_join(qr_thread, NULL);

    


    return 0;
}