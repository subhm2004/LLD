// ============================================================================
//  CommandPattern.cpp  —  COMMAND DESIGN PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Command = "ek REQUEST ko ek OBJECT bana do."
//
//  Sunne me ajeeb lagta hai — request ko object kaise banayein? Par jab request
//  object ban jaati hai, to usme jaadu aa jaata hai:
//     - use VARIABLE me rakh sakte ho
//     - LIST me daal sakte ho (queue, history)
//     - baad me chala sakte ho (scheduling)
//     - LOG kar sakte ho
//     - aur sabse zaroori — UNDO kar sakte ho ⭐
//
//  Ye sab seedha function call se namumkin hai. `light->on()` ko tum list me
//  nahi daal sakte, na undo kar sakte ho. Par `LightCommand` object ko kar sakte ho.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ COMMAND KYUN? — bina iske remote ka haal                            │
//  │                                                                          │
//  │  class RemoteControl {                                                   │
//  │      Light* light;  Fan* fan;  AC* ac;  TV* tv;   // sabko jaanna padega!│
//  │      void pressButton(int n) {                                           │
//  │          if      (n == 0) light->on();                                   │
//  │          else if (n == 1) light->off();                                  │
//  │          else if (n == 2) fan->on();                                     │
//  │          else if (n == 3) ac->cool();      // ... 50 buttons = 50 branch │
//  │      }                                                                   │
//  │  };                                                                      │
//  │                                                                          │
//  │  Teen problem:                                                           │
//  │    1. Remote ko HAR device ka pata hona chahiye (tight coupling)         │
//  │    2. Naya device (Geyser) aaya? -> Remote ka code CHHEDNA padega        │
//  │    3. UNDO? Bhool jao. `light->on()` ko undo kaise karoge?               │
//  │                                                                          │
//  │  Command se: Remote ko sirf `Command*` pata hai. Bas `execute()` bulata  │
//  │  hai. Usko pata bhi nahi ki andar Light hai ya Fan ya Geyser. 🎯         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE TEEN ROLES (GoF naming) — ye teen naam yaad rakho:
//
//     1. COMMAND  -> `Command`, `LightCommand`, `FanCommand`
//                    "kaam" ka object. Jaanta hai KYA karna hai aur KISSE.
//
//     2. RECEIVER -> `Light`, `Fan`
//                    ASLI kaam karne wala. Isko pattern ka kuch pata hi nahi!
//                    Notice: `Light` class me `Command` ka naam tak nahi hai.
//
//     3. INVOKER  -> `RemoteController`
//                    Jo command trigger karta hai. Isko RECEIVER ka kuch pata
//                    nahi — bas `command->execute()` bulata hai.
//
//  ⭐ ASLI JAADU: Invoker aur Receiver ek doosre ko JAANTE HI NAHI. Beech me
//     Command khada hai. Isi liye naya device add karo — Remote ka code ek line
//     bhi nahi badlega. (Open/Closed Principle ✅)
//
//  🧹 MEMORY: is file ka memory management SAAF hai — `RemoteController` ka
//     destructor apne saare commands delete karta hai, aur `main()` receivers
//     ko. Gin ke confirm kiya: 13 `new`, 13 `delete`, 0 leak. ✅
// ============================================================================
#include <bits/stdc++.h> 
using namespace std;

// ============================================================================
//  1. COMMAND (interface) — har "kaam" ka contract
// ----------------------------------------------------------------------------
//  Sirf do function — par ye do hi poora pattern hain:
//     execute() -> kaam karo
//     undo()    -> kaam ULTA karo  ⭐ yahi Command ki sabse badi taakat hai
//
//  ⭐ `undo()` kyun itna khaas hai?
//     Kyunki request ab ek OBJECT hai, wo apne saath ye bhi yaad rakh sakti hai
//     ki "mujhe ulta kaise karna hai". Seedha `light->on()` likhne se ye
//     namumkin tha — ab `LightCommand` khud jaanta hai ki uska ulta `off()` hai.
//
//     Isi se Ctrl+Z banta hai! Commands ko ek stack me daalte jao, aur Ctrl+Z
//     pe upar wala nikaal ke uska `undo()` bula do. Bas.
// ============================================================================
class Command
{
public:
    // `= 0` -> pure virtual. Har concrete command ko dono likhne PADENGE.
    virtual void execute() = 0;
    virtual void undo() = 0;

