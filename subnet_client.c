#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define PORT 7000
#define BUFFER_SIZE 1024
int main() {
    int sock;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
        printf("Invalid IP\n");
        exit(1);
    }
    if (connect(sock, (struct sockaddr*)&address, addr_len) < 0) {
        printf("Error in the Connection\n");
        exit(1);
    }
    printf("Connection Established with Server\n");
    printf("Enter the IP Address: ");
    char buffer[BUFFER_SIZE];
    int n;
    scanf("%s", buffer);
    send(sock, buffer, strlen(buffer), 0);
    printf("Enter the number of Subnets: ");
    scanf("%d", &n);
    send(sock, &n, sizeof(int), 0);
    for (int i = 0; i < n; i++) {
        printf("Enter the Size of Subnet-%d: ", i + 1);
        int a;
        scanf("%d", &a);
        send(sock, &a, sizeof(int), 0);
    }
    close(sock);
    return 0;
}
