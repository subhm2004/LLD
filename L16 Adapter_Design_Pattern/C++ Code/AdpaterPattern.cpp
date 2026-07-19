// ============================================================================
//  AdpaterPattern.cpp  —  ADAPTER DESIGN PATTERN (Structural)
// ----------------------------------------------------------------------------
//  Adapter = "do aisi cheezein jo aapas me FIT nahi hoti, unke beech ek
//             TRANSLATOR laga do."
//
//  Asli duniya ka example: tumhara laptop ka charger 3-pin ka hai, par deewar
//  me 2-pin ka socket hai. Charger badal nahi sakte, socket bhi nahi. To kya
//  karte ho? Beech me ek CONVERTER laga dete ho. 🔌 Bas wahi Adapter hai.
//
//  Yahan code me:
//     Client chahta hai  -> JSON  (`IReports` interface)
//     Legacy class deti hai -> XML  (`XmlDataProvider`)
//     Beech me Adapter    -> XML ko JSON me badal deta hai
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ADAPTER KYUN? — kyunki dono taraf ka code BADAL NAHI SAKTE           │
//  │                                                                          │
//  │  Socho tumne ek third-party library use ki jo XML deti hai. Par tumhara  │
//  │  poora app JSON pe chalta hai. Do "seedhe" hal hain, aur DONO bekaar:    │
//  │                                                                          │
//  │    ❌ Library ka code badlo -> kar hi nahi sakte (tumhari hai hi nahi!)  │
//  │       Aur agar kar bhi lo, to agla update sab uda dega.                  │
//  │                                                                          │
//  │    ❌ Poora app XML pe le aao -> 50 jagah code badalna padega, aur       │
//  │       nayi library aayi jo CSV deti hai? Phir se 50 jagah!               │
//  │                                                                          │
//  │    ✅ ADAPTER: ek chhoti class beech me. Dono taraf ka code CHHUNA hi    │
//  │       nahi padta. Kal CSV wali library aayi? Ek aur adapter bana do.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE CHAAR ROLES (GoF naming) is file me:
//     1. Target (interface) -> IReports              : client ko JO chahiye
//     2. Adaptee (legacy)   -> XmlDataProvider       : jo hai par fit nahi hota
//     3. Adapter            -> XmlDataProviderAdapter : beech ka translator
//     4. Client             -> Client                : sirf Target se baat karta
//
//  ⭐ ASLI JAADU: `Client` ko `XmlDataProvider` ka naam TAK nahi pata! Wo bas
//     `IReports` se baat karta hai. XML kahan se aaya, kaise convert hua — usko
//     kuch pata nahi. Isi liye kal legacy class badal do, client ka code ek line
//     bhi nahi badlega.
//
//  💡 Filename me typo hai ("Adpater" hona chahiye tha "Adapter") — par concept
//     bilkul Adapter hi hai. 😄
// ============================================================================
#include <iostream>
#include <stdexcept> // runtime_error ke liye (input validation me use hota hai)
#include <string>

using namespace std;

// ============================================================================
//  1. TARGET (interface) — client ko JO chahiye
// ----------------------------------------------------------------------------
//  Ye wo interface hai jo client EXPECT karta hai. Client sirf isse baat karega.
//
//  ⭐ Notice: ismein "XML" ka naam tak nahi hai! Ye jaan-boojh ke hai — Target
//  ko ye pata hi nahi hona chahiye ki peeche kya chal raha hai. Kal XML ki jagah
//  CSV aa jaye, to bhi ye interface bilkul same rahega.
// ============================================================================
class IReports {
public:
    // now takes the raw data string and returns JSON
    virtual string getJsonData(const string& data) = 0;

    // Virtual destructor — client `IReports*` (base pointer) me rakhta hai, par
    // asal object `XmlDataProviderAdapter` hai. `delete adapter` BASE pointer se
    // hota hai — iske bina derived ka destructor chalta hi nahi.
    // 📌 RULE: ek bhi virtual function hai? To destructor bhi virtual. Har baar.
    virtual ~IReports() {}
};

