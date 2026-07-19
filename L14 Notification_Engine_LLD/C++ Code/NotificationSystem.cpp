// ============================================================================
//  NotificationSystem.cpp — NOTIFICATION ENGINE (single-file MONOLITH #1)
// ----------------------------------------------------------------------------
//  Poora notification system EK file me — 4 design patterns ek saath!
//  Modular version notification_lld/ folder me hai (detail: design_patterns_used.md).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  EK notification ka SAFAR (4 patterns milke):                          │
//  │                                                                          │
//  │   SimpleNotification("msg")                                             │
//  │      -> TimestampDecorator    \ DECORATOR: content pe layer-by-layer   │
//  │      -> SignatureDecorator    /  timestamp + signature chipakte hain   │
//  │      -> NotificationService (SINGLETON) ka observable set              │
//  │      -> OBSERVER: sab observers ko update() —                          │
//  │           Logger      -> log print                                     │
//  │           Engine      -> STRATEGY: har channel (Email/SMS/Popup) pe    │
//  │                          content bhejo                                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IS FILE KA LAYOUT: Decorator classes -> Observer (Observable+observers) ->
//  Singleton (NotificationService) -> Strategy (channels) -> Engine -> main.
//
//  L14 me 3 monolith versions hain (evolution):
//    1. NotificationSystem.cpp (ye)         -> basic 4-pattern version
//    2. NotificationSystemUpdated.cpp        -> cleaner/extra features
//    3. dclp_...safe_notification_system.cpp -> Singleton thread-safe (DCLP)
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

/*=============================================================================
  [DESIGN PATTERN: Decorator Pattern]
  -----------------------------------------------------------------------------
  Decorator Pattern ka use dynamically notification message content ko enhance 
  (decorate) karne ke liye kiya jata hai bina existing classes ko modify kiye.
  - `INotification` interface define karta hai ki notification content get kaise karein.
  - `SimpleNotification` base component hai jo raw message text represent karta hai.
  - `INotificationDecorator` abstract wrapper hai jo generic `INotification*` holds 
    karta hai (Composition).
  - `TimestampDecorator` aur `SignatureDecorator` concrete decorators hain jo raw content
    ko dynamic timestamps and signature suffix se garnish/modify karte hain.
=============================================================================*/

// Interface for Notifications (Component)
class INotification
{
public:
    virtual string getContent() const = 0;
    virtual ~INotification() {}
};

// Concrete Notification: Simple text message wrapper (Concrete Component)
class SimpleNotification : public INotification
{
private:
    string text;

public:
    SimpleNotification(const string &msg)
    {
        text = msg;
    }
    string getContent() const override
    {
        return text;
    }
};

// Abstract Decorator: Holds reference to wrapped INotification object
class INotificationDecorator : public INotification
{
protected:
    INotification *notification; // Wrapped object pointer

public:
    INotificationDecorator(INotification *n)
    {
        notification = n;
    }
    
    // Destructor: clean up the recursively wrapped components in memory to avoid leaks
    virtual ~INotificationDecorator()
    {
        delete notification;
    }
};

// Concrete Decorator 1: Adds a mock Timestamp to the start of notification
class TimestampDecorator : public INotificationDecorator
{
public:
    TimestampDecorator(INotification *n) : INotificationDecorator(n) {}

    string getContent() const override
    {
        // Wrapper calls internal content and appends timestamp prefix
        return "[2026-04-30 14:24:30] " + notification->getContent();
    }
};

// Concrete Decorator 2: Appends custom signatures at the end of notification
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
        // Wrapper calls internal content and appends signature suffix
        return notification->getContent() + "\n-- " + signature + "\n\n";
    }
};

/*=============================================================================
  [DESIGN PATTERN: Observer Pattern]
  -----------------------------------------------------------------------------
  Observer pattern ka use tab hota hai jab ek event (new notification arrival) hone 
  par automatic register list of dependencies (observers like Logger, Engine) ko 
  notify / update karna ho.
  - `IObserver`: Notification update event observe karne wala standard interface.
  - `IObservable`: Subject interface jo observer add/remove handles manage karta hai.
  - `NotificationObservable`: Event subject state (currentNotification) maintain
    karta hai. Jab state badalti hai (`setNotification`), tab saare observers notify hote hain.
=============================================================================*/

// Observer Interface (Subscriber)
class IObserver
{
public:
    virtual void update() = 0;
    virtual ~IObserver() {}
};

// Observable Interface (Publisher/Subject)
class IObservable
{
public:
    virtual void addObserver(IObserver *observer) = 0;
    virtual void removeObserver(IObserver *observer) = 0;
    virtual void notifyObservers() = 0;
};

// Concrete Subject: Holds reference to state & active observers
class NotificationObservable : public IObservable
{
private:
    vector<IObserver *> observers;     // List of registered observers
    INotification *currentNotification; // Currently active/incoming notification object

public:
    NotificationObservable()
    {
        currentNotification = nullptr;
    }

    // Attach observer
    void addObserver(IObserver *obs) override
    {
        observers.push_back(obs);
    }

    // Detach observer
    void removeObserver(IObserver *obs) override
    {
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }

    // Broadcaster: Calls update method on all registered observers
    void notifyObservers() override
    {
        for (unsigned int i = 0; i < observers.size(); i++)
        {
            observers[i]->update();
        }
    }

    // Update active notification state & notify subscribers
    void setNotification(INotification *notification)
    {
        if (currentNotification != nullptr)
        {
            delete currentNotification; // Clean old active notification object
        }
        currentNotification = notification;
        notifyObservers(); // Trigger notification broadcast
    }

    INotification *getNotification()
    {
        return currentNotification;
    }

    string getNotificationContent()
    {
        return currentNotification->getContent();
    }

