// ============================================================================
//  vending_machine_LLD.cpp  —  STATE DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  State = "object ka BEHAVIOR uske ANDAR ke state ke hisaab se badal jaata hai,
//           aur bahar se aisa lagta hai jaise object ne apni CLASS hi badal li ho."
//
//  Asli duniya ka example: vending machine ka "coin daalo" button.
//    - Machine khaali hai (SOLD_OUT)  -> coin wapas kar dega
//    - Coin nahi pada (NO_COIN)       -> coin le lega, balance dikhayega
//    - Coin pehle se pada hai         -> aur coin jod dega
//    - Item nikal raha hai (DISPENSE) -> "ruko bhai" bol ke coin wapas
//  EK HI button, chaar ALAG behavior. Kaunsa chalega ye state decide karta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  STATE PATTERN KYUN? — bina iske code ka haal:                          │
//  │                                                                          │
//  │    void insertCoin(int coin) {                                          │
//  │        if      (state == NO_COIN)   { ... }                             │
//  │        else if (state == HAS_COIN)  { ... }                             │
//  │        else if (state == DISPENSE)  { ... }                             │
//  │        else if (state == SOLD_OUT)  { ... }                             │
//  │    }                                                                     │
//  │    void selectItem()  { ...wahi 4-branch if-else DOBARA... }            │
//  │    void dispense()    { ...wahi 4-branch if-else TEESRI baar... }       │
//  │    void returnCoin()  { ...CHAUTHI baar... }                            │
//  │    void refill()      { ...PAANCHVI baar... }                           │
//  │                                                                          │
//  │  5 actions × 4 states = wahi if-else 5 baar copy-paste. Ab ek NAYA state │
//  │  add karo (jaise MAINTENANCE) -> PAANCHO functions me jaake ek-ek branch │
//  │  add karni padegi. Ek bhi bhool gaye = silent bug. 😵                    │
//  │                                                                          │
//  │  STATE PATTERN se: har state apni CLASS hai. Naya state = ek nayi class, │
//  │  bas. Purana koi bhi code CHHUNA nahi padta. Ye OPEN/CLOSED PRINCIPLE hai│
//  │  — extension ke liye khula, modification ke liye band.                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//    1. State (interface)  -> VendingState    : saare actions ka contract
//    2. ConcreteState      -> NoCoinState     : coin ka intezaar
//                          -> HasCoinState    : paisa pada hai, item chuno
//                          -> DispenseState   : item nikal raha hai
//                          -> SoldOutState    : stock khatam
//    3. Context            -> VendingMachine  : current state rakhta hai aur
//                                               har call usko DELEGATE karta hai
//
//  ============================================================================
//   STATE TRANSITION TABLE — poori machine ek nazar me
//   (row = abhi kaunsa state, column = kya action kiya, cell = agla state)
//  ----------------------------------------------------------------------------
//              | insertCoin | selectItem      | dispense        | returnCoin | refill
//   -----------+------------+-----------------+-----------------+------------+---------
//   NO_COIN    | HAS_COIN ✅| NO_COIN (error) | NO_COIN (error) | NO_COIN    | NO_COIN
//   HAS_COIN   | HAS_COIN   | DISPENSE ✅     | HAS_COIN (error)| NO_COIN ✅ | HAS_COIN
//              | (paisa jud)| (agar paisa poora;                | (paisa wapas)| (mana)
//              |            |  warna HAS_COIN)                  |            |
//   DISPENSE   | DISPENSE   | DISPENSE (wait) | NO_COIN ✅      | DISPENSE   | DISPENSE
//              | (coin wapas)|                | ya SOLD_OUT     | (mana)     | (mana)
//              |            |                 | (agar stock 0)  |            |
//              |            |                 | ⭐ paisa YAHIN  |            |
//              |            |                 |   kata jaata hai|            |
//   SOLD_OUT   | SOLD_OUT   | SOLD_OUT        | SOLD_OUT        | SOLD_OUT   | NO_COIN ✅
//              | (coin wapas)| (sab mana)     |                 |            | (agar stock
//              |            |                 |                 |            |  asal me aaya;
//              |            |                 |                 |            |  warna SOLD_OUT)
//
//   ⭐ Notice karo: har cell bhara hua hai. Koi bhi galat action kisi bhi state
//   me chalao — machine crash nahi hogi, bas polite error de ke USI state me ruk
//   jaayegi. "Invalid action" handling apne aap ho jaati hai, kyunki HAR state ne
//   HAR action ka jawab likha hai. Ye pattern ki sabse badi khoobi hai.
//
//   📌 SABSE BADA RULE — STATE AUR DATA KABHI JHOOTH NA BOLEIN:
//   Har transition ke baad ye sach hona chahiye:
//        state == SOLD_OUT   <=>   itemCount == 0
//        state == HAS_COIN   =>    insertedCoins > 0
//   Is file me ek asli bug THA jo yahi rule todta tha: SoldOutState::refill(0)
//   machine ko NO_COIN me bhej deta tha jabki stock 0 hi tha. Machine "chalu"
//   dikhti, paisa le leti, aur itemCount -1 pe chala jaata. 💀
//   Ab fix hai — refill state ko stock ke SACH se hi milata hai.
//  ============================================================================
#include <iostream>
#include <string>

