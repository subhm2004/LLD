// ============================================================================
//  dclp_multithreading_safe_notification_system.cpp  —  thread-safe variant
// ----------------------------------------------------------------------------
//  Notification system ka thread-safe version: Singleton ko Double-Checked
//  Locking (DCLP) + mutex se thread-safe banaya gaya hai, taaki multi-threaded
//  environment me bhi ek hi NotificationService instance bane (race nahi).
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#include <fstream>
#include <ctime>

using namespace std;

/*============================
      Notification & Decorators
=============================*/

class INotification
{
public:
    virtual string getContent() const = 0;
    virtual ~INotification() {}
};

class SimpleNotification : public INotification
{
private:
    string text;

public:
    SimpleNotification(const string &msg) { text = msg; }
    string getContent() const override { return text; }
};

class INotificationDecorator : public INotification
{
protected:
    INotification *notification;

public:
    INotificationDecorator(INotification *n) { notification = n; }
    virtual ~INotificationDecorator() { delete notification; }
};

class TimestampDecorator : public INotificationDecorator
{
public:
    TimestampDecorator(INotification *n) : INotificationDecorator(n) {}
    string getContent() const override
    {
        return "[2026-04-30 14:24:30] " + notification->getContent();
    }
};

class SignatureDecorator : public INotificationDecorator
{
private:
    string signature;

public:
    SignatureDecorator(INotification *n, const string &sig) : INotificationDecorator(n)
    {
        signature = sig;
    }
    string getContent() const override
    {
        return notification->getContent() + "\n-- " + signature + "\n\n";
    }
};

/*============================
  Observer Interface
=============================*/

class IObserver
{
public:
    virtual void update() = 0;
    virtual ~IObserver() {}
};

class IObservable
{
public:
    virtual void addObserver(IObserver *observer) = 0;
    virtual void removeObserver(IObserver *observer) = 0;
    virtual void notifyObservers() = 0;
};

/*============================
  Strategy Pattern Components
  (Logger se PEHLE define karo)
=============================*/

class INotificationStrategy
{
public:
    virtual void sendNotification(string content) = 0;
    virtual string getRecipient() const { return "N/A"; }
    virtual ~INotificationStrategy() {}
};

class EmailStrategy : public INotificationStrategy
{
private:
    string emailId;

public:
    EmailStrategy(string emailId) { this->emailId = emailId; }
    string getRecipient() const override { return emailId; }
    void sendNotification(string content) override
    {
        cout << "Sending email Notification to: " << emailId << "\n"
             << content;
    }
};

class SMSStrategy : public INotificationStrategy
{
private:
    string mobileNumber;

public:
    SMSStrategy(string mobileNumber) { this->mobileNumber = mobileNumber; }
    string getRecipient() const override { return mobileNumber; }
    void sendNotification(string content) override
    {
        cout << "Sending SMS Notification to: " << mobileNumber << "\n"
             << content;
    }
};

class PopUpStrategy : public INotificationStrategy
{
public:
    void sendNotification(string content) override
    {
        cout << "Sending Popup Notification: \n"
             << content;
    }
};

/*============================
  Observable
=============================*/

class NotificationObservable : public IObservable
{
private:
    vector<IObserver *> observers;
    INotification *currentNotification;

public:
    NotificationObservable() { currentNotification = nullptr; }

    void addObserver(IObserver *obs) override { observers.push_back(obs); }

    void removeObserver(IObserver *obs) override
    {
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }

    void notifyObservers() override
    {
        for (unsigned int i = 0; i < observers.size(); i++)
            observers[i]->update();
    }

    void setNotification(INotification *notification)
    {
        if (currentNotification != nullptr)
            delete currentNotification;
        currentNotification = notification;
        notifyObservers();
    }

    INotification *getNotification() { return currentNotification; }
    string getNotificationContent() { return currentNotification->getContent(); }

    ~NotificationObservable()
    {
        if (currentNotification != NULL)
            delete currentNotification;
    }
};

/*============================
  Logger (File + Console)
  Strategy ke BAAD define karo
=============================*/

class Logger : public IObserver
{
private:
    NotificationObservable *notificationObservable;
    vector<INotificationStrategy *> *strategiesRef;
    string logFilePath;

