// ============================================================================
//  MediatorPattern.cpp  —  MEDIATOR DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Mediator = "objects ek doosre se SEEDHA baat na karein — sab ek BICHOLIYE
//              (mediator) ke through baat karein."
//
//  Asli duniya ka example: hawai adda (airport). Pilots aapas me radio pe baat
//  nahi karte "bhai tu ruk, main utarta hu." Sab ATC TOWER (control tower) se
//  baat karte hain. Tower sabko coordinate karta hai. Agar har pilot har doosre
//  pilot se seedha baat karta, to aasman me afra-tafri mach jaati. 🛬
//
//  Yahan wahi cheez ek CHAT ROOM se:
//     Tower  = ChatMediator (chat room)
//     Pilots = Users
//  Users ek doosre ka pointer NAHI rakhte. Sirf mediator ko jaante hain. Message
//  bhejna ho -> mediator ko do, wo aage pahunchayega.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ N² -> N  — Mediator ka poora jaadu ek line me:                       │
//  │                                                                          │
//  │     BINA mediator (WithoutMediator.cpp):                                 │
//  │         har user har doosre user se juda  ->  N×(N-1)/2 connections      │
//  │         (mesh/jaal — 100 users = 4,950 connections)                     │
//  │                                                                          │
//  │     Mediator ke saath (ye file):                                         │
//  │         har user sirf mediator se juda   ->  sirf N connections          │
//  │         (star/taara — 100 users = 100 connections)                      │
//  │                                                                          │
//  │     Naya user? Bas mediator se jodo. Baaki kisi ko chhuo mat. ✅         │
//  │     User hatao? Bas mediator ki list se nikalo. Bas.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//     1. Mediator (interface)  -> IMediator      (routing ka contract)
//     2. ConcreteMediator      -> ChatMediator   (asli routing + mute logic)
//     3. Colleague (interface) -> Colleague      (mediator se baat karne wala)
//     4. ConcreteColleague     -> User           (asli chat user)
//
//  ⭐ ASLI FAYDA — LOGIC EK JAGAH:
//     Broadcast kaise ho, mute kaise ho, private-send kaise ho — sab ab
//     ChatMediator me hai, User me NAHI. User phir se sirf "user" ban gaya
//     (naam + mediator ka pointer, bas). Naya feature (history, filter, typing
//     indicator) chahiye? Sirf mediator me add karo. Ek jagah. Ek baar.
//
//  📌 MEDIATOR vs OBSERVER (interview me poocha jaata hai):
//     Dono "objects ko decouple" karte hain, par ulta soch se:
//       OBSERVER : ek subject -> KAI observers ko broadcast (ek-tarfa, one-to-many)
//       MEDIATOR : KAI objects aapas me baat karte hain, par bichauliye ke
//                  through (do-tarfa, many-to-many). Mediator "hub" hai jisme
//                  poori baat-cheet ka gyaan hai.
//     Yaad rakhne ka tareeka: Observer = "newsletter" (ek bhejta, sab padhte).
//     Mediator = "group chat admin" (sab bolte, admin route karta).
// ============================================================================
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// forward‐declare Colleague so IMediator can refer to it.
// Murgi-anda problem: IMediator ko `Colleague*` chahiye, aur Colleague ko
// `IMediator*` chahiye. Dono ek doosre ko chahte hain. Forward declaration se
// compiler ko bata dete hain "Colleague naam ki class HAI, detail baad me."
// Pointer banane ke liye itna hi kaafi hai.
class Colleague;

// ─────────────── Mediator Interface ───────────────
//  Ye "contract" hai — koi bhi mediator (chat room, game lobby, ATC tower) inhi
//  teen kaamon ko support karega. Colleagues sirf is INTERFACE se baat karte
//  hain, kisi concrete mediator se nahi. Isi liye kal ChatMediator ko
//  EncryptedChatMediator se badal do — Users ki ek line bhi nahi badlegi.
class IMediator {
public:
    virtual ~IMediator() {}  // virtual destructor — base pointer se delete safe

    // Naya colleague apne aap ko mediator ke paas "register" karta hai (taaki
    // mediator use messages route kar sake).
    virtual void registerColleague(Colleague* c) = 0;

    // Broadcast — sabko bhejo (khud ko chhod ke, aur mute wale ko chhod ke)
    virtual void send(const string& from, const string& msg) = 0;

    // Private — sirf ek bande ko
    virtual void sendPrivate(const string& from, const string& to, const string& msg) = 0;
};

