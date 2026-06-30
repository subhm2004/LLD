// notification_lld.h — Umbrella header: poore modular notification system ke
// saare parts (core/enums/models/rules/services) ek jagah include kar deta hai.
#ifndef NOTIFICATION_LLD_H
#define NOTIFICATION_LLD_H

#include "core/NotificationSystem.h"
#include "enums/ChannelType.h"
#include "enums/NotificationPriority.h"
#include "models/NotificationMessage.h"
#include "models/Recipient.h"
#include "rules/NotificationRules.h"
#include "services/NotificationEngine.h"
#include "services/NotificationService.h"

#endif
#ifndef NOTIFICATION_LLD_H
#define NOTIFICATION_LLD_H

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace notification_lld {

class INotification {
public:
    virtual std::string getContent() const = 0;
    virtual ~INotification() = default;
};

class SimpleNotification : public INotification {
private:
    std::string text_;

public:
    explicit SimpleNotification(std::string msg) : text_(std::move(msg)) {}

    std::string getContent() const override { return text_; }
};

class INotificationDecorator : public INotification {
protected:
    INotification* notification_;

public:
    explicit INotificationDecorator(INotification* notification) : notification_(notification) {}

    ~INotificationDecorator() override { delete notification_; }
};

class TimestampDecorator : public INotificationDecorator {
public:
    explicit TimestampDecorator(INotification* notification) : INotificationDecorator(notification) {}

    std::string getContent() const override {
        return "[2025-04-13 14:22:00] " + notification_->getContent();
    }
};

class SignatureDecorator : public INotificationDecorator {
private:
    std::string signature_;

public:
    SignatureDecorator(INotification* notification, std::string signature)
        : INotificationDecorator(notification), signature_(std::move(signature)) {}

    std::string getContent() const override {
        return notification_->getContent() + "\n-- " + signature_ + "\n\n";
    }
};

class IObserver {
public:
    virtual void update() = 0;
    virtual ~IObserver() = default;
};

class IObservable {
public:
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers() = 0;
    virtual ~IObservable() = default;
};

class NotificationObservable : public IObservable {
private:
    std::vector<IObserver*> observers_;
    INotification* currentNotification_ = nullptr;

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

    void setNotification(INotification* notification) {
        if (currentNotification_ != nullptr) {
            delete currentNotification_;
        }
        currentNotification_ = notification;
        notifyObservers();
    }

    std::string getNotificationContent() const {
        return currentNotification_ == nullptr ? std::string() : currentNotification_->getContent();
    }

    ~NotificationObservable() override {
        if (currentNotification_ != nullptr) {
            delete currentNotification_;
        }
    }
};

class NotificationService {
private:
    NotificationObservable* observable_;
    inline static NotificationService* instance_ = nullptr;

    NotificationService() : observable_(new NotificationObservable()) {}

public:
    static NotificationService* getInstance() {
        if (instance_ == nullptr) {
            instance_ = new NotificationService();
        }
        return instance_;
    }

    NotificationObservable* getObservable() const { return observable_; }

    void sendNotification(INotification* notification) { observable_->setNotification(notification); }

    ~NotificationService() { delete observable_; }
};

class Logger : public IObserver {
private:
    NotificationObservable* notificationObservable_;

public:
    Logger() {
        notificationObservable_ = NotificationService::getInstance()->getObservable();
        notificationObservable_->addObserver(this);
    }

    explicit Logger(NotificationObservable* observable) : notificationObservable_(observable) {
        notificationObservable_->addObserver(this);
    }

    void update() override {
        std::cout << "Logging New Notification : \n"
                  << notificationObservable_->getNotificationContent();
    }
};

class INotificationStrategy {
public:
    virtual void sendNotification(const std::string& content) = 0;
    virtual ~INotificationStrategy() = default;
};

class EmailStrategy : public INotificationStrategy {
private:
    std::string emailId_;

public:
    explicit EmailStrategy(std::string emailId) : emailId_(std::move(emailId)) {}

    void sendNotification(const std::string& content) override {
        std::cout << "Sending email Notification to: " << emailId_ << "\n" << content;
    }
};

class SMSStrategy : public INotificationStrategy {
private:
    std::string mobileNumber_;

public:
    explicit SMSStrategy(std::string mobileNumber) : mobileNumber_(std::move(mobileNumber)) {}

    void sendNotification(const std::string& content) override {
        std::cout << "Sending SMS Notification to: " << mobileNumber_ << "\n" << content;
    }
};

class PopUpStrategy : public INotificationStrategy {
public:
    void sendNotification(const std::string& content) override {
        std::cout << "Sending Popup Notification: \n" << content;
    }
};

class NotificationEngine : public IObserver {
private:
    NotificationObservable* notificationObservable_;
    std::vector<INotificationStrategy*> notificationStrategies_;

public:
    NotificationEngine() {
        notificationObservable_ = NotificationService::getInstance()->getObservable();
        notificationObservable_->addObserver(this);
    }

    explicit NotificationEngine(NotificationObservable* observable) : notificationObservable_(observable) {
        notificationObservable_->addObserver(this);
    }

    void addNotificationStrategy(INotificationStrategy* strategy) {
        notificationStrategies_.push_back(strategy);
    }

    void update() override {
        const std::string content = notificationObservable_->getNotificationContent();
        for (const auto* strategy : notificationStrategies_) {
            strategy->sendNotification(content);
        }
    }
};

}  // namespace notification_lld

#endif  // NOTIFICATION_LLD_H
