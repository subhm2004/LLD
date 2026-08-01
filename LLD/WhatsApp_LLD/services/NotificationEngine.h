#ifndef NOTIFICATION_ENGINE_H
#define NOTIFICATION_ENGINE_H

#include <algorithm>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace whatsapp_lld {

class INotification {
public:
    virtual ~INotification() = default;
    virtual std::string getContent() const = 0;
};

class SimpleNotification : public INotification {
private:
    std::string text;

public:
    explicit SimpleNotification(std::string message) : text(std::move(message)) {}
    std::string getContent() const override { return text; }
};

class NotificationDecorator : public INotification {
protected:
    std::shared_ptr<INotification> notification;

public:
    explicit NotificationDecorator(std::shared_ptr<INotification> baseNotification)
        : notification(std::move(baseNotification)) {}
    ~NotificationDecorator() override = default;
};

class TimestampDecorator : public NotificationDecorator {
public:
    explicit TimestampDecorator(std::shared_ptr<INotification> baseNotification)
        : NotificationDecorator(std::move(baseNotification)) {}

    std::string getContent() const override {
        return "[ts:" + std::to_string(std::time(nullptr)) + "] " + notification->getContent();
    }
};

class SignatureDecorator : public NotificationDecorator {
private:
    std::string signature;

public:
    SignatureDecorator(std::shared_ptr<INotification> baseNotification, std::string sign)
        : NotificationDecorator(std::move(baseNotification)), signature(std::move(sign)) {}

    std::string getContent() const override { return notification->getContent() + " -- " + signature; }
};

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update() = 0;
};

class IObservable {
public:
    virtual ~IObservable() = default;
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers() = 0;
};

class NotificationObservable : public IObservable {
private:
    std::vector<IObserver*> observers;
    std::shared_ptr<INotification> currentNotification;

public:
    void addObserver(IObserver* observer) override { observers.push_back(observer); }

    void removeObserver(IObserver* observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notifyObservers() override {
        for (IObserver* observer : observers) {
            observer->update();
        }
    }

    void setNotification(std::shared_ptr<INotification> notification) {
        currentNotification = std::move(notification);
        notifyObservers();
    }

    std::string getNotificationContent() const {
        return currentNotification ? currentNotification->getContent() : "";
    }
};

class INotificationStrategy {
public:
    virtual ~INotificationStrategy() = default;
    virtual void send(const std::string& content) = 0;
};

class EmailStrategy : public INotificationStrategy {
private:
    std::string email;

public:
    explicit EmailStrategy(std::string emailId) : email(std::move(emailId)) {}
    void send(const std::string& content) override {
        std::cout << "[Email:" << email << "] " << content << std::endl;
    }
};

class SMSStrategy : public INotificationStrategy {
private:
    std::string mobileNumber;

public:
    explicit SMSStrategy(std::string number) : mobileNumber(std::move(number)) {}
    void send(const std::string& content) override {
        std::cout << "[SMS:" << mobileNumber << "] " << content << std::endl;
    }
};

class PopUpStrategy : public INotificationStrategy {
public:
    void send(const std::string& content) override {
        std::cout << "[Popup] " << content << std::endl;
    }
};

class NotificationHub {
private:
    NotificationObservable observable;
    NotificationHub() = default;

public:
    NotificationHub(const NotificationHub&) = delete;
    NotificationHub& operator=(const NotificationHub&) = delete;

    static NotificationHub& getInstance() {
        static NotificationHub instance;
        return instance;
    }

    NotificationObservable* getObservable() { return &observable; }

    void sendNotification(const std::shared_ptr<INotification>& notification) {
        observable.setNotification(notification);
    }
};

class LoggerObserver : public IObserver {
private:
    NotificationObservable* observable;

public:
    explicit LoggerObserver(NotificationObservable* notificationObservable)
        : observable(notificationObservable) {
        observable->addObserver(this);
    }

    ~LoggerObserver() override { observable->removeObserver(this); }

    void update() override {
        std::cout << "[Logger] " << observable->getNotificationContent() << std::endl;
    }
};

class NotificationEngine : public IObserver {
private:
    NotificationObservable* observable;
    std::vector<std::unique_ptr<INotificationStrategy>> strategies;

public:
    explicit NotificationEngine(NotificationObservable* notificationObservable)
        : observable(notificationObservable) {
        observable->addObserver(this);
    }

    ~NotificationEngine() override { observable->removeObserver(this); }

    void addNotificationStrategy(std::unique_ptr<INotificationStrategy> strategy) {
        strategies.push_back(std::move(strategy));
    }

    void update() override {
        const std::string content = observable->getNotificationContent();
        for (auto& strategy : strategies) {
            strategy->send(content);
        }
    }
};

}  // namespace whatsapp_lld

#endif