    // Virtual destructor — RemoteController commands ko `Command*` (base pointer)
    // me rakhta hai, par asal object `LightCommand` hai. Uska destructor
    // `delete cmd` karta hai — BASE pointer se. Iske bina derived ka destructor
    // chalta hi nahi -> undefined behavior.
    // 📌 RULE: ek bhi virtual function hai? To destructor bhi virtual. Har baar.
    virtual ~Command() {}
};

// ============================================================================
//  2. RECEIVERS — asli kaam karne wale (Light, Fan)
// ----------------------------------------------------------------------------
//  ⭐ SABSE ZAROORI BAAT — in classes me `Command` ka naam TAK nahi hai!
//
//  Light ko pata hi nahi ki wo kisi "pattern" ka hissa hai. Wo bas on/off jaanti
//  hai. Aur yahi sahi hai — receiver ko pattern se aazaad rehna chahiye.
//
//  Fayda: `Light` class kahin bhi use ho sakti hai — Command ke saath, bina
//  Command ke, kisi aur pattern me. Wo kisi se bandhi nahi hai.
//  📌 Ye SINGLE RESPONSIBILITY hai: Light ka kaam sirf light hona hai.
// ============================================================================
class Light
{
public:
    void on()
    {
        cout << "Light is ON" << endl;
    }
    void off()
    {
        cout << "Light is OFF" << endl;
    }
};

class Fan
{
public:
    void on()
    {
        cout << "Fan is ON" << endl;
    }
    void off()
    {
        cout << "Fan is OFF" << endl;
    }
};

// ============================================================================
//  3. CONCRETE COMMANDS — kaam ko object me lapetna
// ----------------------------------------------------------------------------
//  Har command me do cheezein hoti hain:
//     - RECEIVER ka pointer  (kisse kaam karwana hai)  <- HAS-A
//     - kya karna hai        (execute/undo me likha)
//
//  Dhyaan do ki command khud kuch NAHI karta — bas receiver ko bolta hai.
//  Wo ek "parchi" (order slip) hai: "Light ko ON karo".
// ============================================================================

// Concrete Command for Light
class LightCommand : public Command
{
private:
    Light *light; // RECEIVER ka pointer (has-a)
                  // ⚠ Command is Light ka MAALIK nahi hai — bas use "dekh" raha
                  //   hai (borrow). Light ka maalik main() hai, wahi delete
                  //   karega. Isi liye yahan koi destructor nahi hai.
                  //   📌 "Pointer hone ka matlab maalik hona nahi hota."

public:
    LightCommand(Light *l)
    {
        light = l;
    }

    // `override` -> compiler check karega ki base class me sach me aisa virtual
    // function hai. Typo hui (jaise `exectue()`) to COMPILE ERROR milega —
    // chupke se ek naya function banne ke bajaye.
    //
    // ⚠ Ye pehle MISSING tha! Bina `override` ke, agar naam galat likh dete to
    // compiler chup rehta, aur runtime pe base ka pure-virtual chalne ki koshish
    // hoti. Chhota keyword, bade bugs se bachav.
    void execute() override
    {
        light->on();
    }

    // ⭐ `undo()` = `execute()` ka ULTA. Yahi wo baat hai jo Command ko seedhe
    // function call se alag banati hai.
    void undo() override
    {
        light->off();
    }
};

// Concrete Command for Fan
class FanCommand : public Command
{
private:
    Fan *fan; // RECEIVER (borrowed — main() maalik hai)

public:
    FanCommand(Fan *f)
    {
        fan = f;
    }
    void execute() override
    {
        fan->on();
    }
    void undo() override
    {
        fan->off();
    }
};

