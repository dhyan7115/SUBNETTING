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
void validate_IP(char* ip, int arr[]) {
    int default_mask = 0;
    char class;
    char* a = ".";
    char* token = strtok(ip, a);
    int flag = 1;
    int index = 0;
    while (token != NULL) {
        int num = atoi(token);
        if (num <= 255 && num >= 0) {
            if (num < 128) {
                class = 'A';
                default_mask = 8;
            }
            else if (num > 127 && num <= 191) {
                class = 'B';
                default_mask = 16;
            }
            else if (num > 191 && num <= 223) {
                class = 'C';
                default_mask = 24;
            }
            else if (num > 223 && num <= 239) {
                class = 'D';
            }
            else {
                class = 'E';
            }
            arr[index++] = num;
        }
        else {
            flag = 0;
        }
        token = strtok(NULL, a);
    }
    if (flag == 0) {
        printf("Invalid IP\n");
        exit(1);
    }
    else {
        printf("The given IP is Correct\n");
        printf("Class of the Given IP : %c\n", class);
        printf("Default Network Mask : %d\n", default_mask);
    }
}

int power_of_2(int n) {
    if (n == 0) {
        return 1;
    }
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

int power(int a) {
    int p = 0;
    while (a > 1) {
        a >>= 1;
        p++;
    }
    return p;
}

int main() {
    int num_subnets;
    int ip[4];
    char buffer[BUFFER_SIZE];
    int server_socket;
    int client_server;
    struct sockaddr_in address;
    int subnet_mask;
    socklen_t addr_len = sizeof(address);
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);
    
    bind(server_socket, (struct sockaddr*)&address, addr_len);
    listen(server_socket, 10);
    printf("Server is Listening to Port %d\n", PORT);
    
    client_server = accept(server_socket, (struct sockaddr*)&address, &addr_len);
    printf("Connected to the Client\n");
    
    int bytes = recv(client_server, buffer, BUFFER_SIZE, 0);
    validate_IP(buffer, ip);
    
    bytes = recv(client_server, &num_subnets, sizeof(num_subnets), 0);
    printf("Number of Subnets = %d\n", num_subnets);
    
    int size[num_subnets];
    printf("Subnet Sizes: ");
    for (int i = 0; i < num_subnets; i++) {
        bytes = recv(client_server, &size[i], sizeof(int), 0);  // Correct size
        printf("%d ", size[i]);
    }
    printf("\n");
    
    printf("Number of Addresses Alloted to each Subnet: ");
    for (int i = 0; i < num_subnets; i++) {
        size[i] = power_of_2(size[i]);
        printf("%d ", size[i]);
    }
    printf("\n");
    
    int current_ip[4] = {ip[0], ip[1], ip[2], ip[3]};
    for (int i = 0; i < num_subnets; i++) {
        subnet_mask = 32 - power(size[i]);
        
        int subnet_start = current_ip[3];
        int subnet_end = current_ip[3] + size[i] - 1;

        // Adjust the subnet end if it exceeds 255
        while (subnet_end > 255) {
            int overflow = subnet_end - 255;
            current_ip[3] = 255;
            current_ip[2]++;
            subnet_end = overflow - 1;
        }

        // Print the IP range for the subnet
        printf("IP Range for Subnet-%d: %d.%d.%d.%d - %d.%d.%d.%d\n",
               i + 1, current_ip[0], current_ip[1], current_ip[2], subnet_start,
               current_ip[0], current_ip[1], current_ip[2], subnet_end);

        // Update the current_ip for the next subnet
        current_ip[3] = subnet_end + 1;

        // Handle overflow in octets
        while (current_ip[3] > 255) {
            current_ip[3] -= 256;
            current_ip[2]++;
        }

        // Handle overflow in second octet
        while (current_ip[2] > 255) {
            current_ip[2] -= 256;
            current_ip[1]++;
        }

        // Handle overflow in first octet
        while (current_ip[1] > 255) {
            current_ip[1] -= 256;
            current_ip[0]++;
        }
    }
    close(server_socket);
    close(client_server);
    return 0;
}
