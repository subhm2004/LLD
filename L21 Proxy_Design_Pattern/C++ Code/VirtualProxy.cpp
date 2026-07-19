// ============================================================================
//  VirtualProxy.cpp  —  PROXY PATTERN (Structural) : VIRTUAL proxy variant
// ----------------------------------------------------------------------------
//  Proxy = "real object ke saamne ek STAND-IN (dummy) rakho jo SAME interface
//           deta hai aur real object tak access ko CONTROL karta hai. Client
//           ko pata bhi nahi chalta ki wo proxy se baat kar raha hai ya real se."
//
//  L21 me PROXY ke 3 flavours hain (kaam alag, structure same):
//    - VirtualProxy (ye)   -> LAZY LOADING (heavy object ko zaroorat pe banao)
//    - ProtectionProxy     -> ACCESS CONTROL (authorization check)
//    - RemoteProxy         -> LOCATION TRANSPARENCY (remote object local jaisa)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  VIRTUAL PROXY ka kaam — LAZY LOADING:                                  │
//  │                                                                          │
//  │    RealImage banana MEHNGA hai (disk se load, memory... heavy!).        │
//  │    Agar image kabhi DISPLAY hi na ho, to usse banana WASTE hai.         │
//  │                                                                          │
//  │    ImageProxy: constructor me sirf filename yaad rakhta hai — RealImage │
//  │    NAHI banata. Jab pehli baar display() call hota hai TAB RealImage    │
//  │    banata hai (lazy init). Iske baad wahi cached real object use hota.  │
//  │                                                                          │
//  │    Fayda: 100 images ki gallery kholi par sirf 5 dekhi? Sirf 5 hi       │
//  │    actually load hongi — baaki 95 ka heavy kaam bacha! ✅               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES:
//    1. Subject (interface) -> IImage       : common contract (display)
//    2. RealSubject         -> RealImage    : asli heavy object
//    3. Proxy               -> ImageProxy   : stand-in, lazy loading control
//    4. Client              -> main()       : IImage* use karta, proxy pata nahi
//
//  ⭐ PROXY vs DECORATOR (dono wrap karte hain, confusion hota hai):
//    Proxy     = access CONTROL karta hai (kab/kaise real tak jaana) — same
//                interface, SAME behavior (bas controlled)
//    Decorator = behavior ADD karta hai (nayi functionality wrap) — same
//                interface, ENHANCED behavior
// ============================================================================
#include <iostream>
#include <string>

using namespace std;

class IImage {
public:
  virtual void display() = 0;
  virtual ~IImage() = default;
};

class RealImage : public IImage {
  string filename;

public:
  RealImage(string file) {
    this->filename = file;
    // Heavy Operation
    cout << "[RealImage] Loading image from disk: " << filename << "\n";
  }

  void display() override {
    cout << "[RealImage] Displaying " << filename << "\n";
  }
};

class ImageProxy : public IImage {
  RealImage *realImage;  // asli object — abhi nullptr, zaroorat pe banega
  string filename;       // sirf naam yaad rakha (heavy kuch nahi)

public:
  ImageProxy(string file) {
    this->filename = file;
    realImage = nullptr;  // <- KEY: constructor me RealImage NAHI banaya!
                          //    (yahi "lazy" ka matlab — abhi kuch load nahi hua)
  }

  void display() override {
    // LAZY INITIALIZATION — RealImage sirf PEHLI baar display() pe banta hai.
    // Agar display() kabhi call na hota, RealImage kabhi banta hi nahi
    // (heavy disk-load bach jaata). Dusri baar se cached wahi use hoga.
    if (realImage == nullptr) {
      realImage = new RealImage(filename);  // ab (zaroorat pad gayi) banao
    }
    realImage->display();  // asli kaam real object ko delegate
  }
};

int main() {
  // Client ko IImage* mila — usse pata bhi nahi ye proxy hai ya real.
  IImage *image1 = new ImageProxy("yoyo.jpg");
  // Abhi tak "Loading image from disk" NAHI chhpa — kyunki display() nahi hua!
  image1->display();
  // AB output me pehle "Loading..." (lazy create) phir "Displaying..." aayega.
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  [RealImage] Loading image from disk: yoyo.jpg   <- lazy create (display pe!)
//  [RealImage] Displaying yoyo.jpg
//
//  Notice: "Loading" tab hua jab display() call hua, ImageProxy banate waqt
//  NAHI. Yahi lazy loading — heavy kaam last moment tak taala. ✅
// ============================================================================
