#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "list.h"

#define PORT 9034
#define ACK "ACK: "
#define MAX_COMMAND_LINE_LEN 1024

static list_t *g_list = NULL;

void handle_sigint(int sig) {
    (void)sig;
    if (g_list) {
        list_free(g_list);
        g_list = NULL;
    }
    printf("\nServer shutting down gracefully...\n");
    exit(0);
}

int main(int argc, char const *argv[]) {
    (void)argc;
    (void)argv;

    signal(SIGINT, handle_sigint);

    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if (servSockD < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(servSockD, 1) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    int clientSocket = accept(servSockD, NULL, NULL);
    if (clientSocket < 0) {
        perror("accept");
        exit(1);
    }

    g_list = list_alloc();
    if (!g_list) {
        fprintf(stderr, "list_alloc failed\n");
        exit(1);
    }

    char buf[MAX_COMMAND_LINE_LEN];
    char sbuf[MAX_COMMAND_LINE_LEN];
    char *token;

    while (1) {
        memset(buf, 0, sizeof(buf));
        int n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';

        memset(sbuf, 0, sizeof(sbuf));
        token = strtok(buf, " ");
        if (!token) continue;

        if (strcmp(token, "exit") == 0) {
            list_free(g_list);
            g_list = NULL;
            close(clientSocket);
            close(servSockD);
            exit(0);
        }
        else if (strcmp(token, "print") == 0) {
            snprintf(sbuf, sizeof(sbuf), "%s", listToString(g_list));
        }
        else if (strcmp(token, "get_length") == 0) {
            int len = list_length(g_list);
            snprintf(sbuf, sizeof(sbuf), "Length = %d", len);
        }
        else if (strcmp(token, "add_back") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                int val = atoi(token);
                list_add_to_back(g_list, val);
                snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
            } else {
                snprintf(sbuf, sizeof(sbuf), "ERROR: missing value");
            }
        }
        else if (strcmp(token, "add_front") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                int val = atoi(token);
                list_add_to_front(g_list, val);
                snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
            } else {
                snprintf(sbuf, sizeof(sbuf), "ERROR: missing value");
            }
        }
        else if (strcmp(token, "add_position") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                int idx = atoi(token);
                token = strtok(NULL, " ");
                if (token) {
                    int val = atoi(token);
                    list_add_at_index(g_list, idx, val);
                    snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
                } else {
                    snprintf(sbuf, sizeof(sbuf), "ERROR: missing value");
                }
            } else {
                snprintf(sbuf, sizeof(sbuf), "ERROR: missing index");
            }
        }
        else if (strcmp(token, "remove_back") == 0) {
            int val = list_remove_from_back(g_list);
            snprintf(sbuf, sizeof(sbuf), "%d", val);
        }
        else if (strcmp(token, "remove_front") == 0) {
            int val = list_remove_from_front(g_list);
            snprintf(sbuf, sizeof(sbuf), "%d", val);
        }
        else if (strcmp(token, "remove_position") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                int idx = atoi(token);
                int val = list_remove_at_index(g_list, idx);
                snprintf(sbuf, sizeof(sbuf), "%d", val);
            } else {
                snprintf(sbuf, sizeof(sbuf), "ERROR: missing index");
            }
        }
        else if (strcmp(token, "get") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                int idx = atoi(token);
                int val = list_get_elem_at(g_list, idx);
                snprintf(sbuf, sizeof(sbuf), "%d", val);
            } else {
                snprintf(sbuf, sizeof(sbuf), "ERROR: missing index");
            }
        }
        else {
            snprintf(sbuf, sizeof(sbuf), "ERROR: unknown command");
        }

        send(clientSocket, sbuf, strlen(sbuf) + 1, 0);
    }

    if (g_list) list_free(g_list);
    close(clientSocket);
    close(servSockD);
    return 0;
}