using namespace std;

// ============================================================================
//  FORWARD DECLARATIONS — ye kyun chahiye?
// ----------------------------------------------------------------------------
//  Yahan CIRCULAR DEPENDENCY hai (murgi-anda wali problem):
//     VendingState ke functions ko `VendingMachine*` chahiye (param me)
//     VendingMachine ko `VendingState*` chahiye (member me)
//  Dono ek dusre ko chahte hain! Koi bhi "pehle" nahi likha ja sakta.
//
//  Solution: forward declaration. `class VendingMachine;` ka matlab hai —
//  "compiler, itna maan le ki VendingMachine naam ki koi class HAI. Uske andar
//   kya hai wo main baad me bataunga." Compiler ko POINTER banane ke liye bas
//  itna hi chahiye (kyunki har pointer ka size same hota hai, chahe andar kuch
//  bhi ho). Puri class ki definition tabhi chahiye jab tum uske MEMBERS use karo
//  (jaise `machine->getPrice()`) — aur wo neeche tak aa hi jaayegi.
// ============================================================================
class VendingMachine;

// Ye chaar iss liye (aur sirf iss liye) chahiye kyunki VendingMachine ka
// constructor inhe `new` karta hai — par constructor ki BODY neeche likhi hai
// (file ke end me), jahan tak ye chaaro classes poori define ho chuki hongi.
class NoCoinState;
class HasCoinState;
class DispenseState;
class SoldOutState;

// ============================================================================
//  1. STATE (interface) — har state ko ye 5 sawaalon ka jawab dena HI padega
// ----------------------------------------------------------------------------
//  ⭐ SABSE IMPORTANT DESIGN CHOICE — har function `VendingState*` RETURN karta
//  hai. Ye return value hi AGLA STATE hai!
//
//      currentState = currentState->insertCoin(this, coin);
//                     └──────── purana state kaam karta hai ────────┘
//      └── aur jo lauta ke deta hai, wahi naya state ban jaata hai ──┘
//
//  Iska matlab: TRANSITION KA FAISLA STATE KHUD LETA HAI, machine nahi.
//  Machine bilkul "bewakoof" hai — usko pata hi nahi ki NO_COIN ke baad HAS_COIN
//  aata hai. Wo bas puchhti hai "ab kya?" aur jo state batata hai, maan leti hai.
//  Yahi asli decentralization hai — poori state machine ka gyaan 4 chhoti classes
//  me baanta hua hai, ek jagah ka bada switch nahi.
//
//  (Ek doosra popular variant: return void, aur state khud `machine->setState(x)`
//   bulaye. Dono theek hain. Return-wala tareeka thoda saaf hai kyunki state ko
//   machine ka state BADALNE ka haq dena hi nahi padta — bas "suggest" karta hai,
//   set machine khud karti hai. Kam power = kam bug.)
//
//  "Stay in same state" ka matlab bhi yahi hai — state apne aap ko hi wapas kar
//  deta hai (`return machine->getNoCoinState();`). Koi special "kuch mat karo"
//  wala case likhne ki zaroorat hi nahi.
// ============================================================================
class VendingState {
public:
    // `= 0` -> pure virtual. Har concrete state ko ye 6 ke 6 likhne PADENGE.
    // Compiler majboor kar dega — koi state kisi action ko "bhool" nahi sakta.
    // Yahi wajah hai ki transition table me koi khaali cell nahi hai.
    virtual VendingState* insertCoin(VendingMachine* machine, int coin) = 0;
    virtual VendingState* selectItem(VendingMachine* machine) = 0;
    virtual VendingState* dispense(VendingMachine* machine) = 0;
    virtual VendingState* returnCoin(VendingMachine* machine) = 0;
    virtual VendingState* refill(VendingMachine* machine, int quantity) = 0;
    virtual string getStateName() = 0;  // sirf debugging/printing ke liye

    // ✅ VIRTUAL DESTRUCTOR — ye ZAROORI hai.
    //
    // Kyun? VendingMachine in states ko `VendingState*` (base pointer) me rakhti
    // hai, par asal object `NoCoinState` hai (derived). Jab machine ka destructor
    // `delete noCoinState;` karega, to wo BASE pointer se delete kar raha hoga.
    // Agar ye destructor virtual na hota, to sirf VendingState ka destructor
    // chalta aur NoCoinState ka chalta hi NAHI -> undefined behavior.
    //
    // 📌 RULE: class me ek bhi `virtual` function hai? To destructor bhi virtual
    //    hona chahiye. Bina soche. Har baar.
    virtual ~VendingState() {}
};

