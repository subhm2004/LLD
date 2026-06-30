// ============================================================================
//  FacadePattern.cpp  —  Facade Design Pattern (Structural)
// ----------------------------------------------------------------------------
//  Facade = ek complex subsystem (bahut saari classes) ke upar ek SIMPLE
//  interface. Client ko andar ke saare parts (PowerSupply, CPU, BIOS...) ke
//  saath alag-alag deal nahi karna padta — bas ek hi call `startComputer()`.
//  Example: computer boot karna — peeche bahut steps hain, par client ke liye
//  ek button. Fayda: client aur subsystem ke beech loose coupling.
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
