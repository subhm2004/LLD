// ============================================================================
//  dclp_multithreading_safe_notification_system.cpp  —  thread-safe variant
// ----------------------------------------------------------------------------
//  Notification system ka thread-safe version: Singleton ko Double-Checked
//  Locking (DCLP) + mutex se thread-safe banaya gaya hai, taaki multi-threaded
//  environment me bhi ek hi NotificationService instance bane (race nahi).
//  Is file me detailed comments (Hinglish + English mix) add kiye gaye hain.
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#include <fstream>
#include <ctime>

using namespace std;

/*=============================================================================
  [DESIGN PATTERN: Decorator Pattern]
  -----------------------------------------------------------------------------
  INotification interface base is formatted dynamically with timestamps and signature
  details recursively during notification creation.
=============================================================================*/

// Interface for Notifications (Component)
class INotification
{
public:
    virtual string getContent() const = 0;
    virtual ~INotification() {}
};

// Concrete Notification: raw message content (Concrete Component)
class SimpleNotification : public INotification
{
private:
    string text;

public:
    SimpleNotification(const string &msg) { text = msg; }
    string getContent() const override { return text; }
};

// Abstract Decorator
class INotificationDecorator : public INotification
{
protected:
    INotification *notification; // Wrapped object pointer

public:
    INotificationDecorator(INotification *n) { notification = n; }
    
    // Destructor: recursively cleans dynamic wrappers to avoid leaks
    virtual ~INotificationDecorator() { delete notification; }
};

// Concrete Decorator 1: prepends timestamp
class TimestampDecorator : public INotificationDecorator
{
public:
    TimestampDecorator(INotification *n) : INotificationDecorator(n) {}
    string getContent() const override
    {
        return "[2026-04-30 14:24:30] " + notification->getContent();
    }
};

// Concrete Decorator 2: appends signature
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

/*=============================================================================
  [DESIGN PATTERN: Observer Pattern - Interfaces]
  -----------------------------------------------------------------------------
  Subject (IObservable) publishes notifications to all registered observers (IObserver).
=============================================================================*/

// Observer Interface (Subscriber)
class IObserver
{
public:
    virtual void update() = 0;
    virtual ~IObserver() {}
};

// Observable Interface (Publisher)
class IObservable
{
public:
    virtual void addObserver(IObserver *observer) = 0;
    virtual void removeObserver(IObserver *observer) = 0;
    virtual void notifyObservers() = 0;
};

/*=============================================================================
  [DESIGN PATTERN: Strategy Pattern]
  -----------------------------------------------------------------------------
  Delivery channels (Email, SMS, PopUp) implement the INotificationStrategy interface,
  decoupling dispatch transport mechanism logic from NotificationEngine.
=============================================================================*/

// Strategy Interface
class INotificationStrategy
{
public:
    virtual void sendNotification(string content) = 0;
    virtual string getRecipient() const { return "N/A"; }
    virtual ~INotificationStrategy() {}
};

// Concrete Strategy 1: Email channel
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

// Concrete Strategy 2: SMS channel
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

// Concrete Strategy 3: Push PopUp channel
class PopUpStrategy : public INotificationStrategy
{
public:
    void sendNotification(string content) override
    {
        cout << "Sending Popup Notification: \n"
             << content;
    }
};

/*=============================================================================
  Observable (Subject implementation)
=============================================================================*/
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
        if (currentNotification != nullptr)
            delete currentNotification;
    }
};

/*=============================================================================
  Logger Observer (Concrete Observer 1)
  -----------------------------------------------------------------------------
  Logger prints notification events to standard console output and archives
  them to a persistent file "logs.txt".
=============================================================================*/
class Logger : public IObserver
{
private:
    NotificationObservable *notificationObservable;
    vector<INotificationStrategy *> *strategiesRef; // Strategy reference to retrieve target numbers/emails
    string logFilePath;