// ============================================================================
//  2. CONTEXT — VendingMachine
// ----------------------------------------------------------------------------
//  Context ke paas 2 cheezein hain:
//    (a) DATA  -> itemCount, itemPrice, insertedCoins
//                 (states ke paas apna koi data nahi hota — sab yahin rehta hai)
//    (b) CURRENT STATE -> aur har public call bas ise DELEGATE ho jaati hai
//
//  ⭐ Dekho ki VendingMachine me ek bhi `if (state == ...)` NAHI hai. Ek bhi
//  nahi. Poori business logic states ke paas chali gayi hai. Machine sirf ek
//  "dabba" hai jo data aur current state pakde rehta hai. Yahi Context ka kaam
//  hai — patla (thin) rehna.
// ============================================================================
class VendingMachine {
private:
    VendingState* currentState;  // abhi machine kis mood me hai
    int itemCount;               // kitne bottles bache
    int itemPrice;               // ek bottle ka daam
    int  insertedCoins;          // abhi tak kitna paisa pada hai

    // ---- Chaaro state objects — ek-ek karke, PEHLE SE bana ke rakhe hue -----
    // ⭐ Ye states har baar `new` nahi hote! Constructor me EK BAAR bante hain,
    // aur poori machine ki zindagi bhar wahi 4 objects ghoomte rehte hain.
    //
    // Ye kyun possible hai? Kyunki states STATELESS hain — unke paas apna koi
    // member variable nahi hai! Saara data (coins, count, price) machine ke paas
    // hai, aur state usko `machine->` se maangta hai. Jab object ke paas apna
    // kuch hai hi nahi, to uski ek se zyada copy banane ka koi matlab hi nahi.
    //
    // 💡 L30 (Flyweight) yaad aaya? Bilkul wahi idea hai — shared, immutable
    // objects jo baar-baar reuse hote hain. Asal me to ye 4 `static` bhi ho sakte
    // the (saari machines ke beech shared) — 100 vending machines banao to abhi
    // 400 state objects banenge, jabki 4 se kaam chal jaata.
    VendingState* noCoinState;
    VendingState* hasCoinState;
    VendingState* dispenseState;
    VendingState* soldOutState;

public:
    VendingMachine(int itemCount, int itemPrice);

    // ✅ DESTRUCTOR — constructor ne 4 baar `new` kiya tha, ab 4 baar `delete`.
    // Iske bina wo 4 objects leak ho jaate the.
    //
    // ⭐ Ye kaam karta hi tabhi hai jab VendingState ka destructor VIRTUAL ho
    // (upar dekho) — kyunki hum BASE pointer se DERIVED object delete kar rahe
    // hain. Dono cheezein ek doosre ke bina adhuri hain.
    //
    // 💡 Sabse saaf tareeka: raw pointer ki jagah `unique_ptr<VendingState>`
    // rakh lo — phir ye destructor likhne ki zaroorat hi nahi padti, aur delete
    // bhoolne ka sawaal hi khatam. Yahan raw pointer isi liye rakha hai taaki
    // pattern ka dhaancha saaf dikhe, aur ownership manually samajh aaye.
    ~VendingMachine() {
        delete noCoinState;
        delete hasCoinState;
        delete dispenseState;
        delete soldOutState;
        // currentState ko delete NAHI karna! Wo in chaaro me se hi kisi EK ko
        // point kar raha hai — ek hi object ko do baar delete karna = crash.
        // Ye "ownership" ka classic sabak hai: pointer hone ka matlab maalik
        // hona nahi hota. currentState sirf "dekh raha hai", "rakh nahi raha".
    }

    // ---- Public API — 5 actions. Inki body neeche hai, aur ekdum boring hai --
    // Har ek bas ek line: "current state se poochho, jo bataye wo naya state."
    void insertCoin(int coin);
    void selectItem();
    void dispense();
    void returnCoin();
    void refill(int quantity);

    void printStatus();

    // ---- STATE GETTERS — states inhi se "agla state" maangte hain ------------
    // NoCoinState ko HasCoinState ka object kahan se milega? Machine se hi:
    //     return machine->getHasCoinState();
    // Isi liye machine ko chaaro states public me expose karne padte hain.
    // (Thoda leaky lagta hai — machine apne andar ka maal bahar dikha rahi hai.
    //  Par yahi is pattern ka standard tareeka hai, aur practical bhi.)
    VendingState* getNoCoinState() {
        return noCoinState;
    }
    VendingState* getHasCoinState() {
        return hasCoinState;
    }
    VendingState* getDispenseState() {
        return dispenseState;
    }
    VendingState* getSoldOutState() {
        return soldOutState;
    }

    // ---- DATA ACCESS — states ka poora "toolkit" ----------------------------
    // States ke paas apna data nahi hai (stateless hain), to machine ka data
    // padhne/badalne ke liye unhe ye getters/setters chahiye. Ye set jitna chhota
    // rakho utna acha — jitna states ko chahiye, bas utna hi expose karo.
    int getItemCount() {
        return itemCount;
    }
    void decrementItemCount() {
        itemCount--;
    }
    void incrementItemCount(int count = 1) {
        itemCount += count;
    }
    int getInsertedCoin() {
        return insertedCoins;
    }
    void setInsertedCoin(int coin) {   // 0 karne ke liye bhi yahi use hota hai
        insertedCoins = coin;
    }
    void addCoin(int coin) {           // purane paise ke UPAR jodne ke liye
        insertedCoins += coin;
    }
    int getPrice() {
        return this->itemPrice;
    }
    void setPrice(int itemPrice) {
        this->itemPrice = itemPrice;
    }
};