// ============================================================================
//  2. ADAPTEE — "legacy" class jo XML deti hai
// ----------------------------------------------------------------------------
//  ⭐ SABSE ZAROORI BAAT — ye class hum BADAL NAHI SAKTE!
//
//  Socho ye kisi third-party library se aayi hai, ya 10 saal purana code hai
//  jise 50 aur jagah use kar rahi hain. Isko chhedna = sab kuch todna.
//
//  Isi liye ismein `IReports` ka naam tak nahi hai — usko pata bhi nahi ki koi
//  usko "adapt" kar raha hai. Aur yahi Adapter pattern ka poora point hai:
//  **adaptee ko chhue bina kaam nikaal lena.**
//
//  📌 Isi wajah se input validation ADAPTER me hai, yahan nahi (neeche dekho) —
//     kyunki legacy class hamari hai hi nahi, use badal nahi sakte.
// ============================================================================
class XmlDataProvider {
public:
    // Expect data in "name:id" format (e.g. "Shubham:124")
    string getXmlData(const string& data) {
        // `find(':')` -> colon kahan hai, uska index.
        // ⚠ Agar colon hai hi nahi to `npos` (ek vishaal number) return hota hai.
        //   Ye class usko handle nahi karti — kyunki ye "legacy" hai aur uska
        //   contract saaf hai: "mujhe hamesha name:id format do".
        //   Us contract ko lagu karwana ADAPTER ka kaam hai (neeche dekho).
        size_t sep = data.find(':');
        string name = data.substr(0, sep);
        string id   = data.substr(sep + 1);
        // Build an XML representation
        return "<user>"
               "<name>" + name + "</name>"
               "<id>"   + id   + "</id>"
               "</user>";
    }
};

// ============================================================================
//  3. ADAPTER — beech ka TRANSLATOR (pattern ka dil)
// ----------------------------------------------------------------------------
//  ⭐ ADAPTER me "IS-A + HAS-A" dono hote hain — bilkul Decorator (L13) ki tarah:
//
//      class XmlDataProviderAdapter : public IReports   // <- IS-A  (Target)
//      {
//          XmlDataProvider* xmlProvider;                // <- HAS-A (Adaptee)
//      };
//
//     IS-A  -> Adapter KHUD ek IReports hai, isi liye client use apna hi
//              samajhta hai (usko shak bhi nahi hota!)
//     HAS-A -> Adapter ke ANDAR legacy class hai, jisse wo asli kaam karwata hai
//
//  📌 DECORATOR vs ADAPTER — dono wrap karte hain, dono me IS-A + HAS-A hai!
//     To farak kya? **NIYAT (intent) ka:**
//        Decorator -> interface SAME rehta hai, feature JODTA hai
//        Adapter   -> interface BADALTA hai (XML -> JSON), feature wahi rehta
//     Interview me ye poocha jaata hai. (L13 ka Markdown.md me poora table hai.)
// ============================================================================
class XmlDataProviderAdapter : public IReports {
private:
    XmlDataProvider* xmlProvider; // ADAPTEE (has-a)
                                  // ⚠ Adapter iska MAALIK nahi hai — bas "dekh"
                                  //   raha hai (borrow). Iska maalik main() hai,
                                  //   wahi delete karega. Isi liye yahan koi
                                  //   destructor nahi hai — agar hota, to main()
                                  //   ka `delete xmlProv` DOUBLE FREE kar deta! 💥
                                  //   📌 "Pointer hone ka matlab maalik hona nahi."
public:
    XmlDataProviderAdapter(XmlDataProvider* provider) {
        this->xmlProvider = provider;
    }

