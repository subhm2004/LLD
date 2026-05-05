#ifndef NOTIFICATION_ENGINE_H
#define NOTIFICATION_ENGINE_H

#include <algorithm>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace notification_lld {

class INotification {
public:
    virtual ~INotification() = default;
    virtual std::string getContent() const = 0;
};

class SimpleNotification : public INotification {
private:
    std::string text_;

public:
    explicit SimpleNotification(std::string message) : text_(std::move(message)) {}
    std::string getContent() const override { return text_; }
};

class NotificationDecorator : public INotification {
protected:
    std::shared_ptr<INotification> notification_;

public:
    explicit NotificationDecorator(std::shared_ptr<INotification> baseNotification)
        : notification_(std::move(baseNotification)) {}
    ~NotificationDecorator() override = default;
};

class TimestampDecorator : public NotificationDecorator {
public:
    explicit TimestampDecorator(std::shared_ptr<INotification> baseNotification)
        : NotificationDecorator(std::move(baseNotification)) {}

    std::string getContent() const override {
        return "[ts:" + std::to_string(std::time(nullptr)) + "] " + notification_->getContent();
    }
};

class SignatureDecorator : public NotificationDecorator {
private:
    std::string signature_;

public:
    SignatureDecorator(std::shared_ptr<INotification> baseNotification, std::string signature)
        : NotificationDecorator(std::move(baseNotification)), signature_(std::move(signature)) {}

    std::string getContent() const override { return notification_->getContent() + " -- " + signature_; }
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
    std::vector<IObserver*> observers_;
    std::shared_ptr<INotification> currentNotification_;

public:
    void addObserver(IObserver* observer) override { observers_.push_back(observer); }

    void removeObserver(IObserver* observer) override {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }

    void notifyObservers() override {
        for (IObserver* observer : observers_) {
            observer->update();
        }
    }

    void setNotification(std::shared_ptr<INotification> notification) {
        currentNotification_ = std::move(notification);
        notifyObservers();
    }

    std::string getNotificationContent() const {
        return currentNotification_ ? currentNotification_->getContent() : "";
    }
};

class INotificationStrategy {
public:
    virtual ~INotificationStrategy() = default;
    virtual void send(const std::string& content) = 0;
};

class EmailStrategy : public INotificationStrategy {
private:
    std::string email_;

public:
    explicit EmailStrategy(std::string email) : email_(std::move(email)) {}
    void send(const std::string& content) override {
        std::cout << "[Email:" << email_ << "] " << content << std::endl;
    }
};

class SMSStrategy : public INotificationStrategy {
private:
    std::string mobileNumber_;

public:
    explicit SMSStrategy(std::string mobileNumber) : mobileNumber_(std::move(mobileNumber)) {}
    void send(const std::string& content) override {
        std::cout << "[SMS:" << mobileNumber_ << "] " << content << std::endl;
    }
};

class PopUpStrategy : public INotificationStrategy {
public:
    void send(const std::string& content) override { std::cout << "[Popup] " << content << std::endl; }
};

class NotificationHub {
private:
    NotificationObservable observable_;
    NotificationHub() = default;

public:
    NotificationHub(const NotificationHub&) = delete;
    NotificationHub& operator=(const NotificationHub&) = delete;

    static NotificationHub& getInstance() {
        static NotificationHub instance;
        return instance;
    }

    NotificationObservable* getObservable() { return &observable_; }

    void sendNotification(const std::shared_ptr<INotification>& notification) {
        observable_.setNotification(notification);
    }
};

class LoggerObserver : public IObserver {
private:
    NotificationObservable* observable_;

public:
    explicit LoggerObserver(NotificationObservable* observable) : observable_(observable) {
        observable_->addObserver(this);
    }

    ~LoggerObserver() override { observable_->removeObserver(this); }

    void update() override { std::cout << "[Logger] " << observable_->getNotificationContent() << std::endl; }
};

class NotificationEngine : public IObserver {
private:
    NotificationObservable* observable_;
    std::vector<std::unique_ptr<INotificationStrategy>> strategies_;

public:
    explicit NotificationEngine(NotificationObservable* observable) : observable_(observable) {
        observable_->addObserver(this);
    }

    ~NotificationEngine() override { observable_->removeObserver(this); }

    void addNotificationStrategy(std::unique_ptr<INotificationStrategy> strategy) {
        strategies_.push_back(std::move(strategy));
    }

    void update() override {
        const std::string content = observable_->getNotificationContent();
        for (auto& strategy : strategies_) {
            strategy->send(content);
        }
    }
};

}  // namespace notification_lld

#endif