// ============================================================================
//  3. CONCRETE STATE — NO_COIN : "paisa daalo pehle"
// ----------------------------------------------------------------------------
//  Machine khadi hai, stock hai, par abhi tak kisi ne paisa nahi daala.
//  Yahan sirf EK cheez sahi hai: coin daalna. Baaki sab pe polite error.
// ============================================================================
class NoCoinState : public VendingState {
public:
    // ✅ Sahi action — coin aaya
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
        // `setInsertedCoin` (= assign), `addCoin` (+= ) nahi — kyunki is state
        // me paisa pehle se ZERO hi hota hai, jodne ko kuch hai hi nahi.
        machine->setInsertedCoin(coin); // Rs 10
        cout << "Coin inserted. Current balance: Rs " << coin <<endl;

        // ⭐ TRANSITION! Ab machine HAS_COIN mode me chali gayi. Agli baar jab
        // koi insertCoin() bulayega, to HasCoinState::insertCoin chalega —
        // wahi function NAHI jo abhi chala. Behavior badal gaya, kyunki state
        // badal gaya. Yahi poore pattern ka nichod hai.
        return machine->getHasCoinState(); // Transition to HasCoinState
    }

    // ❌ Galat action — paisa daale bina item chun rahe ho
    VendingState* selectItem(VendingMachine* machine) override {
       cout << "Please insert coin first!" <<endl;
        // Khud ko hi wapas kar do = "state nahi badla, wahi ke wahi hain".
        // Crash nahi, exception nahi, bas ek saaf message. Har "galat" action
        // ka handling itni hi simple hai.
        return machine->getNoCoinState(); // Stay in same state
    }

    // ❌ Galat action
    VendingState* dispense(VendingMachine* machine) override {
       cout << "Please insert coin and select item first!" <<endl;
        return machine->getNoCoinState(); // Stay in same state
    }

    // ❌ Galat action — jo paisa daala hi nahi, wo wapas kya karein?
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "No coin to return!" <<endl;
        return machine->getNoCoinState(); // Stay in same state
    }

    // ✅ Sahi action — staff aa ke stock bhar sakta hai (koi transaction chal
    //    nahi rahi, to bilkul safe hai)
    VendingState* refill(VendingMachine* machine, int quantity) override {
        // ✅ Wahi validation jo SoldOutState::refill me hai — 0 ya negative
        // quantity ka koi matlab nahi. (Yahan bug utna khatarnak nahi tha kyunki
        // stock pehle se hai, par negative quantity phir bhi count bigaad sakti
        // thi. Guard dono jagah hona chahiye.)
        if (quantity <= 0) {
            cout << "Invalid refill quantity: " << quantity <<endl;
            return machine->getNoCoinState(); // kuch nahi badla
        }

        cout << "Items refilling" <<endl;
        machine->incrementItemCount(quantity);
        return machine->getNoCoinState(); // Stay in same state (stock to pehle se tha)
    }

   string getStateName() override {
        return "NO_COIN";
    }
};

