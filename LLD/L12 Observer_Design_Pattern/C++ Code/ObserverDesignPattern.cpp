// ============================================================================
//  ObserverDesignPattern.cpp  —  OBSERVER DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Observer = "ek SUBJECT apne kai OBSERVERS ko, state badalte hi, KHUD-B-KHUD
//              khabar kar deta hai."
//
//  Yahan YouTube ka example hai:
//     Channel (subject)     -> video upload karta hai
//     Subscribers (observers) -> unhe turant notification milti hai
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OBSERVER KYUN? — iska ULTA hai POLLING, aur wo bekaar hai            │
//  │                                                                          │
//  │  Bina Observer ke, subscriber ko khud baar-baar poochna padta:           │
//  │      while (true) {                                                      │
//  │          if (channel.hasNewVideo()) { ... }   // "naya aaya kya?"        │
//  │          sleep(5);                            // "ab? ... ab? ... ab?"   │
//  │      }                                                                   │
//  │  Ise POLLING kehte hain. Teen problem:                                   │
//  │     1. BEKAAR ka kaam — 99% baar jawab "nahi" hota hai                   │
//  │     2. CPU/network barbaad — har check ek request                        │
//  │     3. DER ho jaati hai — video aaya 1 baje, pata chala 1:05 pe          │
//  │                                                                          │
//  │  Observer me ULTA hota hai: subscriber poochta hi nahi. Channel KHUD     │
//  │  bata deta hai, aur theek us waqt jab asal me kuch hua ho. Ise           │
//  │  "event-driven" ya "push model" kehte hain.                             │
//  │                                                                          │
//  │  📌 Ek line me: "Don't call us, we'll call you." (Hollywood Principle)  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//     1. Subject (interface)  -> IChannel     : subscribe/unsubscribe/notify
//     2. ConcreteSubject      -> Channel      : asli YouTube channel
//     3. Observer (interface) -> ISubscriber  : sirf `update()`
//     4. ConcreteObserver     -> Subscriber   : asli subscriber
//
//  ⭐ ASLI JAADU — "1-to-many" aur LOOSE COUPLING:
//     Channel ko ye pata hi NAHI ki uske subscribers kaun hain. Usko bas itna
//     pata hai ki "koi cheez hai jo `update()` ka jawab de sakti hai". Isi
//     liye kal ko `PremiumSubscriber`, `EmailNotifier`, `MobileApp` — kuch bhi
//     add kar do, Channel ka code EK LINE bhi nahi badlega.
//
//  ⚠ Is file me ek MEMORY LEAK hai (main() me — neeche detail me likha hai).
//    Pattern ke liye zaroori nahi, par jaanna zaroori hai.
// ============================================================================
// yt notification system ese design kr skte hai
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // find() ke liye — subscribe/unsubscribe me use hota hai

using namespace std;

// ============================================================================
//  1. OBSERVER (interface) — ISubscriber
// ----------------------------------------------------------------------------
//  Ye "contract" hai: jo bhi notification lena chahta hai, usko `update()`
//  likhna PADEGA. Bas itna hi.
//
//  💡 INTERFACE vs ABSTRACT CLASS — ek chhota sa fark (neeche wala original
//     note yahi keh raha hai):
//       - SAARE methods pure virtual (`= 0`)  -> ise INTERFACE bolte hain
//       - ek bhi method ka body hai            -> ABSTRACT CLASS
//     C++ me dono ke liye alag keyword nahi hai (Java me `interface` hota hai),
//     bas convention hai. `ISubscriber` me sirf `update()` hai aur wo pure
//     virtual hai -> ye ek interface hai. Isi liye naam `I` se shuru hota hai.
// ============================================================================
// agar kisi abstract class sare methos virtual hai to use hum Interface bolte hai or koi ek bhi method virtual h to use bolte hai abstract class

