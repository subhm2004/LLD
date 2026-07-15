// ============================================================================
//  NotificationSystemUpdated.cpp  —  Notification Engine (improved version)
// ----------------------------------------------------------------------------
//  NotificationSystem.cpp ka refined version — same patterns (Singleton +
//  Decorator + Observer + Strategy) par cleaner structure / extra features.
//  Is file me detailed comments (Hinglish + English mix) add kiye gaye hain.
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

/*=============================================================================
  [DESIGN PATTERN: Decorator Pattern]
  -----------------------------------------------------------------------------
  INotification decorator implementation raw string messages ko dynamically wrap 
  karke prefix/suffix (timestamps, signatures) append karti hai.
=============================================================================*/

// Interface for Notifications (Component)
class INotification {
public:
    virtual string getContent() const = 0;
    virtual ~INotification() {}
};

// Concrete Notification: Simple text message wrapper (Concrete Component)
class SimpleNotification : public INotification {
private:
    string text;
public:
    SimpleNotification(const string& msg) {
        text = msg;
    }
    string getContent() const override {
        return text;
    }
};

// Abstract Decorator class for extending behavior
class INotificationDecorator : public INotification {
protected:
    INotification* notification; // Wrapped component instance pointer
public:
    INotificationDecorator(INotification* n) {
        notification = n;
    }
    
    // Destructor: cleans recursively allocated wrapped objects to prevent leaks
    virtual ~INotificationDecorator() {
        delete notification;
    }
};

// Concrete Decorator 1: prepends timestamp formatting strings
class TimestampDecorator : public INotificationDecorator {
public:
    TimestampDecorator(INotification* n) : INotificationDecorator(n) { }
    
    string getContent() const override {
        return "[2025-04-13 14:22:00] " + notification->getContent();
    }
};

// Concrete Decorator 2: appends signature metadata strings
class SignatureDecorator : public INotificationDecorator {
private:
    string signature;
public:
    SignatureDecorator(INotification* n, const string& sig) : INotificationDecorator(n) {
        signature = sig;
    }
    string getContent() const override {
        return notification->getContent() + "\n-- " + signature + "\n\n";
    }
};

/*=============================================================================
  [DESIGN PATTERN: Observer Pattern]
  -----------------------------------------------------------------------------
  Subscribers (Observers) update checks perform karte hain jab new state variables
  publish hote hain Observable (Subject) pipeline me.
  
  *Refinement in Updated Version*: 
  Concrete Observers (Logger, NotificationEngine) ab self-registration coordinate
  karte hain. Apne default constructor me, yeh Singleton `NotificationService` se 
  automatically observable fetch karke khud ko register kar lete hain. Client (main) 
  ko manually `addObserver` call nahi karna padta.
=============================================================================*/

// Observer interface
class IObserver {
public:
    virtual void update() = 0;
    virtual ~IObserver() {}
};

// Observable/Subject interface
class IObservable {
public:
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers() = 0;
};

// Concrete Subject class managing observers and notifications
class NotificationObservable :  public IObservable {
private:
    vector<IObserver*> observers;
    INotification* currentNotification;
public:
    NotificationObservable() { 
        currentNotification = nullptr; 
    }

    void addObserver(IObserver* obs) override {
        observers.push_back(obs);
    }

    void removeObserver(IObserver* obs) override {
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }

    void notifyObservers() override {
        for (unsigned int i = 0; i < observers.size(); i++) {
            observers[i]->update();
        }
    }

    void setNotification(INotification* notification) {
        if (currentNotification != nullptr) {
            delete currentNotification;
        }
        currentNotification = notification;
        notifyObservers();
    }

    INotification* getNotification() {
        return currentNotification;
    }

    string getNotificationContent() {
        return currentNotification->getContent();
    }

    ~NotificationObservable() {
        if (currentNotification != nullptr) {
            delete currentNotification;
        }
    }
};

/*=============================================================================
  [DESIGN PATTERN: Singleton Pattern]
  -----------------------------------------------------------------------------
  `NotificationService` manager acts as a Singleton. Client is ke zariye 
  incoming messages pipe me send karta hai.
=============================================================================*/

class NotificationService {
private:
    NotificationObservable* observable;
    static NotificationService* instance;
    vector<INotification*> notifications; // Stores history database logs

    NotificationService() {
        observable = new NotificationObservable();
    }

public:
    static NotificationService* getInstance() {
        if(instance == nullptr) {
            instance = new NotificationService();
        }
        return instance;
    }

