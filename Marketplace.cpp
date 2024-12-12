#include "Marketplace.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>


using namespace std;

void Marketplace::addSeller(const std::string& name) {
    sellers.emplace_back(name, sellers.size() + 1);
}

void Marketplace::addCustomer(const std::string& name, double balance) {
    customers.emplace_back(name, balance);
}

void Marketplace::addProduct(int sellerId, const std::string& name, double price, int quantity) {
    if (sellerId <= 0 || sellerId > sellers.size()) {
        throw std::runtime_error("Некорректный ID продавца");
    }
    products.emplace_back(name, price, quantity, sellerId);
}

std::string Marketplace::listProducts() const {
    std::ostringstream oss;
    for (const auto& product : products) {
        oss << "Название: " << product.name
            << ", Цена: " << std::fixed << std::setprecision(2) << product.price
            << ", Количество: " << product.quantity
            << ", ID продавца: " << product.sellerId << "\n";
    }
    return oss.str();
}

std::string Marketplace::purchaseProduct(int customerId, const std::string& productName, int quantity, int paymentMethodChoice) {
    if (customerId <= 0 || customerId > customers.size()) {
        return "Некорректный ID покупателя\n";
    }
    Customer& customer = customers[customerId - 1];

    for (auto& product : products) {
        if (product.name == productName && product.quantity >= quantity) {
            double totalCost = product.price * quantity;
            std::unique_ptr<PaymentMethod> paymentMethod;
            if (paymentMethodChoice == 1) {
                paymentMethod = std::make_unique<CashPayment>();
            } else if (paymentMethodChoice == 2) {
                paymentMethod = std::make_unique<CardPayment>();
            } else if (paymentMethodChoice == 3) {
                paymentMethod = std::make_unique<CryptoPayment>();
            } else {
                return "Неверный метод оплаты\n";
            }

            if (customer.makePayment(totalCost, *paymentMethod)) {
                product.quantity -= quantity;
                if (product.quantity == 0) {
                    products.erase(std::remove(products.begin(), products.end(), product), products.end());
                }

                // Формируем чек
                std::ostringstream receipt;
                receipt << "Покупка успешна!\n";
                receipt << "----------------------------\n";
                receipt << "ЧЕК:\n";
                receipt << "Покупатель: " << customer.name << "\n";
                receipt << "Товар: " << productName << "\n";
                receipt << "Количество: " << quantity << "\n";
                receipt << "Общая стоимость: " << std::fixed << std::setprecision(2) << totalCost << "\n";
                receipt << "Метод оплаты: " << paymentMethod->getMethodName() << "\n";
                receipt << "Оставшийся баланс: " << std::fixed << std::setprecision(2) << customer.balance << "\n";
                receipt << "----------------------------\n";

                return receipt.str();
            } else {
                return "Недостаточно средств\n";
            }
        }
    }
    return "Товар не найден или недостаточно на складе\n";
}


void Marketplace::clearProducts() {
    products.clear();
    cout << "Все товары удалены." << std::endl; // Для отладки
}
