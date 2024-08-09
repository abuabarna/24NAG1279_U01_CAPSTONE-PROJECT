#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

typedef struct {
    char username[50];
    char password[50];
} User;

typedef struct {
    int busNumber;
    char source[50];
    char destination[50];
    int totalSeats;
    int availableSeats;
    float fare;
} Bus;

User users[] = {
    {"user1", "password1"},
    {"user2", "password2"}
};

Bus buses[] = {
    {101, "City A", "City B", 50, 50, 25.00}
};

void handleClient(int new_socket);
int authenticateUser(char *username, char *password);
void bookTicket(int new_socket, char *busNumber, char *name, char *age);
void cancelTicket(int new_socket, char *name);
void checkBusStatus(int new_socket, char *busNumber);
void changePassword(int new_socket, char *username, char *oldPassword, char *newPassword);
void forgotPassword(int new_socket, char *username, char *newPassword);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        handleClient(new_socket);
    }

    return 0;
}

void handleClient(int new_socket) {
    char buffer[MAXLINE] = {0};
    int valread;
    char *token;
    char *command;
    char *username, *password, *oldPassword, *newPassword, *busNumber, *name, *age;

    while ((valread = read(new_socket, buffer, MAXLINE)) > 0) {
        buffer[valread] = '\0';

        command = strtok(buffer, " ");

        if (strcmp(command, "LOGIN") == 0) {
            username = strtok(NULL, " ");
            password = strtok(NULL, " ");
            if (authenticateUser(username, password)) {
                send(new_socket, "Login successful. Welcome!", strlen("Login successful. Welcome!"), 0);
            } else {
                send(new_socket, "Login failed. Please check your username and password.", strlen("Login failed. Please check your username and password."), 0);
            }
        } else if (strcmp(command, "BOOK") == 0) {
            busNumber = strtok(NULL, " ");
            name = strtok(NULL, " ");
            age = strtok(NULL, " ");
            bookTicket(new_socket, busNumber, name, age);
        } else if (strcmp(command, "CANCEL") == 0) {
            name = strtok(NULL, " ");
            cancelTicket(new_socket, name);
        } else if (strcmp(command, "STATUS") == 0) {
            busNumber = strtok(NULL, " ");
            checkBusStatus(new_socket, busNumber);
        } else if (strcmp(command, "CHANGE_PASSWORD") == 0) {
            username = strtok(NULL, " ");
            oldPassword = strtok(NULL, " ");
            newPassword = strtok(NULL, " ");
            changePassword(new_socket, username, oldPassword, newPassword);
        } else if (strcmp(command, "FORGOT_PASSWORD") == 0) {
            username = strtok(NULL, " ");
            newPassword = strtok(NULL, " ");
            forgotPassword(new_socket, username, newPassword);
        }

        memset(buffer, 0, MAXLINE);
    }

    close(new_socket);
}

int authenticateUser(char *username, char *password) {
    for (int i = 0; i < sizeof(users) / sizeof(users[0]); i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

void bookTicket(int new_socket, char *busNumber, char *name, char *age) {
    int busNum = atoi(busNumber);
    for (int i = 0; i < sizeof(buses) / sizeof(buses[0]); i++) {
        if (buses[i].busNumber == busNum && buses[i].availableSeats > 0) {
            buses[i].availableSeats--;
            char response[MAXLINE];
            snprintf(response, MAXLINE, "Ticket booked successfully! Seat Number: %d", buses[i].totalSeats - buses[i].availableSeats);
            send(new_socket, response, strlen(response), 0);
            return;
        }
    }
    send(new_socket, "Failed to book ticket.", strlen("Failed to book ticket."), 0);
}

void cancelTicket(int new_socket, char *name) {
    // Simulate cancellation
    for (int i = 0; i < sizeof(buses) / sizeof(buses[0]); i++) {
        if (buses[i].availableSeats < buses[i].totalSeats) {
            buses[i].availableSeats++;
            send(new_socket, "Ticket canceled successfully!", strlen("Ticket canceled successfully!"), 0);
            return;
        }
    }
    send(new_socket, "Failed to cancel ticket.", strlen("Failed to cancel ticket."), 0);
}

void checkBusStatus(int new_socket, char *busNumber) {
    int busNum = atoi(busNumber);
    for (int i = 0; i < sizeof(buses) / sizeof(buses[0]); i++) {
        if (buses[i].busNumber == busNum) {
            char response[MAXLINE];
            snprintf(response, MAXLINE, "Bus Number: %d\nSource: %s\nDestination: %s\nTotal Seats: %d\nAvailable Seats: %d\nFare: %.2f",
                     buses[i].busNumber, buses[i].source, buses[i].destination, buses[i].totalSeats, buses[i].availableSeats, buses[i].fare);
            send(new_socket, response, strlen(response), 0);
            return;
        }
    }
    send(new_socket, "Bus not found.", strlen("Bus not found."), 0);
}

void changePassword(int new_socket, char *username, char *oldPassword, char *newPassword) {
    for (int i = 0; i < sizeof(users) / sizeof(users[0]); i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, oldPassword) == 0) {
            strcpy(users[i].password, newPassword);
            send(new_socket, "Password changed successfully.", strlen("Password changed successfully."), 0);
            return;
        }
    }
    send(new_socket, "Failed to change password.", strlen("Failed to change password."), 0);
}

void forgotPassword(int new_socket, char *username, char *newPassword) {
    for (int i = 0; i < sizeof(users) / sizeof(users[0]); i++) {
        if (strcmp(users[i].username, username) == 0) {
            strcpy(users[i].password, newPassword);
            send(new_socket, "Password reset successfully.", strlen("Password reset successfully."), 0);
            return;
        }
    }
    send(new_socket, "Failed to reset password.", strlen("Failed to reset password."), 0);
}