    // Get current formatted calendar date/time string
    string getCurrentTime()
    {
        time_t now = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

    // Write log entry lines to persistent local storage file
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

    // Observer callback updates handler
    void update() override
    {
        string content = notificationObservable->getNotificationContent();
        string timestamp = getCurrentTime();

        string logEntry = "";
        logEntry += "========================================\n";
        logEntry += "Time     : " + timestamp + "\n";
        logEntry += "Message  : " + content + "\n";
        logEntry += "Sent To  :\n";

        // Query active strategies types inside Logger to print routing channels info
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
        writeToFile(logEntry); // Save log logs.txt
    }
};

/*=============================================================================
  NotificationEngine (Concrete Observer 2)
  -----------------------------------------------------------------------------
  Engine context class delegates notifications dynamically over configured strategies.
=============================================================================*/
class NotificationEngine : public IObserver
{
private:
    NotificationObservable *notificationObservable;

public:
    vector<INotificationStrategy *> notificationStrategies; // Active list of strategies

    NotificationEngine(NotificationObservable *observable)
    {
        this->notificationObservable = observable;
    }

    void addNotificationStrategy(INotificationStrategy *ns)
    {
        this->notificationStrategies.push_back(ns);
    }

    ~NotificationEngine() {
        for (auto* strat : notificationStrategies) {
            delete strat;
        }
    }

    void update() override
    {
        string notificationContent = notificationObservable->getNotificationContent();
        // Invoke dynamic channels strategies
        for (const auto notificationStrategy : notificationStrategies)
            notificationStrategy->sendNotification(notificationContent);
    }
};

/*=============================================================================
  [DESIGN PATTERN: Singleton Pattern with Double-Checked Locking (DCLP)]
  -----------------------------------------------------------------------------
  Multithreaded execution me duplicate allocation blocks protect karne ke liye
  `NotificationService` Double-Checked Locking strategy and std::mutex locks follow 
  karti hai.
  1) Lock free 1st check check speed perform karta hai.
  2) Synchronization lock concurrent threads block secure rakhta hai.
  3) lock ke andar 2nd check validation double verify karta hai.
=============================================================================*/

class NotificationService
{
private:
    NotificationObservable *observable;
    static NotificationService *instance;
    static mutex mtx;                      // Thread locks controller mutex
    vector<INotification *> notifications; // Ledger histories vector

    NotificationService() { observable = new NotificationObservable(); }

public:
    // Block assignments to maintain pure Singleton structure
    NotificationService(const NotificationService &) = delete;
    NotificationService &operator=(const NotificationService &) = delete;

    // Double Checked Locking singleton access point
    static NotificationService *getInstance()
    {
        if (instance == nullptr) // 1st Check (Lock-free optimization)
        {
            lock_guard<mutex> lock(mtx); // Acquire Thread Lock
            if (instance == nullptr) // 2nd Check (Security confirmation)
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

// Static pointer initializations
NotificationService *NotificationService::instance = nullptr;
mutex NotificationService::mtx;

/*=============================================================================
  Main driver thread simulator
=============================================================================*/
int main()
{
    // 1) Initialize Thread-Safe Singleton
    NotificationService *notificationService = NotificationService::getInstance();
    NotificationObservable *notificationObservable = notificationService->getObservable();

    // 2) Configure Engine & Strategies
    NotificationEngine *notificationEngine = new NotificationEngine(notificationObservable);
    notificationEngine->addNotificationStrategy(new EmailStrategy("subhu04012003@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSStrategy("+91 8168447388"));
    notificationEngine->addNotificationStrategy(new PopUpStrategy());

    // 3) Create Logger and pass strategies reference for status outputs
    Logger *logger = new Logger(
        notificationObservable,
        &notificationEngine->notificationStrategies,
        "logs.txt");

    // 4) Attach Observers
    notificationObservable->addObserver(logger);
    notificationObservable->addObserver(notificationEngine);

    // 5) Send decorated message 1
    INotification *n1 = new SimpleNotification("Your order has been shipped!");
    n1 = new TimestampDecorator(n1);
    n1 = new SignatureDecorator(n1, "Customer Care");
    notificationService->sendNotification(n1);

    // 6) Send decorated message 2
    INotification *n2 = new SimpleNotification("Your payment of Rs. 499 was successful.");
    n2 = new TimestampDecorator(n2);
    n2 = new SignatureDecorator(n2, "Billing Team");
    notificationService->sendNotification(n2);

    // Dynamic memory cleanup
    delete logger;
    delete notificationEngine;
    delete NotificationService::getInstance();

    return 0;
}