    string getJsonData(const string& data) override {
        // ---- ✅ INPUT VALIDATION — ye ADAPTER ka kaam hai --------------------
        //
        // 🐛 Ye pehle MISSING tha, aur usse ek chup-chaap bug banta tha:
        //    Input "ShubhamNoColon" (bina colon ke) do, to output aata tha:
        //        {"name":"ShubhamNoColon", "id":ShubhamNoColon}
        //                                        └── naam! aur bina quotes ke!
        //    Ye INVALID JSON hai. Crash nahi hota — bas chup-chaap galat data
        //    aage chala jaata. Aisa bug production me hafton chhupa reh sakta hai.
        //
        //    Wajah: `data.find(':')` colon na milne pe `npos` deta hai, phir
        //    `substr(0, npos)` poori string de deta hai, aur `substr(npos+1)`
        //    me npos+1 wrap hoke 0 ban jaata -> phir se poori string!
        //
        // ⭐ Validation YAHAN kyun, `XmlDataProvider` me kyun nahi?
        //    Kyunki adaptee "LEGACY" hai — hum use badal hi nahi sakte (yahi to
        //    poore pattern ki wajah hai!). Adapter HAMARA code hai, to input ko
        //    saaf karna, format check karna — sab uska kaam hai.
        //    📌 Ye Adapter ki ek asli zimmedari hai: legacy ka contract lagu
        //       karwana, taaki usko galat input jaaye hi na.
        if (data.find(':') == string::npos) {
            throw runtime_error("Galat format! 'name:id' chahiye (jaise "
                                "\"Shubham:124\"), mila: \"" + data + "\"");
        }

        // ---- 1. Adaptee se XML lo ------------------------------------------
        string xml = xmlProvider->getXmlData(data);
        // -> "<user><name>Shubham</name><id>124</id></user>"

        // ---- 2. XML se values nikaalo --------------------------------------
        //
        // `find("<name>")` tag ki SHURUAAT ka index deta hai. `+ 6` isliye kyunki
        // "<name>" khud 6 character ka hai — hume tag ke BAAD wali jagah chahiye.
        //     "<user><name>Shubham</name>..."
        //            ^     ^
        //            |     +-- startName (yahan se naam shuru)
        //            +-- find() yahan point karta hai
        //
        // ⚠ Ye "naive parsing" hai — asli XML parser nahi. Agar naam me hi
        //   "<name>" likha ho, ya tags ka order badal jaye, to ye toot jaayega.
        //   Asli project me proper XML library use karo (jaise pugixml). Yahan
        //   simple rakha hai taaki PATTERN dikhe, parsing ka jhanjhat na ho.
        //   (Ye safe hai kyunki XML hamesha upar wale adaptee ne khud banaya hai.)
        size_t startName = xml.find("<name>") + 6;
        size_t endName   = xml.find("</name>");
        string name      = xml.substr(startName, endName - startName);

        size_t startId = xml.find("<id>") + 4; // "<id>" = 4 character
        size_t endId   = xml.find("</id>");
        string id      = xml.substr(startId, endId - startId);

        // ---- 3. JSON bana ke do --------------------------------------------
        //
        // ⭐ YAHI hai poora "adaptation"! Andar XML tha, bahar JSON gaya.
        // Client ko pata bhi nahi chala ki beech me kya hua.
        //
        // `\"` -> quote character (JSON me strings quotes me hoti hain).
        // Note: `id` ko quotes me NAHI daala — kyunki JSON me number quotes ke
        // bina hota hai: {"id":124}, not {"id":"124"}.
        return "{\"name\":\"" + name + "\", \"id\":" + id + "}";
    }
};