class ISubscriber
{
public:
    // `= 0` -> pure virtual. Koi apna implementation nahi. Jo inherit karega
    // (Subscriber) usko ye likhna HI padega — compiler majboor karega.
    //
    // ⭐ Notice: `update()` me koi PARAMETER nahi hai! Channel sirf ye kehta
    // hai "kuch badla hai", ye nahi ki "kya badla hai". Data lene ke liye
    // observer khud channel se poochta hai (`getVideoData()`).
    // Ise PULL model kehte hain — neeche `Subscriber::update()` me detail hai.
    virtual void update() = 0;

    // Virtual destructor — Channel subscribers ko `ISubscriber*` (base pointer)
    // me rakhta hai, par asal object `Subscriber` hai. Agar kabhi base pointer
    // se delete kiya, to iske bina derived ka destructor chalta hi NAHI ->
    // undefined behavior (uska `name` string leak ho jaati).
    //
    // 📌 RULE: class me ek bhi `virtual` function hai? To destructor bhi
    //    virtual hona chahiye. Bina soche. Har baar.
    virtual ~ISubscriber() {} // virtual destructor for interface
};

// ============================================================================
//  2. SUBJECT (interface) — IChannel
// ----------------------------------------------------------------------------
//  Har "dekhe jaane wale" object ka contract. Teen kaam:
//     subscribe   -> naya observer jodo
//     unsubscribe -> observer hatao
//     notify      -> sabko khabar karo
//
//  ⭐ Ye interface kyun banaya, seedha `Channel` class kyun nahi?
//     Kyunki kal ko aur bhi "subject" ho sakte hain — `Playlist`, `LiveStream`,
//     `CommunityPost`. Sab isi interface ko implement karenge. Aur koi bhi code
//     jo `IChannel*` se baat karta hai, un sab ke saath chal jaayega.
//     (Waise is chhote demo me sirf ek Channel hai — to ye interface abhi
//      "aage ke liye" hai. Chhote code me ye thoda zyada lag sakta hai, par
//      pattern ka poora dhaancha dikhane ke liye rakha hai.)
// ============================================================================
// Abstract Observable interface: a YouTube channel interface
class IChannel
{
public:
    virtual void subscribe(ISubscriber *subscriber) = 0;
    virtual void unsubscribe(ISubscriber *subscriber) = 0;
    virtual void notifySubscribers() = 0;
    virtual ~IChannel() {} // wahi virtual destructor wala rule
};

// ============================================================================
//  3. CONCRETE SUBJECT — Channel (asli YouTube channel)
// ----------------------------------------------------------------------------
//  Ye subscribers ki LIST rakhta hai aur video upload hote hi sabko notify
//  karta hai.
//
//  ⭐ SABSE ZAROORI BAAT — list ka type dekho:
//         vector<ISubscriber *> subscribers;
//                └── INTERFACE ka pointer, `Subscriber*` NAHI!
//
//  Isi ek line se poora "loose coupling" aata hai. Channel ko `Subscriber`
//  class ka naam tak nahi pata — usko bas itna pata hai ki "in sabme `update()`
//  hai". Kal `EmailNotifier` add karo, `Channel` ka code chhuna hi nahi padega.
// ============================================================================
// Concrete Subject: a YouTube channel that observers can subscribe to
class Channel : public IChannel
{
private:
    vector<ISubscriber *> subscribers; // list of subscribers (INTERFACE pointers!)
    string name;
    string latestVideo; // latest uploaded video title — yahi wo "state" hai
                        // jise observers dekhna chahte hain
public:
    Channel(const string &name)
    {
        this->name = name;
    }

    // ---- Naya subscriber jodo ---------------------------------------------
    //
    // ⭐ DUPLICATE check zaroori hai! Bina iske, agar koi galti se do baar
    // subscribe kar de, to usko har video ki notification DO BAAR milegi. 😤
    // Asli app me user "subscribe" button do baar daba sakta hai — ye guard
    // usse bachata hai.
    //
    // `find(begin, end, value)` -> value milti hai to uska iterator, warna
    // `end()`. To `== end()` ka matlab "list me hai hi nahi" -> add kar do.
    //
    // 💡 `vector` me ye check O(n) hai. Lakhon subscribers hon to `set` ya
    //    `unordered_set` behtar hota (O(1)/O(log n)). Par tab NOTIFICATION ka
    //    order kho jaata. Yahan 2 subscribers hain — vector bilkul theek hai.
    void subscribe(ISubscriber *subscriber) override
    {
        if (find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end())
        {
            subscribers.push_back(subscriber);
        }
    }