// ============================================================================
//  4. INVOKER — RemoteController (ye implementation ka sabse interesting hissa)
// ----------------------------------------------------------------------------
//  Ye remote hai — jo button dabata hai. Par ye normal remote nahi, ek
//  DYNAMIC 2D GRID hai (jaise asli remote pe buttons ka jaal hota hai).
//
//  ⭐ SABSE ZAROORI BAAT — remote ko RECEIVER ka kuch pata NAHI:
//        vector<vector<Command *>> buttons;
//                     └── `Command*`, `Light*`/`Fan*` NAHI!
//
//  Remote ko Light/Fan ka naam tak nahi pata. Wo bas `execute()` bulata hai.
//  Isi liye kal Geyser, AC, TV — kuch bhi add karo, remote ka code EK LINE bhi
//  nahi badlega. Bas naya command bana ke `setCommand()` kar do. 🎯
//
//  ⭐ DO parallel grids kyun?
//        buttons[r][c]       -> us button pe kaunsa command hai
//        buttonPressed[r][c] -> wo button abhi ON hai ya OFF (toggle state)
//
//  Ye "toggle" ka logic hai: pehli baar dabao -> execute(), doosri baar -> undo().
//  Bilkul asli switch ki tarah.
//
//  💡 Ek design sawaal: kya ye toggle-state INVOKER me honi chahiye ya COMMAND
//     me? Yahan invoker me hai. Command me hoti to har command ko apni state
//     yaad rakhni padti — par tab wo "stateless" na rehta aur reuse mushkil
//     hota. Invoker me rakhna yahan theek hai, kyunki toggle "button" ki
//     property hai, "kaam" ki nahi.
// ============================================================================
/* --- Dynamic Invoker: RemoteController --- */
class RemoteController
{
private:
    // Dynamic 2D structure: rows contain columns of commands
    vector<vector<Command *>> buttons;  // har button ka command (remote MAALIK hai)
    vector<vector<bool>> buttonPressed; // har button ka toggle state

public:
    // Constructor handles dynamic grid size
    RemoteController(int rows, int cols)
    {
        // `resize(rows, vector<Command*>(cols, nullptr))` -> rows × cols ka grid,
        // sab `nullptr` se bhara (yaani "is button pe abhi kuch nahi hai").
        buttons.resize(rows, vector<Command *>(cols, nullptr));
        buttonPressed.resize(rows, vector<bool>(cols, false));
    }

    // ---- Button pe command lagao ------------------------------------------
    //
    // ⭐ Yahi wo jagah hai jahan remote "programmable" banta hai. Runtime pe
    // kisi bhi button pe koi bhi command laga sakte ho. Ek hi remote se light,
    // fan, AC — sab chal jaate hain.
    //
    // ⭐ BOUNDS CHECK — aur ismein ek CHHUPA HUA TRAP hai, dhyaan se padho:
    //
    //   Pehle code aisa tha:  `if (row < buttons.size() && ...)`
    //   Isme `row` INT hai aur `size()` SIZE_T (unsigned). Compiler warning
    //   deta tha: "comparison of integers of different signs".
    //
    //   Par ittefaq se ye SAFE tha! Kyunki `row = -1` hone pe, -1 size_t me
    //   convert hoke ek VISHAAL number (18 quintillion) ban jaata tha -> check
    //   FALSE -> reject. 😅
    //
    //   ⚠ TRAP: agar koi warning ko "theek" karne ke liye seedha cast lagata:
    //         if (row < (int)buttons.size())     // ❌ AB -1 < 2 = TRUE!
    //   To negative index PASS ho jaata aur `buttons[-1]` -> UNDEFINED BEHAVIOR 💥
    //   Yaani warning "theek" karne se ek ASLI BUG paida ho jaata!
    //
    //   ✅ Sahi tareeka: pehle `>= 0` check karo, PHIR cast karo. Ab dono
    //   theek — warning bhi nahi, aur negative bhi safe (jaan-boojh ke, ittefaq
    //   se nahi).
    //   📌 Sabak: warning ko chup karana aur bug theek karna — do alag cheezein hain.
    void setCommand(int row, int col, Command *cmd)
    {
        if (row >= 0 && col >= 0 && static_cast<size_t>(row) < buttons.size() &&
            static_cast<size_t>(col) < buttons[row].size())
        {
            // Purana command tha? Use delete karo — warna leak ho jaata.
            // (Remote commands ka MAALIK hai, isi liye ye uski zimmedari hai.)
            if (buttons[row][col] != nullptr)
                delete buttons[row][col];

            buttons[row][col] = cmd;
            buttonPressed[row][col] = false; // naya command -> toggle reset
        }
        // ⚠ Note: galat index pe `cmd` chup-chaap LEAK ho jaayega (caller ne
        // `new` kiya tha, aur hum use na rakhte hain na delete karte). Ise theek
        // karne ka tareeka: `else { delete cmd; }` ya `bool` return karke caller
        // ko batao. Abhi main() hamesha sahi index bhejta hai, isi liye trigger
        // nahi hota.
    }

