// ============================================================================
//  WithoutMediator.cpp  —  Mediator ke BINA (ye galat tareeka hai, seekhne ke liye)
// ----------------------------------------------------------------------------
//  Ye file JAAN-BOOJH KE "kharab" tareeke se likhi gayi hai — taaki problem
//  dikhe. Solution wali file: MediatorPattern.cpp (usse compare karo).
//
//  PROBLEM EK LINE ME:
//    Har User doosre SAARE Users ka DIRECT pointer rakhta hai. Jaise ek room me
//    har banda har doosre bande se seedha baat kar raha ho — bina kisi bich-bachav
//    ke. Chhote group me theek. Par jaise log badhte hain, ye "mesh" (jaal)
//    itna ulajh jaata hai ki sambhalna namumkin ho jaata hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ N² COUPLING — asli dard yahi hai. Ginti karke dekho:                 │
//  │                                                                          │
//  │     Har jodi ko MANUALLY jodna padta hai (dono taraf se):                │
//  │         3 users -> 3 jodiyan  -> 6 addPeer() calls (neeche main me dekho)│
//  │         5 users -> 10 jodiyan -> 20 calls                                │
//  │         10 users -> 45 jodiyan -> 90 calls                               │
//  │         100 users -> 4,950 jodiyan! 😱                                   │
//  │                                                                          │
//  │     Formula:  N × (N-1) / 2  jodiyan  (yaani connections ~ N²)           │
//  │                                                                          │
//  │     Ek naya user add karo -> baaki SABKE saath jodna padega. Ek user     │
//  │     hatao -> sabki peer-list se nikalna padega (aur yahan to wo code hai │
//  │     hi nahi — leak/dangling pointer!).                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  DOOSRI PROBLEM — LOGIC HAR JAGAH DOHRAAYA (duplication):
//    Dekho: mute ki list HAR User ke andar hai (`mutedUsers`). broadcast ka
//    logic, private-send ka logic, mute-check ka logic — sab User class me
//    ghusa hua hai. Kal ko ek naya feature chahiye (message history? typing
//    indicator? profanity filter?) to wo BHI User me hi ghusega, aur har User
//    usko dohrayega. User class ek "God object" banti ja rahi hai.
//
//    Yaani User do kaam kar raha hai:
//       1. "Main ek user hu" (naam, apni state)
//       2. "Main poore chat-room ka traffic manage karta hu" (routing, mute)
//    Doosra kaam User ka HAI HI NAHI. Yahi SINGLE RESPONSIBILITY ka ullanghan hai.
//
//  ✅ FIX (MediatorPattern.cpp me): ek CENTRAL mediator (chat room) banao. Users
//     ek-doosre ko nahi, sirf MEDIATOR ko jaante hain. N² connections -> N
//     connections. Saara routing/mute logic ek jagah. User phir se sirf "user"
//     ban jaata hai.
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Each User knows *all* the others directly.
// If you have N users, you wind up wiring N*(N–1)/2 connections,
// and every new feature (mute, private send, logging...) lives in User too.

class User {
private:
    string name;
    vector<User*> peers;          // ⚠ har doosre user ka DIRECT pointer — yahi mesh hai
    vector<string> mutedUsers;    // ⚠ har User ki apni-apni mute list (duplication!)

public:
    User(const string& n) {
        name = n;
    }

    // ⚠ Har JODI ko haath se jodna padta hai -> N² wiring.
    // Aur dhyaan do: ye ek-tarfa hai! `a->addPeer(b)` sirf a ki list me b daalta
    // hai. Dono ko ek doosre ka pata ho, iske liye `b->addPeer(a)` bhi likhna
    // padta hai (main me dekho — har jodi 2 baar likhi hai). Ek bhool gaye =
    // ek-tarfa connection = chhupa hua bug.
    void addPeer(User* u) {
        peers.push_back(u);
    }

    // ⚠ DUPLICATION: mute karne ka logic har User ke andar. 100 users = 100
    // jagah yahi code. Mute ka niyam badla (jaise "mute with expiry")? 100
    // jagah badlo.
    //
    // Semantics: "main is bande ke messages nahi dekhna chahta." Yaani mera naam
    // uski broadcast me aaye to skip. (send() me neeche dekho ki ye kaise use hota.)
    void mute(const string& userToMute) {
        mutedUsers.push_back(userToMute);
    }