// ============================================================================
//  4. CONCRETE STATE — HAS_COIN : "paisa pada hai, ab item chuno"
// ----------------------------------------------------------------------------
//  Ye sabse "interesting" state hai — yahan asli business logic hai (paisa poora
//  hai ya nahi, change kitna banega). Baaki states zyadatar error handling hi hain.
// ============================================================================
class HasCoinState : public VendingState {
public:
    // ✅ Aur paisa daal sakte ho — 10 daala tha, 10 aur daalo = 20
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
        // Ab `addCoin` (+=), kyunki purane paise ke UPAR jodna hai.
        // NoCoinState me `setInsertedCoin` (=) tha. Zara sa fark, par ye hi to
        // pattern ka point hai — same action, alag state, alag behavior.
        machine->addCoin(coin);
        cout << "Additional coin inserted. Current balance: Rs " << machine->getInsertedCoin() <<endl;
        return machine->getHasCoinState(); // Stay in same state (paisa aur jud gaya)
    }

    // ⭐ Ye is poori file ka sabse important function hai — asli faisla yahan hota
    VendingState* selectItem(VendingMachine* machine) override {
        // ✅ STOCK CHECK — pehle ye dekho ki dene ko kuch hai bhi ya nahi.
        //
        // Ye "defense in depth" hai. Normally machine stock khatam hote hi
        // SOLD_OUT chali jaati hai, to HAS_COIN me 0 stock ke saath pahunchna
        // nahi chahiye. Par "nahi hona chahiye" aur "nahi ho sakta" me farak hai
        // — pehle SoldOutState::refill(0) me ek bug tha jo theek yahi raasta khol
        // deta tha. Paisa lene se PEHLE stock verify karna zaroori hai, chahe
        // theory me ye check kabhi trigger na ho.
        if (machine->getItemCount() <= 0) {
            cout << "Machine is sold out! Returning coin: Rs " << machine->getInsertedCoin() <<endl;
            machine->setInsertedCoin(0);           // paisa wapas — machine ka haq nahi
            return machine->getSoldOutState();     // aur sach batao ki dukan band hai
        }

        if (machine->getInsertedCoin() >= machine->getPrice()) {
            // ---- Paisa poora hai (ya zyada) -> transaction aage badhega ------
           cout << "Item selected. Dispensing..." <<endl;

            // ✅ PAISA YAHAN NAHI KAATA — ab ye kaam dispense() karega.
            //
            // Pehle yahan `setInsertedCoin(0)` tha aur change bhi yahin wapas
            // hota tha. Problem: paisa kat jaata tha par item DISPENSING state
            // me jaake ATAK jaata tha. Agar user ne `dispense()` kabhi bulaya
            // hi nahi, to paisa gaya aur item mila hi nahi. 💸
            //
            // Ab paisa aur item DONO `dispense()` me ek saath move hote hain —
            // ya to dono hote hain, ya dono nahi. Ise ATOMICITY kehte hain, aur
            // paise wale kisi bhi system me ye non-negotiable hai.
            //
            // (Isi liye ab DISPENSING state me printStatus "Rs 20" dikhayega,
            //  "Rs 0" nahi — machine ne paisa PAKDA hua hai, kaata nahi hai.
            //  Ye zyada honest hai.)
            return machine->getDispenseState(); // ⭐ Transition -> DISPENSING
        }
        else {
            // ---- Paisa kam hai -> na item, na state change ------------------
            int needed = machine->getPrice() - machine->getInsertedCoin();
            cout << "Insufficient funds. Need Rs " << needed << " more." <<endl;

            // Paisa wapas NAHI kar rahe — user aur daal sakta hai. Isi liye
            // HAS_COIN me hi rukte hain. Bilkul asli machine jaisa.
            return machine->getHasCoinState(); // Stay in same state
        }
    }

    // ❌ Item chune bina dispense nahi ho sakta
    VendingState* dispense(VendingMachine* machine) override {
       cout << "Please select an item first!" <<endl;
        return machine->getHasCoinState(); // Stay in same state
    }

    // ✅ Mann badal gaya? Paisa wapas lo
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "Coin returned: Rs " << machine->getInsertedCoin() <<endl;
        machine->setInsertedCoin(0);
        // Paisa 0 ho gaya -> ab NO_COIN wapas. Poora circle ghoom ke wapas shuru.
        return machine->getNoCoinState(); // Transition to NoCoinState
    }

    // ❌ Beech transaction me refill mana hai — user ka paisa pada hua hai,
    //    abhi stock chhedna galat hai. (Business rule, technical majboori nahi.)
    //    (param ka naam hata diya — use hota hi nahi, aur bina naam ke compiler
    //     "unused parameter" ki warning nahi deta.)
    VendingState* refill(VendingMachine* machine, int) override {
        cout << "Can't refill in this state" <<endl;
        return machine->getHasCoinState(); // Stay in same state
    }

   string getStateName() override {
        return "HAS_COIN";
    }
};

// ============================================================================
//  5. CONCRETE STATE — DISPENSING : "item nikal raha hai, ruko"
// ----------------------------------------------------------------------------
//  Ye ek "busy"/transient state hai — machine ka motor chal raha hai. Yahan
//  sirf EK cheez allowed hai: dispense() poora karna. Baaki sab "wait" bolega.
// ============================================================================
class DispenseState : public VendingState {
public:
    // ❌ Beech me coin mat daalo — turant wapas
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
       cout << "Please wait, already dispensing item. Coin returned: Rs " << coin <<endl;
        // Note: `addCoin` NAHI bulaya — yahi "coin wapas kar diya" hai. Machine
        // ne coin liya hi nahi, to wapas karne ke liye kuch karna hi nahi tha.
        return machine->getDispenseState();  // Stay in same state
    }

    // ❌ Ek baar me ek hi item
    VendingState* selectItem(VendingMachine* machine) override {
       cout << "Already dispensing item. Please wait." <<endl;
        return machine->getDispenseState(); // Stay in same state
    }

    // ✅ Asli kaam — yahan PAISA aur ITEM dono ek saath move hote hain
    VendingState* dispense(VendingMachine* machine) override {
        // ---- Ab payment YAHAN hota hai (pehle HasCoin::selectItem me hota tha) --
        // ⭐ Paisa aur item ek hi function me handle karne se ATOMICITY milti hai:
        // ya to dono hote hain, ya dono nahi. Beech me atakne ka koi mauka hi nahi.
        int change = machine->getInsertedCoin() - machine->getPrice();
        machine->setInsertedCoin(0);   // machine ne apna daam le liya

        if (change > 0) {              // 0 hone pe print mat karo, bekaar ka shor
            cout << "Change returned: Rs " << change <<endl;
        }

        cout << "Item dispensed!" <<endl;

        // Stock ab kam hua — theek USI waqt jab paisa kata. Dono saath saath.
        machine->decrementItemCount();

        // ⭐ CONDITIONAL TRANSITION — agla state DATA pe depend karta hai, action
        // pe nahi! Ye state pattern ki ek powerful baat hai: transition sirf
        // "kya kiya" se nahi, "ab haalat kya hai" se bhi decide hota hai.
        if (machine->getItemCount() > 0) {
            return machine->getNoCoinState();   // stock bacha -> agle grahak ke liye taiyaar
        }
        else {
           cout << "Machine is now sold out!" <<endl;
            return machine->getSoldOutState();  // stock khatam -> dukan band
        }
    }

    // ❌ Motor chal raha hai — ab refund nahi.
    //
    // Note: ab paisa machine ke paas ASAL ME pada hai (kyunki hamne payment
    // dispense() me shift kar diya). To technically refund ho SAKTA tha. Par
    // physically item nikal raha hai — motor ko beech me rok nahi sakte.
    // Isi liye mana karna hi sahi hai. Ye technical majboori nahi, BUSINESS RULE
    // hai — aur donon me farak samajhna zaroori hai.
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "Cannot return coin while dispensing item!" <<endl;
        return machine->getDispenseState(); // Stay in same state
    }

    // ❌ Motor chal raha hai, abhi stock mat chhedo
    // (param `quantity` ka naam jaan-boojh ke hata diya — use hota hi nahi, aur
    //  naam na dene se compiler ki "unused parameter" warning bhi nahi aati.)
    VendingState* refill(VendingMachine* machine, int) override {
        cout << "Can't refill in this state" <<endl;
        return machine->getDispenseState(); // Stay in same state
    }

   string getStateName() override {
        return "DISPENSING";
    }
};