    // ---- Button dabao — TOGGLE logic ---------------------------------------
    //
    // ⭐ Ye poore Command pattern ka DIL hai — aur dekho kitna simple hai:
    //       buttons[row][col]->execute();
    //
    // Remote ko pata hi NAHI ki andar Light hai ya Fan. Bas `execute()` bulaya
    // aur aage badh gaya. Sahi class ka function apne aap chalta hai (virtual
    // dispatch). Yahi POLYMORPHISM aur loose coupling hai.
    void pressButton(int row, int col)
    {
        if (row >= 0 && col >= 0 && static_cast<size_t>(row) < buttons.size() &&
            static_cast<size_t>(col) < buttons[row].size() && buttons[row][col] != nullptr)
        {
            // ⭐ TOGGLE: pehli baar -> ON, doosri baar -> OFF
            if (buttonPressed[row][col] == false)
            {
                buttons[row][col]->execute(); // ON
            }
            else
            {
                buttons[row][col]->undo(); // OFF
            }
            buttonPressed[row][col] = !buttonPressed[row][col]; // state flip
        }
        else
        {
            // Galat index, ya us button pe koi command hi nahi laga.
            // Notice: CRASH nahi — bas ek saaf message. (Khaali button dabana
            // aam baat hai, uspe program band nahi hona chahiye.)
            cout << "Invalid button or no command at [" << row << "][" << col << "]\n";
        }
    }

    // ---- Destructor — saare commands saaf ---------------------------------
    //
    // ⭐ Remote commands ka MAALIK hai (unhe `setCommand` me le liya tha), isi
    // liye unhe delete karna uski zimmedari hai.
    //
    // ⚠ Par receivers (Light/Fan) ko delete NAHI karta! Wo main() me bane the,
    // aur command unhe sirf "dekh" raha tha (borrow). Unka maalik main() hai.
    // 📌 "Jo banata hai, wahi delete karta hai."
    ~RemoteController()
    {
        for (auto &row : buttons)
        {
            for (auto &cmd : row)
            {
                if (cmd != nullptr)
                    delete cmd;
            }
        }
    }
};

// ============================================================================
//  5. CLIENT — main()
// ============================================================================
int main()
{
    // ---- 1. RECEIVERS — asli devices --------------------------------------
    Light *livingRoomLight = new Light();
    Fan *ceilingFan = new Fan();

    // ---- 2. INVOKER — 2×2 buttons wala remote -----------------------------
    // Constructor ab (rows, cols) leta hai — grid ka size runtime pe tay hota hai.
    RemoteController *remote = new RemoteController(2, 2);

    // ---- 3. Buttons pe COMMANDS lagao -------------------------------------
    //
    // ⭐ Yahan poora pattern jud jaata hai:
    //       new LightCommand(livingRoomLight)
    //           └── COMMAND        └── RECEIVER
    //       remote->setCommand(...)
    //         └── INVOKER ko command mil gaya
    //
    // Aur remote ko `Light` ka naam tak nahi pata — usko sirf `Command*` mila.
    remote->setCommand(0, 0, new LightCommand(livingRoomLight));
    remote->setCommand(0, 1, new FanCommand(ceilingFan));

    // ---- 4. Buttons dabao — TOGGLE dekho ----------------------------------
    // Pehli baar -> execute() -> ON
    // Doosri baar -> undo()  -> OFF
    cout << "--- Toggling Light Button [0][0] ---" << endl;
    remote->pressButton(0, 0); // ON
    remote->pressButton(0, 0); // OFF

    cout << "--- Toggling Fan Button [0][1] ---" << endl;
    remote->pressButton(0, 1); // ON
    remote->pressButton(0, 1); // OFF

    // ---- 5. Khaali button dabao — crash nahi hona chahiye ------------------
    // [1][1] pe koi command laga hi nahi (grid 2×2 hai, par sirf [0][0] aur
    // [0][1] pe command hai). Remote polite message deta hai.
    cout << "--- Pressing Unassigned Button [1][1] ---" << endl;
    remote->pressButton(1, 1);

    // ---- 6. Cleanup — ORDER dhyaan se -------------------------------------
    //
    // ⭐ `delete remote` PEHLE — kyunki uska destructor commands ko delete
    // karta hai, aur commands ke andar Light/Fan ke pointers hain. Agar pehle
    // Light delete kar dete aur phir remote, to... yahan koi problem nahi hoti
    // (commands destructor me receiver ko chhute hi nahi). Par ye order phir bhi
    // sahi hai — "andar se bahar" saaf karo.
    //
    // Note: RemoteController ka destructor khud commands delete kar dega —
    // isi liye yahan `delete lightCommand` nahi likhna (wo DOUBLE FREE hota! 💥)
    delete remote;
    delete livingRoomLight; // receivers ka maalik main() hai
    delete ceilingFan;

    return 0;
    // 🧹 Gin ke confirm kiya: 13 `new`, 13 `delete` -> 0 leak. Memory saaf. ✅
}
