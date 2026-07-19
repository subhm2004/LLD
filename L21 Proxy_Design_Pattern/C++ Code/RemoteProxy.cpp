// ============================================================================
//  RemoteProxy.cpp  —  PROXY PATTERN (Structural) : REMOTE proxy variant
// ----------------------------------------------------------------------------
//  Remote Proxy = "ek LOCAL stand-in jo kisi REMOTE (network/dusre server pe
//                  bethe) object ki taraf se kaam kare. Client ko lage object
//                  local hi hai — networking ka jhamela proxy chhupata hai."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  REMOTE PROXY ka kaam — LOCATION TRANSPARENCY:                          │
//  │                                                                          │
//  │    RealDataService kisi dusre server pe hai (network ke us paar).       │
//  │    Client ko connection banana, request bhejna, response parse karna —  │
//  │    ye sab nahi karna chahiye (complex + har jagah repeat).              │
//  │                                                                          │
//  │    DataServiceProxy LOCAL object hai jo ye sab andar sambhalta hai:     │
//  │    "Connecting to remote..." handle karke RealDataService ko delegate.  │
//  │    Client bas dataService->fetchData() bolta hai — jaise local call!    │
//  │                                                                          │
//  │    Yahi asli duniya me RPC/gRPC/REST client stubs karte hain — network  │
//  │    ki complexity ek local proxy ke peeche chhupi hoti hai. ✅           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES:
//    1. Subject (interface) -> IDataService       : common contract (fetchData)
//    2. RealSubject         -> RealDataService    : "remote" asli service
//    3. Proxy               -> DataServiceProxy   : local stand-in, networking
//    4. Client              -> main()             : IDataService* use karta
//
//  TEENO PROXY EK NAZAR ME (L21 ka summary):
//    Virtual    -> KAB banana (lazy — heavy object zaroorat pe)
//    Protection -> KISKO allow (authorization check)
//    Remote     -> KAHAN se (location transparency — remote ko local jaisa)
//  Structure teeno ka SAME (same interface + wrapped real object), sirf
//  proxy ke andar ka "control logic" alag!
// ============================================================================
#include<iostream>
#include<string>

using namespace std;

class IDataService {
public:
    virtual string fetchData() = 0;
    virtual ~IDataService() = default;
};

class RealDataService : public IDataService {
public:
    RealDataService() {
        // Imagine this connects to a remote server or loads heavy resources.
        cout << "[RealDataService] Initialized (simulating remote setup)\n";
    }
    string fetchData() override {
        return "[RealDataService] Data from server";
    }
};

// Remote proxy — local object jo remote service ko represent karta hai
class DataServiceProxy : public IDataService {
private:
    RealDataService* realService = nullptr;

public:
    DataServiceProxy() {
        realService = new RealDataService();  // "remote" service se link
    }

    string fetchData() override {
        // Yahan networking complexity chhupi hai — connection setup, retries,
        // serialization... (demo me sirf ek print). Client ko is jhamele
        // se matlab nahi, wo bas fetchData() bolta hai.
        cout << "[DataServiceProxy] Connecting to remote service...\n";
        return realService->fetchData();  // asli data remote se laao
    }
};

int main() {
    // Client IDataService* use karta hai — usse pata bhi nahi ki data
    // remote se aa raha hai. Local call jaisa feel (location transparency).
    IDataService* dataService = new DataServiceProxy();
    dataService->fetchData();
    // NOTE: cleanup (delete) demo me skip — production me zaroori.
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  [RealDataService] Initialized (simulating remote setup)
//  [DataServiceProxy] Connecting to remote service...
//
//  (fetchData ka return value use nahi hua isliye print nahi, par flow
//   dikhta hai: proxy banate hi remote setup, phir connect karke fetch.)
// ============================================================================
    