// ============================================================================
//  6. CONCRETE STATE — SOLD_OUT : "stock khatam, dukan band"
// ----------------------------------------------------------------------------
//  Sabse "sakht" state — customer kuch bhi kare, machine mana kar degi.
//  Sirf EK raasta bahar nikalne ka hai: staff aa ke refill kare.
// ============================================================================
class SoldOutState : public VendingState {
public:
    // ❌ Paisa mat lo — dene ko kuch hai hi nahi. Turant wapas.
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
       cout << "Machine is sold out. Coin returned: Rs " << coin <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }

    VendingState* selectItem(VendingMachine* machine) override {
       cout << "Machine is sold out!" <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }

    VendingState* dispense(VendingMachine* machine) override {
       cout << "Machine is sold out!" <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }

    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "Machine is sold out. No coin inserted." <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }

    // ✅ EKMATR raasta bahar nikalne ka — staff stock bhar de
    VendingState* refill(VendingMachine* machine, int quantity) override {
        // ---- ✅ FIX #1: bekaar ka refill reject karo ------------------------
        // `refill(0)` ya `refill(-5)` ka koi matlab nahi. Pehle ye check tha hi
        // nahi — negative quantity seedha itemCount ko negative kar sakti thi.
        if (quantity <= 0) {
            cout << "Invalid refill quantity: " << quantity <<endl;
            return machine->getSoldOutState();   // kuch nahi badla -> yahin ruko
        }

        cout << "Items refilling" <<endl;
        machine->incrementItemCount(quantity);

        // ---- ✅ FIX #2: state ko DATA se milao, umeed se nahi ----------------
        //
        // 🐛 Pehle yahan seedha `return machine->getNoCoinState();` tha — bina ye
        // dekhe ki stock ASAL ME aaya bhi ya nahi. To `refill(0)` ke baad machine
        // NO_COIN me chali jaati thi jabki itemCount abhi bhi 0 tha. Machine
        // "chalu" dikhti thi par khaali hoti thi. Uske baad:
        //     insertCoin(20) -> paisa le liya
        //     selectItem()   -> DISPENSING (item ka wada kar diya)
        //     dispense()     -> itemCount = -1  💀 (jo item tha hi nahi, wo bech diya)
        // User ka paisa gaya, item nahi mila, aur count negative. Sach me test
        // karke dekha tha — bilkul yahi hota tha.
        //
        // Ab state hamesha data ka SACH bolegi: stock hai to NO_COIN, warna
        // SOLD_OUT. Upar `quantity <= 0` wala guard laga hone ke baad ye check
        // theory me kabhi fail nahi hoga — par yahi to baat hai. State machine ka
        // sabse bada rule: **state aur data kabhi jhooth na bolein.** Do jagah
        // guard lagana yahan zyada nahi, zaroori hai.
        if (machine->getItemCount() > 0) {
            return machine->getNoCoinState();    // stock aa gaya -> dukan khul gayi
        }
        else {
            return machine->getSoldOutState();   // abhi bhi khaali -> band hi raho
        }
    }

   string getStateName() override {
        return "SOLD_OUT";
    }
};