    string getCurrentTime()
    {
        time_t now = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

    void writeToFile(const string &entry)
    {
        ofstream logFile(logFilePath, ios::app);
        if (logFile.is_open())
        {
            logFile << entry;
            logFile.close();
        }
        else
        {
            cerr << "[Logger Error] logs.txt open nahi ho paya!\n";
        }
    }

public:
    Logger(NotificationObservable *observable,
           vector<INotificationStrategy *> *strategies,
           const string &filePath = "logs.txt")
    {
        this->notificationObservable = observable;
        this->strategiesRef = strategies;
        this->logFilePath = filePath;
    }

    void update() override
    {
        string content = notificationObservable->getNotificationContent();
        string timestamp = getCurrentTime();

        string logEntry = "";
        logEntry += "========================================\n";
        logEntry += "Time     : " + timestamp + "\n";
        logEntry += "Message  : " + content + "\n";
        logEntry += "Sent To  :\n";

        for (auto *strategy : *strategiesRef)
        {
            if (auto *email = dynamic_cast<EmailStrategy *>(strategy))
                logEntry += "  [Email] " + email->getRecipient() + "\n";
            else if (auto *sms = dynamic_cast<SMSStrategy *>(strategy))
                logEntry += "  [SMS]   " + sms->getRecipient() + "\n";
            else if (dynamic_cast<PopUpStrategy *>(strategy))
                logEntry += "  [PopUp] (screen par)\n";
        }
        logEntry += "========================================\n\n";

        cout << "[LOG] Notification send hui:\n"
             << logEntry;
        writeToFile(logEntry);
    }
};

/*============================
  Notification Engine
=============================*/

class NotificationEngine : public IObserver
{
private:
    NotificationObservable *notificationObservable;

public:
    vector<INotificationStrategy *> notificationStrategies;

    NotificationEngine(NotificationObservable *observable)
    {
        this->notificationObservable = observable;
    }

    void addNotificationStrategy(INotificationStrategy *ns)
    {
        this->notificationStrategies.push_back(ns);
    }

    void update() override
    {
        string notificationContent = notificationObservable->getNotificationContent();
        for (const auto notificationStrategy : notificationStrategies)
            notificationStrategy->sendNotification(notificationContent);
    }
};

/*============================
  NotificationService (DCLP Singleton)
=============================*/

class NotificationService
{
private:
    NotificationObservable *observable;
    static NotificationService *instance;
    static mutex mtx;
    vector<INotification *> notifications;

    NotificationService() { observable = new NotificationObservable(); }

public:
    NotificationService(const NotificationService &) = delete;
    NotificationService &operator=(const NotificationService &) = delete;

    static NotificationService *getInstance()
    {
        if (instance == nullptr)
        {
            lock_guard<mutex> lock(mtx);
            if (instance == nullptr)
                instance = new NotificationService();
        }
        return instance;
    }

    NotificationObservable *getObservable() { return observable; }

    void sendNotification(INotification *notification)
    {
        notifications.push_back(notification);
        observable->setNotification(notification);
    }

    ~NotificationService() { delete observable; }
};

NotificationService *NotificationService::instance = nullptr;
mutex NotificationService::mtx;

/*============================
            main()
=============================*/

int main()
{
    NotificationService *notificationService = NotificationService::getInstance();
    NotificationObservable *notificationObservable = notificationService->getObservable();

    // Engine pehle banao (Logger ko strategies ka reference chahiye)
    NotificationEngine *notificationEngine = new NotificationEngine(notificationObservable);
    notificationEngine->addNotificationStrategy(new EmailStrategy("subhu04012003@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSStrategy("+91 8168447388"));
    notificationEngine->addNotificationStrategy(new PopUpStrategy());

    // Logger ko strategies ka reference do
    Logger *logger = new Logger(
        notificationObservable,
        &notificationEngine->notificationStrategies,
        "logs.txt");

    // Pehle Logger, phir Engine
    notificationObservable->addObserver(logger);
    notificationObservable->addObserver(notificationEngine);

    // Notification 1
    INotification *n1 = new SimpleNotification("Your order has been shipped!");
    n1 = new TimestampDecorator(n1);
    n1 = new SignatureDecorator(n1, "Customer Care");
    notificationService->sendNotification(n1);

    // Notification 2
    INotification *n2 = new SimpleNotification("Your payment of Rs. 499 was successful.");
    n2 = new TimestampDecorator(n2);
    n2 = new SignatureDecorator(n2, "Billing Team");
    notificationService->sendNotification(n2);

    delete logger;
    delete notificationEngine;
    return 0;
}