    // broadcast to all peers
    void send(const string& msg) {
        cout << "[" << name << " broadcasts]: " << msg << endl;
        for (User* peer : peers) {

            // ⭐ MUTE CHECK — par ULTI taraf se socho:
            // `peer->isMuted(name)` = "kya is peer ne MUJHE (sender ko) mute kiya?"
            // Agar haan, to usko mat bhejo.
            //
            // Yaani mute ka faisla SENDER nahi, RECEIVER karta hai — "mujhe is
            // bande ke messages nahi chahiye." Ye sahi semantics hai. Par notice
            // karo: sender ko har ek receiver ke andar jhaank ke uski private
            // mute-list check karni pad rahi hai. Ye tight coupling hai — sender
            // ko receiver ke internal state ki khabar rakhni pad rahi hai.
            if(!peer->isMuted(name)) {
                peer->receive(name, msg);
            }
        }
    }

    bool isMuted(string userName) {
        for(auto name : mutedUsers) {
            if(name == userName) {
                return true;
            }
        }
        return false;
    }

    // ⚠ private send — ye bhi HAR class me dohraaya jaayega. Aur ye bhi seedha
    // `target->receive(...)` bulata hai, yaani sender ko target ka direct pointer
    // aur uski internal `isMuted()` dono chahiye. Coupling upar coupling.
    void sendTo(User* target, const string& msg) {
        cout << "[" << name << "→" << target->name << "]: " << msg << endl;
        if(!target->isMuted(name)) {
            target->receive(name, msg);
        }
    }

    void receive(const string& from, const string& msg) {
        cout << "    " << name << " got from " << from << ": " << msg << endl;
    }
};

int main() {
    // create users
    User* user1 = new User("Rohan");
    User* user2 = new User("Neha");
    User* user3 = new User("Mohan");

    // ⭐ YAHI HAI POORA DARD — har jodi ko 2 baar joda hai (dono taraf se).
    // 3 users ke liye 6 lines. Ye "boilerplate" N ke saath tezi se badhta hai.
    // Aur ye sab client (main) ko khud sambhalna pad raha hai — galti ki poori
    // gunjaish. Ek `addPeer` bhool gaye, to ek-tarfa connection ban jaayega.
    user1->addPeer(user2);
    user2->addPeer(user1);

    user1->addPeer(user3);
    user3->addPeer(user1);

    user2->addPeer(user3);
    user3->addPeer(user2);

    // ⭐ MUTE — code kehta hai: user1 (Rohan) ne "Mohan" ko mute kiya.
    // Yaani "Rohan ko Mohan ke messages nahi chahiye."
    //
    // ⚠ (Original comment yahan galat likha tha — "Mohan mutes Rohan". Asal me
    //  code Rohan ki mute-list me Mohan daal raha hai, yaani ROHAN MUTES MOHAN.)
    user1->mute("Mohan");

    // ---- Scene 1: Rohan sabko broadcast karta hai ---------------------------
    // Rohan ke apne mute se uske OUTGOING pe koi asar nahi -> Neha aur Mohan
    // dono ko milega.
    user1->send("Hello everyone!");

    // ---- Scene 2: Mohan, Neha ko private bhejta hai -------------------------
    // Rohan involved nahi -> mute ka koi asar nahi.
    user3->sendTo(user2, "Hey Neha!");

    // ---- Scene 3: ⭐ MUTE CHALTE HUE — Mohan sabko broadcast karta hai -------
    // Rohan ne Mohan ko mute kiya hua hai. Mohan ke broadcast me se ROHAN skip
    // ho jaayega, par NEHA ko mil jaayega.
    // (Output me sirf "Neha got from Mohan" aayega, Rohan wala nahi.)
    user3->send("Hi from Mohan!");

    // ---- Scene 4: ⭐ Mohan seedha ROHAN ko private bhejta hai ----------------
    // Rohan ne Mohan ko mute kiya hai -> Rohan ko msg NAHI milega.
    // ⚠ Farak notice karo: yahan header line "[Mohan->Rohan]: ..." to print
    // hoti hai (kyunki sendTo pehle print karta, phir mute check karta), par
    // "got from" wali line nahi aati — message chup-chaap gir jaata hai.
    // Mediator wale version me iske liye saaf "[Message is muted]" print hota
    // hai. Ye chhota sa farak dikhata hai ki centralized logic zyada saaf hota.
    user3->sendTo(user1, "Psst Rohan, sun na!");

    // ⚠ CLEANUP ADHOORA: users to delete ho gaye, par kisi ne apni peer-list se
    // dusron ke pointers nahi hataye. Yahan chal gaya kyunki program khatam ho
    // raha hai. Par asli system me agar ek user beech me delete hota, to baaki
    // sabki peer-list me uska DANGLING pointer reh jaata -> agli broadcast pe
    // crash. Ye N² mesh ka ek aur chhupa hua kharcha hai.
    delete user1;
    delete user2;
    delete user3;
    return 0;
}