// ============================================================================
//  7. VendingMachine ki BODY — sabse aakhir me kyun?
// ----------------------------------------------------------------------------
//  Kyunki constructor `new NoCoinState()` karta hai — aur `new` karne ke liye
//  compiler ko us class ka POORA structure chahiye (size kitna hai, constructor
//  kya hai). Forward declaration se sirf pointer ban sakta tha, `new` nahi.
//
//  Isi liye order: interface -> context ka DECLARATION -> concrete states ->
//  context ki DEFINITION. Ye circular dependency ko todne ka standard tareeka hai.
//  (Asli project me ye alag .h/.cpp files me hota, aur ye jhanjhat apne aap
//   sulajh jaati — jaise L31 me tha.)
// ============================================================================
VendingMachine::VendingMachine(int itemCount, int itemPrice) {

    this->itemCount = itemCount;
    this->itemPrice = itemPrice;
    this->insertedCoins = 0;

    // Chaaro states EK BAAR bante hain, aur machine ki poori zindagi chalte hain.
    // (Upar likha tha — ye stateless hain, isi liye reuse safe hai.)
    noCoinState = new NoCoinState();
    hasCoinState = new HasCoinState();
    dispenseState = new DispenseState();
    soldOutState = new SoldOutState();

    // ⭐ Shuruaati state DATA se decide hota hai — hardcoded NO_COIN nahi!
    // Machine khaali bani hai (itemCount = 0)? To seedha SOLD_OUT me shuru karo.
    // Ye chhota sa `if` ek accha touch hai — "state hamesha data ke sach ke saath
    // match karna chahiye". (Afsos ki SoldOutState::refill ye rule tod deta hai.)
    if (itemCount  > 0) {
        currentState = noCoinState;
    } else {
        currentState = soldOutState;
    }
}

// ============================================================================
//  DELEGATION — poore pattern ka dil, aur ye 5 line hi sab kuch hai
// ----------------------------------------------------------------------------
//  Har function bilkul EK jaisa hai:
//      1. current state se kaam karwao (`currentState->action(this, ...)`)
//      2. jo state wo lauta ke de, use naya current state bana do
//
//  Bas. Machine me ek bhi `if (state == ...)` nahi. Zero. Machine ko bilkul
//  nahi pata ki NO_COIN ke baad kya aata hai — wo bas "jo mila, wo rakh liya".
//
//  `this` pass karna zaroori hai — state ke paas apna koi data nahi hai, usko
//  machine ka data (coins, count, price) chahiye hi chahiye. Isi liye har call
//  me machine ka pointer bhejte hain.
// ============================================================================
void VendingMachine::insertCoin(int coin) {
    currentState = currentState->insertCoin(this, coin);
}

void VendingMachine::selectItem() {
    currentState = currentState->selectItem(this);
}

void VendingMachine::dispense() {
    currentState = currentState->dispense(this);
}

void VendingMachine::returnCoin() {
    currentState = currentState->returnCoin(this);
}

void VendingMachine::refill(int quantity) {
    currentState = currentState->refill(this, quantity);
}

void VendingMachine::printStatus() {
    cout << "\n--- Vending Machine Status ---" << endl;
    cout << "Items remaining: " << itemCount << endl;
    cout << "Inserted coin: Rs " << insertedCoins << endl;
    // `getStateName()` bhi virtual hai — current state jo bhi ho, uska naam
    // apne aap sahi print ho jaayega. Machine ko puchhna nahi padta "tu kaun hai".
    cout << "Current state: " << currentState->getStateName() << endl << endl;
}

