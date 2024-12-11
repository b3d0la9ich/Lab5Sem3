#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Не удалось создать сокет\n";
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(7432);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        cerr << "Неправильный адрес\n";
        close(clientSocket);
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Соединение не принято\n";
        close(clientSocket);
        return 1;
    }

    char buffer[1024] = {0};
    while (true) {
        cout << "Введите команду: ";
        string command;
        getline(cin, command);
        send(clientSocket, command.c_str(), command.length(), 0);
        int valread = read(clientSocket, buffer, 1024);
        if (valread <= 0) {
            cerr << "Сервер отключился\n";
            break;
        }
        cout << "Ответ сервера: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer));
    }
    close(clientSocket);
    return 0;
}
