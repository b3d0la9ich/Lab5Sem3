#ifndef MARKETPLACE_H
#define MARKETPLACE_H

#include <string>
#include <vector>
#include <memory>

// Класс Product
class Product {
public:
    std::string name;
    double price;
    int quantity;
    int sellerId;

    Product(const std::string& name, double price, int quantity, int sellerId)
        : name(name), price(price), quantity(quantity), sellerId(sellerId) {}

    bool operator==(const Product& other) const {
        return name == other.name && price == other.price &&
               quantity == other.quantity && sellerId == other.sellerId;
    }
};

// Базовый класс для стратегий оплаты
class PaymentMethod {
public:
    virtual bool pay(double amount, double& balance) = 0;
    virtual std::string getMethodName() const = 0;
    virtual ~PaymentMethod() {}
};

class CashPayment : public PaymentMethod {
public:
    bool pay(double amount, double& balance) override {
        double discountedAmount = amount * 0.87; // Скидка 13%
        if (balance >= discountedAmount) {
            balance -= discountedAmount;
            return true;
        }
        return false;
    }

    std::string getMethodName() const override {
        return "Cash (13% скидка)";
    }
};

class CardPayment : public PaymentMethod {
public:
    bool pay(double amount, double& balance) override {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    std::string getMethodName() const override {
        return "Card";
    }
};

class CryptoPayment : public PaymentMethod {
public:
    bool pay(double amount, double& balance) override {
        double discountedAmount = amount * 0.8; // Скидка 20%
        if (balance >= discountedAmount) {
            balance -= discountedAmount;
            return true;
        }
        return false;
    }

    std::string getMethodName() const override {
        return "Crypto (20% скидка)";
    }
};

// Класс Customer
class Customer {
public:
    std::string name;
    double balance;

    Customer(const std::string& name, double balance) : name(name), balance(balance) {}

    bool makePayment(double amount, PaymentMethod& paymentMethod) {
        return paymentMethod.pay(amount, balance);
    }
};

// Класс Seller
class Seller {
public:
    std::string name;
    int id;

    Seller(const std::string& name, int id) : name(name), id(id) {}
};

// Класс Marketplace
class Marketplace {
private:
    std::vector<Seller> sellers;
    std::vector<Customer> customers;
    std::vector<Product> products;

public:
    void addSeller(const std::string& name);
    void addCustomer(const std::string& name, double balance);
    void addProduct(int sellerId, const std::string& name, double price, int quantity);
    std::string listProducts() const;
    std::string purchaseProduct(int customerId, const std::string& productName, int quantity, int paymentMethodChoice);
    void clearProducts(); // Метод для удаления всех товаров
};

#endif