    NotificationObservable* getObservable() {
        return observable;
    }

    void sendNotification(INotification* notification) {
        notifications.push_back(notification);
        observable->setNotification(notification);
    }

    ~NotificationService() {
        delete observable;
    }
};

NotificationService* NotificationService::instance = nullptr;

/*=============================================================================
  Concrete Observers (Logger, NotificationEngine)
  Self-Registration behavior implemented in constructors.
=============================================================================*/

class Logger : public IObserver {
private:
    NotificationObservable* notificationObservable;

public:
    // Default Constructor: Self-registration using Singleton service helper
    Logger() {
       this->notificationObservable = NotificationService::getInstance()->getObservable();
       notificationObservable->addObserver(this); // Self attachment step
    }

    // Parametrized Constructor: Manual attachment option
    Logger(NotificationObservable* observable) {
        this->notificationObservable = observable;
        notificationObservable->addObserver(this);
    }

    void update() override {
        cout << "Logging New Notification : \n" << notificationObservable->getNotificationContent();
    }
};

/*=============================================================================
  [DESIGN PATTERN: Strategy Pattern]
  -----------------------------------------------------------------------------
  Different channels (SMS, Email, UI PopUp) are represented as interchangeable 
  strategies wrapped inside the context coordinator NotificationEngine.
=============================================================================*/

// Strategy Interface
class INotificationStrategy {
public:    
    virtual void sendNotification(string content) = 0;
    virtual ~INotificationStrategy() {}
};

// Concrete Strategy 1: Email channel
class EmailStrategy : public INotificationStrategy {
private:
    string emailId;
public:
    EmailStrategy(string emailId) {
        this->emailId = emailId;
    }

    void sendNotification(string content) override {
        cout << "Sending email Notification to: " << emailId << "\n" << content;
    }
};

// Concrete Strategy 2: SMS channel
class SMSStrategy : public INotificationStrategy {
private:
    string mobileNumber;
public:
    SMSStrategy(string mobileNumber) {
        this->mobileNumber = mobileNumber;
    }

    void sendNotification(string content) override {
        cout << "Sending SMS Notification to: " << mobileNumber << "\n" << content;
    }
};

// Concrete Strategy 3: Popup screen channel
class PopUpStrategy : public INotificationStrategy {
public:
    void sendNotification(string content) override {
        cout << "Sending Popup Notification: \n" << content;
    }
};

// Strategy Context & Observer implementation
class NotificationEngine : public IObserver {
private:
    NotificationObservable* notificationObservable;
    vector<INotificationStrategy*> notificationStrategies;

public:
    // Default Constructor: Self-registration setup
    NotificationEngine() {
        this->notificationObservable = NotificationService::getInstance()->getObservable();
        notificationObservable->addObserver(this); // Self registration
    }

    NotificationEngine(NotificationObservable* observable) {
        this->notificationObservable = observable;
    }

    void addNotificationStrategy(INotificationStrategy* ns) {
        this->notificationStrategies.push_back(ns);
    }

    ~NotificationEngine() {
        for (auto* strat : notificationStrategies) {
            delete strat;
        }
    }

    void update() override {
        string notificationContent = notificationObservable->getNotificationContent();
        for(const auto notificationStrategy : notificationStrategies) {
            notificationStrategy->sendNotification(notificationContent);
        }
    }
};

// ----------------------------
// Main Flow Driver
// ----------------------------
int main() {
    // 1) Get Singleton Manager instance
    NotificationService* notificationService = NotificationService::getInstance();
   
    // 2) Create Observers (They register themselves to observable in constructors!)
    Logger* logger = new Logger();
    NotificationEngine* notificationEngine = new NotificationEngine();

    // 3) Setup strategies inside Engine context
    notificationEngine->addNotificationStrategy(new EmailStrategy("random.person@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSStrategy("+91 9876543210"));
    notificationEngine->addNotificationStrategy(new PopUpStrategy());

    // 4) Construct and decorate message content
    INotification* notification = new SimpleNotification("Your order has been shipped!");
    notification = new TimestampDecorator(notification);
    notification = new SignatureDecorator(notification, "Customer Care");
    
    // 5) Dispatch notification
    notificationService->sendNotification(notification);

    // Clean-ups
    delete logger;
    delete notificationEngine;
    delete NotificationService::getInstance();

    return 0;
}