    // ---- Subscriber hatao --------------------------------------------------
    //
    // ⭐ Ye pattern ka bahut zaroori hissa hai — RUNTIME pe rishta todna.
    // Subscribe/unsubscribe program chalte-chalte ho sakta hai, aur Channel ka
    // code badalna nahi padta. Yahi "dynamic relationship" hai.
    //
    // ⚠ Agar unsubscribe na karo aur subscriber object delete ho jaye, to
    // Channel ki list me DANGLING pointer reh jaayega -> agli notify pe CRASH.
    // Is problem ka naam hai "LAPSED LISTENER" — Observer pattern ki sabse
    // aam galti. (Markdown.md me iski poori baat hai.)
    void unsubscribe(ISubscriber *subscriber) override
    {
        auto it = find(subscribers.begin(), subscribers.end(), subscriber);
        if (it != subscribers.end())
        {
            subscribers.erase(it);
        }
    }

    // ---- ⭐ PATTERN KA DIL — sabko khabar karo ----------------------------
    //
    // Ye teen line hi poora Observer pattern hain. Dhyaan do:
    //   - Channel ko pata NAHI ki `sub` asal me kaun hai (Subscriber? Email?)
    //   - Wo bas `update()` bulata hai aur aage badh jaata hai
    //   - Sahi class ka `update()` apne aap chalta hai (virtual dispatch)
    //
    // Ye POLYMORPHISM hai, aur yahi loose coupling deta hai.
    void notifySubscribers() override
    {
        for (ISubscriber *sub : subscribers)
        {
            sub->update();
        }
        // 💡 Khaali list? Koi baat nahi — loop chalega hi nahi. Channel bina
        //    kisi subscriber ke bhi theek chalta hai. Dono taraf se aazadi.
    }

    // ---- Video upload — yahi wo "event" hai jo sab shuru karta hai ---------
    //
    // ⭐ Do kaam EK SAATH hote hain:
    //     1. state badlo (`latestVideo = title`)
    //     2. sabko batao (`notifySubscribers()`)
    //
    // Ye order ZAROORI hai! Pehle state badlo, PHIR notify karo. Ulta karte
    // to subscribers `getVideoData()` bulate aur unhe PURANA video milta. 🐛
    // Aur notify ko yahin bulana bhi zaroori hai — agar caller ke bharose
    // chhod dete ("upload karke khud notify kar lena"), to koi bhool jaata
    // aur notification jaati hi nahi.
    void uploadVideo(const string &title)
    {
        latestVideo = title;
        cout << "\n[" << name << " uploaded \"" << title << "\"]\n";
        notifySubscribers();
    }

    // ---- Observers isse data "kheenchte" hain (PULL model) ------------------
    string getVideoData()
    {
        return "\nCheckout our new Video : " + latestVideo + "\n";
    }
};

// ============================================================================
//  4. CONCRETE OBSERVER — Subscriber (asli subscriber)
// ----------------------------------------------------------------------------
//  Bas do field: apna naam, aur channel ka pointer.
// ============================================================================
// Concrete Observer: represents a subscriber to the channel
class Subscriber : public ISubscriber
{
private:
    string name;
    Channel *channel; // kis channel ko subscribe kiya (data kheenchne ke liye)

public:
    Subscriber(const string &name, Channel *channel)
    {
        this->name = name;
        this->channel = channel;
    }

