
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include "player.h"
#include "chlng.h"

#define PORT 1234
#define BACKLOG 10

typedef enum { MODE_BLOCKING, MODE_THREAD, MODE_FORK } server_mode_t;

server_mode_t mode = MODE_BLOCKING;

void handle_client_thread(int client_fd);

void* client_thread_function(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);
    handle_client_thread(client_fd);
    close(client_fd);
    return NULL;
}

void handle_client_thread(int client_fd) {
    player_t* player = player_new();
    char* msg;
    player_get_greeting(player, &msg);
    send(client_fd, msg, strlen(msg), 0);
    free(msg);

    while (!player->finished) {
        player_get_challenge(player, &msg);
        send(client_fd, msg, strlen(msg), 0);
        free(msg);
        char buffer[256] = {0};
        int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;

        buffer[n] = '\0';
        player_post_challenge(player, buffer, &msg);
        send(client_fd, msg, strlen(msg), 0);
        free(msg);
    }
    player_del(player);
    close(client_fd);
}



int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr.s_addr = INADDR_ANY };
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, BACKLOG);
    printf("Server listening on port %d\n", PORT);
    while (1) {
        int* client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, NULL, NULL);
        pthread_t thread;
        pthread_create(&thread, NULL, client_thread_function, client_fd);
    }

    return 0;
}
