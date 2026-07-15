# Tinder App: Design Patterns Analysis

Is document me `/Users/shubham/Desktop/LLD/L27 Tinder_LLD/C++ Code/Tinder_LLD.cpp` file me use hone wale sabhi design patterns ko detail me explain kiya gaya hai.

---

## Quick Summary (Overview of Patterns)

Tinder LLD application me system ko extensible, decoupled aur modular banane ke liye **5 major Design Patterns** ka use kiya gaya hai:

| Pattern Name | Category | Purpose in Tinder App |
| :--- | :--- | :--- |
| **1. Observer Pattern** | Behavioral | Jab bhi user ko new match mile ya new message aaye, tab notifications dispatch karne ke liye. |
| **2. Strategy Pattern** | Behavioral | 1) Location filtering strategy aur 2) Match scoring algorithms ko interchange karne ke liye. |
| **3. Factory Pattern** | Creational | `MatcherType` ke basis par desired matching strategy class ka object create karne ke liye. |
| **4. Singleton Pattern** | Creational | pure application life cycle me `DatingApp`, `LocationService`, aur `NotificationService` ka ek hi global instance maintain karne ke liye. |
| **5. Facade Pattern** | Structural | Subsystems (Location, Matching, Chat, Profiles, Notification) ke dynamic flows ko wrapper (`DatingApp`) ke peeche hide karne ke liye. |

---

## Detailed Analysis of Design Patterns

### 1. Observer Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Tinder me real-time notifications critical hain. Jab do user aapas me match ho jate hain ya messaging karte hain, toh updates ko standard and loose-coupled way me send karne ke liye Observer pattern use hota hai.

#### **Implementation in Code:**
* **Observer Interface:** [NotificationObserver](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L20-L26) ek virtual base class hai jisme `update(message)` method hai.
* **Concrete Observer:** [UserNotificationObserver](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L29-L43) class is notification interface ko implement karti hai aur client/user-specific logic run karti hai.
* **Observable Subject:** [NotificationService](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L46-L90) observable class hai jo sabhi users ke observers register aur manage karti hai aur targeted `notifyUser()` ya multicast `notifyAll()` perform karti hai.

```cpp
// Observer Interface (Line 20-26)
class NotificationObserver {
public:
    virtual ~NotificationObserver() {}
    virtual void update(const std::string &message) = 0;
};

// Concrete Observer (Line 29-43)
class UserNotificationObserver : public NotificationObserver {
private:
    std::string userId;
public:
    UserNotificationObserver(const string &id) : userId(id) {}
    void update(const string &message) override {
        cout << "Notification for user " << userId << ": " << message << endl;
    }
};
```

---

### 2. Strategy Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Tinder me do main places par strategies switch hoti hain:
1. **Location Filtering:** Nearby users dhoondhne ke alag algorithms ho sakte hain (e.g. Quadtree, basic coordinate check, Geohash).
2. **Matching algorithm:** Match calculation ke alag-alag metrics ho sakte hain (e.g. basic profile match, interest-based, premium geo-proximity match).

#### **Implementation in Code:**
* **Example A: Location Strategy**
  * Interface: [LocationStrategy](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L644-L649)
  * Concrete Strategy: [BasicLocationStrategy](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L652-L668)
  * Context class: [LocationService](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L671-L710) allows runtime swapping via `setStrategy()`.

* **Example B: Recommendation Matcher Strategy**
  * Interface: [Matcher](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L724-L729)
  * Concrete Strategies: [BasicMatcher](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L732-L768), [InterestsBasedMatcher](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L771-L807), [LocationBasedMatcher](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L810-L833).

```cpp
// Strategy Context for Location (Line 671-709)
class LocationService {
private:
    LocationStrategy *strategy;
    // ...
public:
    void setStrategy(LocationStrategy *newStrategy) {
        delete strategy;
        strategy = newStrategy;
    }
    vector<User *> findNearbyUsers(const Location &location, double maxDistance, const vector<User *> &allUsers) {
        return strategy->findNearbyUsers(location, maxDistance, allUsers);
    }
};
```

---

### 3. Factory Design Pattern (Simple Factory)
#### **Kyu use kiya gaya? (Intent)**
Faced controller ya client directly concrete matcher classes (jaise `LocationBasedMatcher`) ke objects initiate na kare, isliye matching strategy generation ko Factory class me shift kiya gaya hai.

#### **Implementation in Code:**
* [MatcherFactory](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L836-L853) class runtime enum `MatcherType` ko map karti hai appropriate `Matcher` implementation object ke saath.

```cpp
// Simple Factory for Matcher object creation (Line 836-853)
class MatcherFactory {
public:
    static Matcher *createMatcher(MatcherType type) {
        switch (type) {
        case MatcherType::BASIC:          return new BasicMatcher();
        case MatcherType::INTERESTS_BASED: return new InterestsBasedMatcher();
        case MatcherType::LOCATION_BASED:  return new LocationBasedMatcher();
        default:                           return new BasicMatcher();
        }
    }
};
```

---

### 4. Singleton Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Application data flows ko simplify aur single authority maintain karne ke liye systems ke components ko globally access dena padta hai. Is codebase me 3 classes **Singleton Pattern** implement karti hain.

#### **Implementation in Code:**
* **Classes:**
  1. [NotificationService](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L51-L63) (Manager of notifications)
  2. [LocationService](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L676-L692) (Manager of geographical lookups)
  3. [DatingApp](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L865-L882) (Primary Facade controller)

```cpp
// Notification Service Singleton example
class NotificationService {
private:
    static NotificationService *instance;
    NotificationService() {} // Private Constructor
public:
    static NotificationService *getInstance() {
        if (instance == nullptr) {
            instance = new NotificationService();
        }
        return instance;
    }
};
NotificationService *NotificationService::instance = nullptr;
```

---

### 5. Facade Design Pattern
#### **Kyu use kiya gaya? (Intent)**
Client code (`main()`) ko pure business flow ko execute karne ke liye user matching, notification observers registration, chat validation, swipe history mapping etc. ki micro-managements na karni pade. Facade client ko dynamic entry point interface data hai.

#### **Implementation in Code:**
* [DatingApp](file:///Users/shubham/Desktop/LLD/L27%20Tinder_LLD/C++%20Code/Tinder_LLD.cpp#L858-L1041) class facade ke roop me internal models aur services (`LocationService`, `NotificationService`, `Matcher`) ko interact karke client ko simplified wrapper methods deti hai:
  * `createUser()`
  * `findNearbyUsers()`
  * `swipe()`
  * `sendMessage()`

```cpp
// Facade Interface exposing clean business flows (Line 962-989)
bool swipe(const string &userId, const string &targetUserId, SwipeAction action) {
    User *user = getUserById(userId);
    User *targetUser = getUserById(targetUserId);
    // ... basic logic checking ...
    user->swipe(targetUserId, action);

    if (action == SwipeAction::RIGHT && targetUser->hasLiked(userId)) {
        // Automatically manages Match creation and triggers notification
        ChatRoom *chatRoom = new ChatRoom(chatRoomId, userId, targetUserId);
        NotificationService::getInstance()->notifyUser(userId, "You have a new match!");
        return true;
    }
    return false;
}
```

