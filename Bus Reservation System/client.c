#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

void loginUser(int sock);
void bookTicket(int sock);
void cancelTicket(int sock);
void checkBusStatus(int sock);
void changePassword(int sock);
void forgotPassword(int sock);

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Socket created\nConnected\n");

    while (1) {
        printf("\n======================= Bus Reservation System  =======================\n");
        printf("1. Login\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loginUser(sock);
                break;
            case 2:
                printf("Exiting the program.\n");
                close(sock);
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

void loginUser(int sock) {
    char username[50], password[50];
    char buffer[MAXLINE] = {0};

    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    snprintf(buffer, MAXLINE, "LOGIN %s %s", username, password);
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, MAXLINE);
    printf("%s\n", buffer);

    if (strstr(buffer, "successful")) {
        while (1) {
            printf("\n=== User Menu ===\n");
            printf("1. Book a Ticket\n");
            printf("2. Cancel a Ticket\n");
            printf("3. Check Bus Status\n");
            printf("4. Change Password\n");
            printf("5. Forgot Password\n");
            printf("6. Logout\n");
            printf("Enter your choice: ");
            int choice;
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    bookTicket(sock);
                    break;
                case 2:
                    cancelTicket(sock);
                    break;
                case 3:
                    checkBusStatus(sock);
                    break;
                case 4:
                    changePassword(sock);
                    break;
                case 5:
                    forgotPassword(sock);
                    break;
                case 6:
                    printf("Logging out.\n");
                    return;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        }
    }
}

void bookTicket(int sock) {
    char busNumber[10], name[50], age[5];
    char buffer[MAXLINE] = {0};

    printf("Enter Bus Number: ");
    scanf("%s", busNumber);
    printf("Enter Name: ");
    scanf("%s", name);
    printf("Enter Age: ");
    scanf("%s", age);

    snprintf(buffer, MAXLINE, "BOOK %s %s %s", busNumber, name, age);
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, MAXLINE);
    printf("%s\n", buffer);
}

void cancelTicket(int sock) {
    char name[50];
    char buffer[MAXLINE] = {0};

    printf("Enter Name to Cancel Ticket: ");
    scanf("%s", name);

    snprintf(buffer, MAXLINE, "CANCEL %s", name);
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, MAXLINE);
    printf("%s\n", buffer);
}

void checkBusStatus(int sock) {
    char busNumber[10];
    char buffer[MAXLINE] = {0};

    printf("Enter Bus Number to Check Status: ");
    scanf("%s", busNumber);

    snprintf(buffer, MAXLINE, "STATUS %s", busNumber);
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, MAXLINE);
    printf("%s\n", buffer);
}

void changePassword(int sock) {
    char username[50], oldPassword[50], newPassword[50];
    char buffer[MAXLINE] = {0};

    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Old Password: ");
    scanf("%s", oldPassword);
    printf("Enter New Password: ");
    scanf("%s", newPassword);

    snprintf(buffer, MAXLINE, "CHANGE_PASSWORD %s %s %s", username, oldPassword, newPassword);
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, MAXLINE);
    printf("%s\n", buffer);
}

void forgotPassword(int sock) {
    char username[50], newPassword[50];
    char buffer[MAXLINE] = {0};

    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter New Password: ");
    scanf("%s", newPassword);

    snprintf(buffer, MAXLINE, "FORGOT_PASSWORD %s %s", username, newPassword);
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, MAXLINE);
    printf("%s\n", buffer);
}