    // ---- ⭐ Channel isse bulata hai jab kuch naya hota hai -----------------
    //
    // ⭐ PUSH vs PULL — ye samajhna zaroori hai:
    //
    //    PUSH model : Channel data BHEJ deta -> `update(string videoTitle)`
    //                 Channel ko sochna padta ki "kya-kya bhejun?"
    //
    //    PULL model : Channel sirf "kuch badla" bolta -> `update()`
    //                 Observer KHUD jaake data leta -> `channel->getVideoData()`
    //
    // Ye code PULL use karta hai (dekho — `update()` me koi param nahi, aur
    // andar `channel->getVideoData()` bulaya ja raha hai).
    //
    // Pull ka fayda: har observer sirf WAHI data le jo usko chahiye. Kal ek
    // `ViewCountLogger` add karo — usko video ka naam nahi, sirf count chahiye.
    // Push model me Channel ko sabka data bhejna padta (bekaar), pull me har
    // observer apni cheez khud le leta hai.
    //
    // 💡 Asal me ye HYBRID hai: "kuch hua" ki khabar PUSH hui (`update()`
    //    call), aur data PULL hua (`getVideoData()`). Zyadatar real systems
    //    aisa hi karte hain.
    void update() override
    {
        cout << "Hey " << name << "," << this->channel->getVideoData();
    }
};

// ============================================================================
//  5. CLIENT — demo
// ============================================================================
int main()
{
    // Create a channel and subscribers
    Channel *channel = new Channel("Bhai_ki_padhai");

    Subscriber *subs1 = new Subscriber("Shubham", channel);
    Subscriber *subs2 = new Subscriber("Hardik", channel);

    // ---- SUBSCRIBE — rishta juda ------------------------------------------
    // Ab Channel in dono ko jaanta hai... par sirf `ISubscriber*` ki tarah!
    // Usko "Shubham" ya "Hardik" naam ka pata bhi nahi.
    channel->subscribe(subs1);
    channel->subscribe(subs2);

    // ---- EVENT — video aaya, DONO ko notification -------------------------
    // Ek call, aur poora chain chal padta hai:
    //   uploadVideo -> notifySubscribers -> har sub ka update() -> getVideoData()
    channel->uploadVideo("Observer Pattern Tutorial");

    // ---- UNSUBSCRIBE — Shubham nikal gaya ---------------------------------
    // ⭐ Ye RUNTIME pe hua — koi code recompile nahi, Channel me koi if-else
    // nahi. Bas list se pointer hat gaya. Yahi "dynamic relationship" hai.
    channel->unsubscribe(subs1);

    // ---- Agla video — ab SIRF Hardik ko notification -----------------------
    // (Output me dekho: is baar "Hey Shubham" nahi aayega.)
    channel->uploadVideo("Decorator Pattern Tutorial");

    // ⚠⚠ MEMORY LEAK — yahan `delete` ek bhi nahi hai!
    //
    //   `new` teen baar hua (channel, subs1, subs2), `delete` ZERO baar.
    //   Teeno objects leak ho rahe hain.
    //
    //   Hona chahiye tha:
    //       delete subs1;
    //       delete subs2;
    //       delete channel;
    //   (Aur isi liye `ISubscriber`/`IChannel` me virtual destructor rakha hai
    //    — taaki base pointer se delete karna safe ho.)
    //
    //   💡 Ek dilchasp baat: `leaks` tool is leak ko PAKAD NAHI paata! Wo
    //   "0 leaks" bolta hai. Kyun? Kyunki wo stack ko scan karta hai aur ye
    //   pointers (channel/subs1/subs2) exit ke waqt bhi stack me pade dikhte
    //   hain -> tool unhe "reachable" maan leta hai. Maine `new`/`delete` gin
    //   ke confirm kiya: 8 allocations, sirf 3 frees.
    //   📌 Sabak: tool ka "clean" hamesha sach nahi hota. Code padhna zaroori hai.
    //
    //   Sabse saaf hal: raw pointer ki jagah `unique_ptr` — phir delete
    //   bhoolne ka sawaal hi nahi. (Ya objects ko stack pe hi bana lo — is
    //   demo me `new` ki zaroorat hi nahi thi!)
    return 0;
}