// ─────────────── Colleague Interface ───────────────
//  "Colleague" = wo object jo mediator ke through baat karta hai. Yahan User.
//  Ye khud kabhi doosre colleague ka pointer nahi rakhta — sirf mediator ka.
class Colleague {
protected:
    // ⭐ Har colleague ke paas mediator ka pointer hai — aur BAS. Doosre
    // colleagues ka koi pointer nahi. Yahi "star" shape hai (mesh nahi).
    IMediator* mediator;

public:
    // ⭐ CHALAK CONSTRUCTOR — colleague bante hi KHUD ko register kar leta hai!
    //    `mediator->registerColleague(this)` — `this` = "main, ye naya colleague".
    //
    // Isse client ka kaam aasan ho gaya: `new User("Rohan", chatRoom)` likho,
    // aur bas — Rohan apne aap chat room me shaamil ho gaya. Client ko alag se
    // `chatRoom->add(rohan)` likhne ki zaroorat hi nahi. (WithoutMediator wale
    // 6 addPeer() calls yaad karo — wo poora jhanjhat gayab.)
    //
    // ⚠ Chhoti si baat: constructor me `this` ko bahar dena thoda risky hai
    // (object abhi poora bana bhi nahi, aur uska pointer bahar chala gaya). Yahan
    // safe hai kyunki registerColleague sirf pointer store karta hai, use turant
    // access nahi karta. Par ye jaan-ne layak hai.
    Colleague(IMediator* m) {
        mediator = m;
        mediator->registerColleague(this);
    }
    virtual ~Colleague() {}

    virtual string getName() = 0;
    virtual void send(const string& msg) = 0;                      // broadcast
    virtual void sendPrivate(const string& to, const string& msg) = 0;  // 1-to-1
    virtual void receive(const string& from, const string& msg) = 0;    // msg mila
};

// ─────────────── Concrete Mediator ───────────────
//  YE HAI DIL. Saara routing + mute logic yahin hai. Users bilkul "bewakoof"
//  hain — bas mediator ko bolte hain, mediator sochta hai.
class ChatMediator : public IMediator {
private:
    vector<Colleague*> colleagues;      // room ke saare members
    vector<pair<string,string>> mutes;  // (muter, muted) jodiyan — "who muted whom"

public:
    void registerColleague(Colleague* c) override {
        colleagues.push_back(c);
    }

    // Mute ka record: `who` ne `whom` ko mute kiya. Yaani `who` ko `whom` ke
    // messages nahi chahiye. Ye poora record MEDIATOR ke paas hai — WithoutMediator
    // me ye har User ke andar bikhra hua tha. Ab ek jagah. ✅
    void mute(const string& who, const string& whom)  {
        mutes.push_back(make_pair(who, whom));
    }

    // ---- BROADCAST — sabko bhejo ------------------------------------------
    void send(const string& from, const string& msg) override {
        cout << "[" << from << " broadcasts]: " << msg << "\n";

        for (Colleague* c : colleagues) {
            // Khud ko apna hi message mat bhejo
            if (c->getName() == from) continue;

            // ---- MUTE CHECK — dhyaan se padho, ye ulta lagta hai ----
            // mutes me har jodi (p.first = muter, p.second = muted).
            // Shart: `from == p.second`     -> bhejne wala = jise mute kiya gaya
            //   AUR  `c->getName() == p.first` -> paane wala = jisne mute kiya
            //
            // Matlab: agar `c` ne `from` ko mute kiya hai, to `c` ko mat bhejo.
            // Udaharan: mute(Rohan, Mohan) -> jab Mohan bhejta hai (from=Mohan),
            // aur receiver Rohan hai -> Rohan skip. "Rohan ko Mohan ka msg nahi."
            bool isMuted = false;
            for (auto& p : mutes) {
                if (from == p.second && c->getName() == p.first) {
                    isMuted = true;
                    break;
                }
            }
            if (!isMuted) {
                c->receive(from, msg);
            }
        }
    }

    // ---- PRIVATE — sirf ek bande ko ---------------------------------------
    void sendPrivate(const string& from, const string& to, const string& msg) override {
        cout << "[" << from << "→" << to << "]: " << msg << "\n";

        for (Colleague* c : colleagues) {
            if (c->getName() == to) {   // target mil gaya
                // Wahi mute check, par private ke liye
                for (auto& p : mutes) {
                    if (from == p.second && to == p.first) {
                        cout << "\n[Message is muted]\n";
                        return;   // muted -> bheja hi nahi, function khatam
                    }
                }
                c->receive(from, msg);
                return;   // mil gaya aur bhej diya -> aur dhoondhne ki zaroorat nahi
            }
        }
        // Poori list ghoom gaye, `to` naam ka koi mila hi nahi
        cout << "[Mediator] User \"" << to << "\" not found]\n";

        // 💡 Chhoti si observation: `send` (broadcast) aur `sendPrivate` dono me
        // wahi mute-check ka loop copy-paste hai. Ise ek private helper me daala
        // ja sakta tha: `bool isMuted(from, to)`. Chhota refactor, par saaf.
        // (Aur `colleagues` ko `vector` ke bajaye `map<string, Colleague*>`
        //  rakhte, to naam se dhoondhna O(n) ke bajaye O(log n) ho jaata —
        //  bade room me farak padta.)
    }
};