    ~NotificationObservable()
    {
        if (currentNotification != nullptr)
        {
            delete currentNotification;
        }
    }
};

// Concrete Observer 1: Logs new notifications directly to console screen
class Logger : public IObserver
{
private:
    NotificationObservable *notificationObservable; // Bound subject pointer

public:
    Logger(NotificationObservable *observable)
    {
        this->notificationObservable = observable;
    }

    void update() override
    {
        // Fetch new content from Observable & print to console logs
        cout << "Logging New Notification : \n"
             << notificationObservable->getNotificationContent();
    }
};

/*=============================================================================
  [DESIGN PATTERN: Strategy Pattern]
  -----------------------------------------------------------------------------
  Notification delivery channels differ by client requirements.
  Strategy Pattern different communication formats (Email, SMS, PopUp) ko dynamic
  interchangeable strategies me organize karta hai.
  - `INotificationStrategy`: Abstract strategy base.
  - `EmailStrategy`, `SMSStrategy`, and `PopUpStrategy`: Concrete strategies.
  - `NotificationEngine`: Context executor (which is also an Observer) jo multiple
    configured strategies trigger karta hai updates load hone par.
=============================================================================*/

// Strategy Interface for dispatch channels
class INotificationStrategy
{
public:
    virtual void sendNotification(string content) = 0;
    virtual ~INotificationStrategy() {}
};

// Concrete Strategy 1: Email channel simulator
class EmailStrategy : public INotificationStrategy
{
private:
    string emailId;

public:
    EmailStrategy(string emailId)
    {
        this->emailId = emailId;
    }

    void sendNotification(string content) override
    {
        cout << "Sending email Notification to: " << emailId << "\n"
             << content;
    }
};

// Concrete Strategy 2: SMS channel simulator
class SMSStrategy : public INotificationStrategy
{
private:
    string mobileNumber;

public:
    SMSStrategy(string mobileNumber)
    {
        this->mobileNumber = mobileNumber;
    }

    void sendNotification(string content) override
    {
        cout << "Sending SMS Notification to: " << mobileNumber << "\n"
             << content;
    }
};

// Concrete Strategy 3: Push Notification PopUp screen simulator
class PopUpStrategy : public INotificationStrategy
{
public:
    void sendNotification(string content) override
    {
        cout << "Sending Popup Notification: \n"
             << content;
    }
};

// Context Executor (behaves as Concrete Observer 2)
class NotificationEngine : public IObserver
{
private:
    NotificationObservable *notificationObservable;
    vector<INotificationStrategy *> notificationStrategies; // Active dispatch strategies list

public:
    NotificationEngine(NotificationObservable *observable)
    {
        this->notificationObservable = observable;
    }

    // Strategy configuration addition
    void addNotificationStrategy(INotificationStrategy *ns)
    {
        this->notificationStrategies.push_back(ns);
    }

    // Clean strategies pointers on shutdown
    ~NotificationEngine() {
        for (auto* strategy : notificationStrategies) {
            delete strategy;
        }
    }

    // Triggered automatically by Observable
    void update() override
    {
        string notificationContent = notificationObservable->getNotificationContent();
        // Execute algorithms across all active channels strategies
        for (const auto notificationStrategy : notificationStrategies)
        {
            notificationStrategy->sendNotification(notificationContent);
        }
    }
};

/*=============================================================================
  [DESIGN PATTERN: Singleton Pattern]
  -----------------------------------------------------------------------------
  `NotificationService` manager engine class hai jisse client code communicate
  karta hai. Central point execution secure rakhne ke liye singleton use kiya gaya.
=============================================================================*/

// Singleton Notification Manager Service
class NotificationService
{
private:
    NotificationObservable *observable;
    static NotificationService *instance;
    vector<INotification *> notifications; // History ledger archive

    NotificationService()
    {
        observable = new NotificationObservable();
    }

public:
    static NotificationService *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new NotificationService();
        }
        return instance;
    }

    NotificationObservable *getObservable()
    {
        return observable;
    }

    // Submits new notification to observer pipelines
    void sendNotification(INotification *notification)
    {
        notifications.push_back(notification); // Stores in history archive ledger
        observable->setNotification(notification);
    }

    ~NotificationService()
    {
        delete observable;
        // In real systems, dynamic notifications array items must also be cleaned here 
        // if they are not owned and deleted by Observable's setNotification mechanism
    }
};

// Define static instance pointer
NotificationService *NotificationService::instance = nullptr;

// ----------------------------
// Main Driver flow execution
// ----------------------------
int main()
{
    // 1) Initialize Singleton Service wrapper
    NotificationService *notificationService = NotificationService::getInstance();
    NotificationObservable *notificationObservable = notificationService->getObservable();

    // 2) Instantiate Observers
    Logger *logger = new Logger(notificationObservable);
    NotificationEngine *notificationEngine = new NotificationEngine(notificationObservable);

    // 3) Configure dynamic channels strategies inside Engine context
    notificationEngine->addNotificationStrategy(new EmailStrategy("subhu04012003@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSStrategy("+91 8168447388"));
    notificationEngine->addNotificationStrategy(new PopUpStrategy());

    // 4) Attach Observers to Observable subject publisher
    notificationObservable->addObserver(logger);
    notificationObservable->addObserver(notificationEngine);

    // 5) Build enhanced messages using Decorators
    INotification *notification = new SimpleNotification("Your order has been shipped!");
    notification = new TimestampDecorator(notification);
    notification = new SignatureDecorator(notification, "Customer Care");

    // 6) Dispatch Notification
    notificationService->sendNotification(notification);

    // Dynamic memory cleanup
    delete logger;
    delete notificationEngine;
    delete NotificationService::getInstance(); // Optional singleton release

    return 0;
}
