/**
 * Virtual base class — diamond problem ADVANCED
 * - Who constructs the shared virtual base?
 * - Construction / destruction order
 * - vbptr / layout (conceptual)
 */
#include <iostream>
#include <string>
using namespace std;

class Device {
public:
    string id;
    Device() : id("Device-default") {
        cout << "    [Device] default ctor, id=" << id << endl;
    }
    explicit Device(string s) : id(std::move(s)) {
        cout << "    [Device] ctor id=" << id << endl;
    }
    virtual ~Device() {
        cout << "    [Device] dtor id=" << id << endl;
    }
};

// Middle classes: virtual inheritance — they do NOT own Device alone
class Printer : public virtual Device {
public:
    Printer() { cout << "  [Printer] ctor\n"; }
    Printer(string d, string /*ignored for Device from here*/)
        : Device() {  // middle ctor may run but MOST DERIVED wins for virtual base
        cout << "  [Printer] ctor(string) — Device init here is IGNORED if Laptop provides Device\n";
        (void)d;
    }
    ~Printer() override { cout << "  [Printer] dtor\n"; }
};

class Scanner : public virtual Device {
public:
    Scanner() { cout << "  [Scanner] ctor\n"; }
    Scanner(string d, string)
        : Device() {
        cout << "  [Scanner] ctor(string) — virtual Device init skipped if most-derived does it\n";
        (void)d;
    }
    ~Scanner() override { cout << "  [Scanner] dtor\n"; }
};

// Most derived — MUST initialize virtual base Device directly
class MultiFunction : public Printer, public Scanner {
public:
    MultiFunction()
        : Device("shared-MF-device"),   // ✅ only this initializes virtual Device
          Printer(),
          Scanner() {
        cout << "[MultiFunction] ctor — single Device subobject\n";
        cout << "  id via Printer path: " << Printer::id << endl;
    }

    void showLayout() const {
        cout << "  sizeof(Device)=" << sizeof(Device)
             << " sizeof(MultiFunction)=" << sizeof(MultiFunction) << endl;
    }
};

// --- Non-virtual diamond for sizeof compare ---
class D0 {};
class Left : public D0 {};
class Right : public D0 {};
class JoinBad : public Left, public Right {};

class Dv { int x = 0; };
class Lv : public virtual Dv {};
class Rv : public virtual Dv {};
class JoinGood : public Lv, public Rv {
public:
    JoinGood() : Dv() {}
};

int main() {
    cout << "=== ADVANCED: virtual base construction (most-derived initializes) ===\n";
    {
        MultiFunction mf;
        mf.showLayout();
        mf.id = "updated-once";
        cout << "  After single update (one shared Device::id): " << mf.id << endl;
        cout << "Destroying MultiFunction:\n";
    }

    cout << "\n=== sizeof: duplicate base vs virtual base ===\n";
    cout << "  JoinBad  (2x D0):  " << sizeof(JoinBad) << endl;
    cout << "  JoinGood (1x Dv):  " << sizeof(JoinGood) << endl;
    cout << "  MultiFunction:     " << sizeof(MultiFunction) << endl;

    cout << "\n=== Construction order (C++ rules) ===\n";
    cout << "  1) Virtual bases (left-to-right declaration order)\n";
    cout << "  2) Direct non-virtual bases (left-to-right)\n";
    cout << "  3) Members\n";
    cout << "  4) Most-derived constructor body\n";
    cout << "  Destruction: exact reverse\n";

    cout << "\n=== vbptr (implementation detail) ===\n";
    cout << "  Compiler may add hidden pointer(s) to locate virtual base subobject.\n";
    cout << "  Extra indirection vs normal inheritance — trade-off for single shared base.\n";

    return 0;
}