// ─────────────── Concrete Colleague ───────────────
//  Ab dekho User kitna PATLA ho gaya! WithoutMediator wala User peers, mute-list,
//  broadcast-logic, private-logic — sab sambhalta tha. Ye wala User sirf 4 line
//  ka kaam karta hai: har request ko mediator ko FORWARD kar deta hai.
class User : public Colleague {
private:
    string name;   // bas ek field. Peers ki list? Mute list? Gayab. Mediator ke paas hai.

public:
    // `: Colleague(m)` -> base constructor chalao, jo KHUD ko register kar leta
    // hai. Isi liye yahan alag se register karne ki zaroorat nahi.
    User(const string& n, IMediator* m)
      : Colleague(m) {
        name = n;
    }

    string getName() override {
        return name;
    }

    // ⭐ Dekho — User khud kuch NAHI karta. Bas mediator ko de deta hai.
    // "Main kise bhejun, kaun muted hai, kaise route karun" — ye sab sochna
    // User ka kaam hi nahi raha. Wo bas "bhej do" bolta hai. Yahi DELEGATION hai.
    void send(const string& msg) override {
        mediator->send(name, msg);
    }

    void sendPrivate(const string& to, const string& msg) override {
        mediator->sendPrivate(name, to, msg);
    }

    // Message aaya to bas dikha do. (Asli app me yahan UI update hota, ya
    // notification bajti — par wo bhi is User ka apna kaam, mediator ka nahi.)
    void receive(const string& from, const string& msg) override {
        cout << "    " << name << " got from " << from << ": " << msg << "\n";
    }
};

// ─────────────── Demo ───────────────
int main() {
    // Chat room (mediator) banao
    ChatMediator* chatRoom = new ChatMediator();

    // ⭐ Users banao — aur bas! Har user constructor me KHUD ko chat room me
    // register kar leta hai. WithoutMediator wale 6 addPeer() calls yaad karo —
    // wo poora boilerplate yahan GAYAB hai. Naya user? Bas ek line. 🎯
    User* user1 = new User("Rohan", chatRoom);
    User* user2 = new User("Neha",  chatRoom);
    User* user3 = new User("Mohan", chatRoom);

    // Rohan ne Mohan ko mute kiya -> "Rohan ko Mohan ke messages nahi chahiye."
    // Ye mute chat room ke paas record hota hai, kisi User ke andar nahi.
    chatRoom->mute("Rohan", "Mohan");

    // ---- Scene 1: Rohan sabko bhejta hai ------------------------------------
    // Rohan ke apne mute se uske OUTGOING message pe koi asar nahi -> Neha aur
    // Mohan DONO ko milega. (Mute receiver ke incoming pe lagta hai, sender ke
    // outgoing pe nahi.)
    user1->send("Hello Everyone!");

    // ---- Scene 2: Mohan, Neha ko private bhejta hai -------------------------
    // Rohan involved nahi hai, to mute ka koi lena-dena nahi -> Neha ko milega.
    user3->sendPrivate("Neha", "Hey Neha!");

    // ---- Scene 3: ⭐ MUTE CHALTE HUE DEKHO — Mohan sabko broadcast karta hai --
    // Rohan ne Mohan ko mute kiya hua hai. To Mohan ke broadcast me se ROHAN
    // SKIP ho jaayega (usko ye msg nahi milega), par NEHA ko mil jaayega.
    // (Output me dekho: sirf "Neha got from Mohan" aayega, Rohan wala nahi.)
    user3->send("Hi from Mohan!");

    // ---- Scene 4: ⭐ Mohan seedha ROHAN ko private bhejta hai ----------------
    // Rohan ne Mohan ko mute kiya hai -> ye message block ho jaayega.
    // (Output me "[Message is muted]" print hoga.)
    user3->sendPrivate("Rohan", "Psst Rohan, sun na!");

    // ⭐ CLEANUP — WithoutMediator se ULTA, yahan aasan hai. Users ek doosre ko
    // jaante hi nahi, to kisi ki "peer-list saaf karo" wali jhanjhat nahi.
    // Bas sabko delete karo.
    //
    // ⚠ Ek chhoti kami: `delete chatRoom` colleagues ko delete NAHI karta — wo
    // sirf pointers rakhta hai, unka maalik nahi (users main me bane). Yahan
    // order theek hai (users pehle, room baad me). Par agar koi user delete
    // hone ke baad bhi chatRoom ki `colleagues` list me uska pointer pada rehta
    // — real system me user ko room se "unregister" bhi karna chahiye. Yahan
    // program khatam ho raha hai, to chal gaya.
    delete user1;
    delete user2;
    delete user3;
    delete chatRoom;
    return 0;
}