// ============================================================================
//  4. CLIENT — sirf Target (IReports) se baat karta hai
// ----------------------------------------------------------------------------
//  ⭐ SABSE ZAROORI BAAT — dekho is class me kya NAHI hai:
//     - `XmlDataProvider` ka naam nahi
//     - "XML" shabd tak nahi
//     - conversion ka koi logic nahi
//
//  Client ko bas `IReports*` mila, usne `getJsonData()` bulaya, JSON mil gaya.
//  Usko pata bhi nahi ki peeche XML tha! Aur yahi poore pattern ka maksad hai.
//
//  📌 Kal legacy class CSV dene lagi? Ek naya `CsvDataProviderAdapter` bana do —
//     Client ka code EK LINE bhi nahi badlega. (Open/Closed Principle ✅)
// ============================================================================
class Client {
public:
    // `IReports*` leta hai — kaunsa adapter hai, isse matlab hi nahi.
    void getReport(IReports* report, string rawData) {
        // ⭐ PEHLE data nikalo, PHIR print karo — order zaroori hai!
        //
        // Pehle ye ek hi statement me tha:
        //     cout << "Processed JSON: " << report->getJsonData(rawData) << endl;
        //
        // Problem: `"Processed JSON: "` PEHLE print ho jaata tha, aur agar
        // `getJsonData()` exception phenkta (galat format pe), to output aisa
        // aadha-adhoora dikhta:
        //     Processed JSON: ✅ Reject: Galat format! ...
        //     └── ye print ho chuka tha, phir error aa gaya 🤦
        //
        // Ab pehle string ban jaati hai (agar throw hua to yahin ruk jaayega,
        // kuch print nahi hoga), phir hi print hoti hai. Saaf output.
        //
        // 📌 Sabak: agar kaam FAIL ho sakta hai, to uska output tabhi likho jab
        //    wo poora ho jaye. Aadha print karke phir fail hona sabse ganda hai.
        string json = report->getJsonData(rawData);
        cout << "Processed JSON: " << json << endl;
    }
};

// ============================================================================
//  5. DEMO
// ============================================================================
int main() {
    // ---- 1. ADAPTEE — legacy class ----------------------------------------
    XmlDataProvider* xmlProv = new XmlDataProvider();

    // ---- 2. ADAPTER — usko lapet do ---------------------------------------
    //
    // ⭐ Notice: `IReports*` me rakha hai, `XmlDataProviderAdapter*` me nahi!
    // Ye jaan-boojh ke hai — ab aage ka poora code sirf INTERFACE jaanta hai.
    // Kal doosra adapter laga do, sirf yahi ek line badlegi.
    IReports* adapter = new XmlDataProviderAdapter(xmlProv);

    // ---- 3. Raw data ------------------------------------------------------
    string rawData = "Shubham:124";

    // ---- 4. Client se report nikalwao -------------------------------------
    Client* client = new Client();
    client->getReport(adapter, rawData);
    // → Processed JSON: {"name":"Shubham", "id":124}
    //
    // ⭐ Client ne JSON maanga, JSON mila. Usko pata bhi nahi ki andar XML bana,
    // phir parse hua, phir JSON bana. Poori translation chhup gayi. 🎯

    // ---- 5. Galat format — ab saaf error milta hai -------------------------
    //
    // 🐛 Pehle ye chup-chaap GALAT JSON de deta tha:
    //        {"name":"BinaColon", "id":BinaColon}   <- invalid JSON!
    //    Ab adapter validation karta hai aur saaf error phenkta hai.
    cout << "\n--- Galat format try karte hain (bina ':' ke) ---" << endl;
    try {
        client->getReport(adapter, "BinaColon");
        cout << "❌ Ye nahi hona chahiye tha!" << endl;
    } catch (const exception& e) {
        cout << "✅ Reject: " << e.what() << endl;
    }

    // ---- 6. Cleanup -------------------------------------------------------
    //
    // ⚠ `delete client` pehle MISSING tha — wo object chup-chaap LEAK ho raha
    //   tha! (Gin ke pakda: 3 `new`, sirf 2 `delete`.)
    //
    // 📌 Order: adapter pehle, phir adaptee. Adapter ke andar `xmlProv` ka
    //    pointer hai, par wo use delete nahi karta (borrow hai). Isi liye yahan
    //    dono ko alag-alag delete karna padta hai — aur ye sahi hai, warna
    //    double-free ho jaata.
    delete client; // ✅ ye pehle chhoot gaya tha
    delete adapter;
    delete xmlProv;
    return 0;
}
