// ============================================================================
//  FacadePattern.cpp  —  FACADE DESIGN PATTERN (Structural)
// ----------------------------------------------------------------------------
//  Facade = "complex subsystem (bahut saari classes) ke UPAR ek SIMPLE
//            interface rakh do — client ko andar ki bheed se matlab nahi,
//            bas EK aasaan button chahiye."
//
//  Asli duniya ka example: computer ka POWER BUTTON 🔘
//    Tum button dabate ho — bas. Peeche kya hota hai? Power supply on,
//    fans start, BIOS boot, CPU init, memory self-test, hard drive spin,
//    OS load... 7 subsystems ka tandav! Par tumhe sirf EK button dikhta hai.
//    Yahi Facade hai — complexity CHHUPTI nahi, bas EK jagah ORGANIZE hoti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  FACADE KYUN? — bina iske client ka haal:                               │
//  │                                                                          │
//  │    // Client ko khud 7 classes + unka SAHI ORDER yaad rakhna padta:     │
//  │    powerSupply.providePower();   // pehle ye...                         │
//  │    coolingSystem.startFans();    // phir ye...                          │
//  │    bios.boot(cpu, memory);       // phir ye (CPU/Memory pass karke!)    │
//  │    hardDrive.spinUp();           // order galat = boot fail!            │
//  │    os.load();                                                           │
//  │                                                                          │
//  │  1) Client 7 subsystems se TIGHTLY COUPLED — koi bhi badla, client toota│
//  │  2) Boot ka sahi ORDER har client ko yaad rakhna padta hai              │
//  │  3) Naya client = wahi 5-line sequence copy-paste (aur galti ka chance) │
//  │                                                                          │
//  │  FACADE se: ComputerFacade ek startComputer() deta hai — order ka       │
//  │  gyaan EK jagah, client sirf facade ko jaanta hai. ✅                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES is file me:
//    1. Subsystems -> PowerSupply, CoolingSystem, CPU, Memory, HardDrive,
//                     BIOS, OperatingSystem  (7 complex parts — inhe koi
//                     change NAHI karna padta, ye pattern se anjaan hain!)
//    2. Facade     -> ComputerFacade  (sabko member banake SAHI ORDER me
//                     call karta hai — startComputer() hi poora sequence)
//    3. Client     -> main()  (sirf facade se baat karta hai)
//
//  ⭐ FACADE vs ADAPTER vs MEDIATOR (interview ka confusion):
//    Facade   = SIMPLIFY karta hai (naya aasaan interface UPAR se)
//    Adapter  = CONVERT karta hai (incompatible interface ko fit karana)
//    Mediator = COORDINATE karta hai (objects ki aapas ki baat-cheet route)
//
//  📌 NOTE: Facade subsystem ko BAND nahi karta — zaroorat ho to client
//  seedha CPU/BIOS access kar SAKTA hai. Facade ek convenience hai,
//  restriction nahi (yahi Proxy se bhi alag banata hai).
// ============================================================================
#include <iostream>
using namespace std;

// ---------- Subsystems (complex parts jo facade ke peeche chhupe hain) ----------
class PowerSupply {
public:
    void providePower() {
        cout << "Power Supply: Providing power..." << endl;
    }
};

class CoolingSystem {
public:
    void startFans() {
        cout << "Cooling System: Fans started..." << endl;
    }
};

class CPU {
public:
    void initialize() {
        cout << "CPU: Initialization started..." << endl;
    }
};

class Memory {
public:
    void selfTest() {
        cout << "Memory: Self-test passed..." << endl;
    }
};

class HardDrive {
public:
    void spinUp() {
        cout << "Hard Drive: Spinning up..." << endl;
    }
};

class BIOS {
public:
    // BIOS khud bhi do subsystems (CPU, Memory) ko coordinate karta hai.
    void boot(CPU& cpu, Memory& memory) {
        cout << "BIOS: Booting CPU and Memory checks..." << endl;
        cpu.initialize();
        memory.selfTest();
    }
};

class OperatingSystem {
public:
    void load() {
        cout << "Operating System: Loading into memory..." << endl;
    }
};

// ---------- Facade ----------
// Saare subsystems ko members ki tarah rakhta hai aur sahi ORDER me call karta
// hai. Client ko sirf yahi class dikhti hai.
class ComputerFacade {
private:
    PowerSupply powerSupply;
    CoolingSystem coolingSystem;
    CPU cpu;
    Memory memory;
    HardDrive hardDrive;
    BIOS bios;
    OperatingSystem os;

public:
    // Ek simple method jo poora complex boot sequence chhupa deta hai.
    void startComputer() {
        cout << "----- Starting Computer -----" << endl;
        powerSupply.providePower();
        coolingSystem.startFans();
        bios.boot(cpu, memory);
        hardDrive.spinUp();
        os.load();
        cout << "Computer Booted Successfully!" << endl;
    }
};

// ---------- Client ----------
int main() {
    // Client ko andar ke 7 subsystems ka kuch nahi pata — bas ek call.
    ComputerFacade* computer = new ComputerFacade();
    computer->startComputer();

    return 0;
}