// ============================================================================
//  8. CLIENT — poori state machine ko chala ke dekhte hain
// ----------------------------------------------------------------------------
//  Machine: 2 bottles, Rs 20 each.
//  Neeche har step pe likha hai ki state kahan se kahan jaayega.
// ============================================================================
int main() {
    cout << "=== Water Bottle VENDING MACHINE ===" <<endl;

    int itemCount = 2;
    int itemPrice = 20;

    VendingMachine machine(itemCount, itemPrice);
    machine.printStatus();   // stock 2 hai -> shuruaat NO_COIN se

    // ------------------------------------------------------------------------
    //  Test scenarios — har action state badal SAKTA hai (zaroori nahi ki badle)
    // ------------------------------------------------------------------------

    // ---- 1. GALAT ACTION — bina paise ke item maang rahe ho -----------------
    //  NO_COIN -> NO_COIN (koi badlav nahi, bas ek polite error)
    //  Notice: koi crash nahi, koi exception nahi. Machine bas mana kar deti hai.
    cout << "1. Trying to select item without coin:" <<endl;
    machine.selectItem();  // "Please insert coin first!"
    machine.printStatus();

    // ---- 2. Rs 10 daala -----------------------------------------------------
    //  NO_COIN -> HAS_COIN  ⭐ pehla asli transition
    cout << "2. Inserting coin:" <<endl;
    machine.insertCoin(10);  // balance: 10
    machine.printStatus();

    // ---- 3. Paisa kam hai ---------------------------------------------------
    //  HAS_COIN -> HAS_COIN. Item Rs 20 ka hai, paisa sirf Rs 10.
    //  "Insufficient funds. Need Rs 10 more."
    //  ⭐ Paisa WAPAS nahi kiya — user aur daal sakta hai. Isi liye HAS_COIN me
    //  hi rukte hain, NO_COIN me wapas nahi jaate. Chhota detail, badi baat.
    cout << "3. Selecting item with insufficient funds:" <<endl;
    machine.selectItem();
    machine.printStatus();

    // ---- 4. Rs 10 aur daala -------------------------------------------------
    //  HAS_COIN -> HAS_COIN. Ab balance 10 + 10 = 20.
    //  ⭐ Yahan `addCoin` (+=) chala. Step 2 me `setInsertedCoin` (=) chala tha.
    //  SAME function call (insertCoin), ALAG behavior — kyunki state alag hai.
    //  Yahi to poore pattern ka nichod hai. Isko dobara padho. 🎯
    cout << "4. Adding more coins:" <<endl;
    machine.insertCoin(10);  // balance: 20
    machine.printStatus();

    // ---- 5. Ab item chuno ---------------------------------------------------
    //  HAS_COIN -> DISPENSING  (20 >= 20, paisa poora hai)
    //
    //  ⭐ printStatus dhyaan se dekho:
    //     "Items remaining: 2"  -> abhi bhi 2! Item abhi nikla nahi.
    //     "Inserted coin: Rs 20" -> paisa abhi bhi PADA hai, kata nahi!
    //
    //  Yaani machine ne abhi tak KUCH nahi liya, kuch nahi diya — bas "haan"
    //  bola hai. Paisa katna aur item dena, dono ab dispense() me ek saath
    //  honge. Ya dono, ya kuch bhi nahi. (Pehle yahan paisa kat jaata tha aur
    //  item DISPENSING me atak jaata tha — wo bug ab fix hai.)
    cout << "5. Selecting item Now" <<endl;
    machine.selectItem();
    machine.printStatus();

    // ---- 6. Item bahar aaya -------------------------------------------------
    //  DISPENSING -> NO_COIN  (itemCount 2 se 1 hua, aur 1 > 0 hai -> stock bacha)
    //  ⭐ Ab yahan DONO cheezein ek saath hui: paisa kata (Rs 20 -> Rs 0) AUR
    //  item nikla (2 -> 1). change = 0 tha, isi liye "Change returned" print
    //  nahi hua.
    cout << "6. Dispensing item:" <<endl;
    machine.dispense();
    machine.printStatus();

    // ---- 7. Aakhri bottle — poora cycle ek saath ----------------------------
    //  NO_COIN -> HAS_COIN -> DISPENSING -> SOLD_OUT
    //  Aakhri me itemCount 0 ho jaayega, aur DispenseState ka conditional
    //  transition SOLD_OUT chun lega (NO_COIN nahi). ⭐ Wahi "agla state data se
    //  decide hota hai" wali baat — yahan chal ke dikh rahi hai.
    cout << "7. Buying last item:" <<endl;
    machine.insertCoin(20);  // NO_COIN -> HAS_COIN
    machine.selectItem();    // HAS_COIN -> DISPENSING
    machine.dispense();      // DISPENSING -> SOLD_OUT ("Machine is now sold out!")
    machine.printStatus();

    // ---- 8. Sold out machine me paisa daalne ki koshish ---------------------
    //  SOLD_OUT -> SOLD_OUT. Coin turant wapas.
    //  ⭐ Yahi wahi `insertCoin` hai jo step 2 me paisa le raha tha! Ab mana kar
    //  raha hai. Ek line ka bhi `if` likhe bina — bas state badla, behavior badla.
    cout << "8. Trying to use sold out machine:" <<endl;
    machine.insertCoin(5);   // "Machine is sold out. Coin returned: Rs 5"

    // ---- 9. Staff refill karta hai ------------------------------------------
    //  SOLD_OUT -> NO_COIN. Machine phir se zinda. 2 bottles wapas.
    //  (Print bolta "Trying to use sold out machine" — copy-paste ki galti hai,
    //   asal me ye refill wala step hai.)
    cout << "9. Trying to use sold out machine:" <<endl;
    machine.refill(2);
    machine.printStatus();   // items: 2, state: NO_COIN

    // ---- 10. Bug wala case, ab FIXED ----------------------------------------
    //  Pehle `refill(0)` machine ko NO_COIN me bhej deta tha — 0 stock ke saath!
    //  Machine "chalu" dikhti, paisa le leti, item ka wada karti, aur itemCount
    //  -1 pe chala jaata. User ka paisa gaya, item mila nahi. 🐛
    //
    //  Ab: quantity <= 0 reject ho jaata hai, aur state stock ke SACH se hi
    //  decide hota hai. Machine jhooth nahi bolti.
    //
    //  Pehle stock khatam karte hain (2 bottles bech ke), phir refill(0) try:
    cout << "\n10. BUG TEST — pehle stock khatam, phir refill(0):" <<endl;
    machine.insertCoin(20); machine.selectItem(); machine.dispense();  // bottle 1
    machine.insertCoin(20); machine.selectItem(); machine.dispense();  // bottle 2 -> SOLD_OUT
    machine.refill(0);       // ✅ "Invalid refill quantity: 0" -> SOLD_OUT me hi rahega
    machine.printStatus();   // items: 0, state: SOLD_OUT  (pehle NO_COIN aata tha! ❌)

    return 0;
    // ✅ `machine` stack pe hai, to scope khatam hote hi uska destructor apne aap
    // chalega — aur wo chaaro state objects delete kar dega. Koi leak nahi.
    // (Pehle destructor tha hi nahi, isi liye wo 4 objects leak ho rahe the.)
}
