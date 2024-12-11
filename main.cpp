#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <cstdlib> // Для rand() и srand()
#include <ctime>   // Для time()
#include "Marketplace.h"

std::mutex db_mutex;

void handle_client(int client_socket, Marketplace& marketplace) {
    char buffer[1024] = {0};
    while (true) {
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            std::cerr << "Клиент отключился\n";
            break;
        }

        if (rand() % 5 == 0) { // 20% вероятность
            std::cout << "Грабеж активирован! Все товары будут удалены." << std::endl;
            marketplace.clearProducts();
            std::string robbery_message = "Магазин ограблен! Все товары исчезли.\n";
            send(client_socket, robbery_message.c_str(), robbery_message.size(), 0);
            memset(buffer, 0, sizeof(buffer));
            continue;
        }


        std::string command(buffer);
        command.erase(command.find_last_not_of("\n\r") + 1);
        std::cout << "Получено: " << command << std::endl;

        std::string response;
        {
            std::lock_guard<std::mutex> lock(db_mutex);
            if (command == "LIST_PRODUCTS") {
                response = marketplace.listProducts();
            } else if (command.find("ADD_SELLER") == 0) {
                std::string name = command.substr(11);
                marketplace.addSeller(name);
                response = "Продавец добавлен\n";
            } else if (command.find("ADD_CUSTOMER") == 0) {
                std::istringstream iss(command.substr(13));
                std::string name;
                double balance;
                iss >> name >> balance;
                marketplace.addCustomer(name, balance);
                response = "Покупатель добавлен\n";
            } else if (command.find("ADD_PRODUCT") == 0) {
                std::istringstream iss(command.substr(12));
                int sellerId;
                std::string name;
                double price;
                int quantity;
                iss >> sellerId >> name >> price >> quantity;
                marketplace.addProduct(sellerId, name, price, quantity);
                response = "Товар добавлен\n";
            } else if (command.find("BUY") == 0) {
                std::istringstream iss(command.substr(4));
                int customerId;
                std::string productName;
                int quantity;
                int paymentMethodChoice;
                iss >> customerId >> productName >> quantity >> paymentMethodChoice;
                response = marketplace.purchaseProduct(customerId, productName, quantity, paymentMethodChoice);
            } else {
                response = "Неверная команда\n";
            }
        }

        send(client_socket, response.c_str(), response.size(), 0);
        memset(buffer, 0, sizeof(buffer));
    }
    close(client_socket);
}

int main() {
    srand(time(nullptr));
    Marketplace marketplace;

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Ошибка настройки сокета");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(7432);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Ошибка прослушивания");
        exit(EXIT_FAILURE);
    }

    std::cout << "Сервер запущен. Ожидание подключения клиентов...\n";

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Ошибка подключения клиента");
            continue;
        }
        std::cout << "Клиент подключился\n";
        std::thread(handle_client, new_socket, std::ref(marketplace)).detach();
    }

    return 0;
}
