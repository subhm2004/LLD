// ============================================================================
//  BuilderPattern.cpp  —  BUILDER DESIGN PATTERN (Creational) — classic fluent
// ----------------------------------------------------------------------------
//  Builder = "complex object ko STEP-BY-STEP banao fluent API se
//             (.withUrl().withMethod()...), aur build() pe VALIDATE karke
//             IMMUTABLE product do — na telescoping constructors, na
//             half-built objects."
//
//  Asli duniya ka example: Subway ka sandwich 🥪
//    - Counter pe step-by-step bolte ho: bread choose, veggies choose,
//      sauce choose... har step ka NAAM hai, order flexible hai
//    - Aakhir me "bana do" (build) — TAB sandwich milta hai, poora bana hua
//    - Aadha bana sandwich kabhi tumhare haath me nahi aata!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  PEHLE (WithoutBuilder.cpp) vs AB (ye file):                            │
//  │                                                                          │
//  │    // ❌ PEHLE — 6-arg telescope, order yaad rakho:                     │
//  │    new HttpRequest(url, method, timeout, headers, params, body);        │
//  │                                                                          │
//  │    // ✅ AB — har value ka NAAM, jo chahiye wahi likho, order free:     │
//  │    HttpRequest req = HttpRequestBuilder()                               │
//  │                          .withUrl("https://api.example.com")            │
//  │                          .withMethod("POST")                            │
//  │                          .withBody("{...}")                             │
//  │                          .withTimeout(60)                               │
//  │                          .build();   // <- validation YAHAN hoti hai    │
//  │                                                                          │
//  │  build() se pehle HttpRequest EXIST hi nahi karta — half-built object   │
//  │  use hone ka sawaal hi khatam! Aur build() ke baad wo IMMUTABLE hai.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES is file me:
//    1. Product  -> HttpRequest        : final complex object
//                                        (constructor PRIVATE — sirf builder banaye)
//    2. Builder  -> HttpRequestBuilder : fields step-by-step jama karta hai,
//                                        build() pe validate + product return
//    3. Client   -> main()             : fluent chain likhta hai, ready product leta hai
//
//  ============================================================================
//   ⭐ TEEN C++ TRICKS jo is pattern ko chalati hain (interview gold):
//  ----------------------------------------------------------------------------
//   Trick                    | Kya karti hai
//   -------------------------+------------------------------------------------
//   PRIVATE constructor      | Client `new HttpRequest()` kar hi nahi sakta —
//   (HttpRequest me)         |  object banane ka EKMATRA rasta builder hai
//                            |  (Singleton me bhi yahi trick thi!)
//   friend class             | Builder ko HttpRequest ke private members ka
//   HttpRequestBuilder       |  access milta hai — bina unhe public kiye
//   return *this;            | Har with*() method builder ka reference wapas
//   (har setter me)          |  deta hai -> METHOD CHAINING possible:
//                            |  .withUrl().withMethod().withBody()...
//
//   📌 SABSE BADA RULE — build() hi GATEKEEPER hai:
//   - build() TERMINATING method hai — chain ko khatam karke product deta hai
//   - Validation SIRF yahan hoti hai (scattered nahi): url khaali? method
//     khaali? -> exception TURANT, object banne se PEHLE hi
//   - Isliye jo bhi HttpRequest exist karta hai, wo GUARANTEED valid hai —
//     "make invalid states unrepresentable" wala principle!
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using namespace std;

// ============================================================================
//  SECTION 1: PRODUCT — HttpRequest
// ----------------------------------------------------------------------------
//  Final object jo client ko chahiye. Iske paas KOI public setter nahi —
//  aur constructor bhi private! Matlab: jo bana, wo waisa hi rahega
//  (immutable), aur banega sirf builder ke through.
// ============================================================================
class HttpRequest
{
private:
    string url;
    string method;
    map<string, string> headers;
    map<string, string> queryParams;
    string body;
    int timeout; // in seconds

    // PRIVATE constructor — `new HttpRequest()` bahar se IMPOSSIBLE.
    // Bilkul Singleton wali trick: constructor chhupa do, controlled
    // rasta (yahan: builder) hi ekmatra entry point bane.
    HttpRequest() {}

public:
    // friend = HttpRequestBuilder ko private members (url, method...)
    // ka DIRECT access — bina unhe public kiye. Encapsulation bahar
    // walon ke liye intact, sirf builder ko VIP pass mila hai.
    friend class HttpRequestBuilder;

    // Request execute karo — yahan koi validation ki zaroorat NAHI,
    // kyunki build() pehle hi guarantee de chuka ki object valid hai!
    void execute()
    {
        cout << "Executing " << method << " request to " << url << endl;

        if (!queryParams.empty())
        {
            cout << "Query Parameters:" << endl;
            for (const auto &param : queryParams)
            {
                cout << "  " << param.first << "=" << param.second << endl;
            }
        }

        cout << "Headers:" << endl;
        for (const auto &header : headers)
        {
            cout << "  " << header.first << ": " << header.second << endl;
        }

        if (!body.empty())
        {
            cout << "Body: " << body << endl;
        }

        cout << "Timeout: " << timeout << " seconds" << endl;
        cout << "Request executed successfully!" << endl;
    }
};

// ============================================================================
//  SECTION 2: BUILDER — HttpRequestBuilder
// ----------------------------------------------------------------------------
//  Ye class fields ko step-by-step JAMA karti hai (staging area) aur
//  build() pe validate karke final product deti hai.
//
//  NAMING CONVENTION: "with" prefix (withUrl, withMethod...) builder
//  pattern ki pehchan hai — padhte hi pata chal jaata hai ki ye fluent
//  builder methods hain, normal setters nahi.
// ============================================================================
class HttpRequestBuilder
{
private:
    HttpRequest req; // ban-raha-hua product (private ctor accessible — friend!)

public:
    // -------------------- FLUENT METHODS (method chaining) --------------------
    // HAR method ke end me `return *this;` — matlab "main (builder) hi
    // wapas aa gaya, agla method mujh pe hi call karo." Isi se chain
    // banti hai: builder.withUrl(...).withMethod(...).withBody(...)
    // Return type `HttpRequestBuilder&` (reference) hai — copy nahi banti,
    // same builder object pe hi kaam hota rehta hai.

    HttpRequestBuilder &withUrl(const string &u)
    {
        req.url = u;    // friend hone ki wajah se private field me seedha likh paye
        return *this;   // chaining ke liye khud ko wapas do
    }

    HttpRequestBuilder &withMethod(string method)
    {
        req.method = method;
        return *this;
    }

    // Header ADD hota hai (overwrite nahi) — isliye chain me do baar
    // .withHeader() likh sakte ho, dono headers jud jayenge
    HttpRequestBuilder &withHeader(const string &key, const string &value)
    {
        req.headers[key] = value;
        return *this;
    }

    HttpRequestBuilder &withQueryParams(const string &key, const string &value)
    {
        req.queryParams[key] = value;
        return *this;
    }

    HttpRequestBuilder &withBody(const string &body)
    {
        req.body = body;
        return *this;
    }

    HttpRequestBuilder &withTimeout(int timeout)
    {
        req.timeout = timeout;
        return *this;
    }

    // -------------------- TERMINATING METHOD: build() --------------------
    // Chain ka THE END — ye builder nahi, PRODUCT return karta hai.
    // Aur yahi validation ka EKMATRA gate hai:
    //   - url khaali? -> exception (object banega hi nahi!)
    //   - method khaali? -> exception
    // Is gate se jo bhi HttpRequest nikla, wo 100% valid + consistent hai.
    // WithoutBuilder me ye guarantee KAHIN nahi thi — yahi sabse bada fark hai.
    HttpRequest build()
    {
        if (req.url.empty())
        {
            throw runtime_error("URL cannot be empty");
        }
        if (req.method.empty())
        {
            throw runtime_error("HTTP method cannot be empty");
        }

        return req; // product ki COPY jaati hai — builder baad me bhi reuse ho sakta
    }
};

// ============================================================================
//  SECTION 3: CLIENT — main()
// ----------------------------------------------------------------------------
//  Dekho kitna readable hai — har value ka naam saath me, order ki
//  tension nahi, aur execute() hamesha valid object pe hi chalega.
// ============================================================================
int main()
{
    // Style #1: temporary builder — ek hi expression me sab kuch.
    // HttpRequestBuilder() ek anonymous object hai jo chain khatam hote
    // hi udd jaata hai — sirf product (request) bachta hai.
    HttpRequest request = HttpRequestBuilder()
                              .withUrl("https://api.example.com")
                              .withMethod("POST")
                              .withHeader("Content-Type", "application/json")
                              .withHeader("Accept", "application/json")   // do headers — dono add hue!
                              .withQueryParams("key", "12345")
                              .withBody("{\"name\": \"Shubham\"}")
                              .withTimeout(60)
                              .build();

    request.execute(); // build() se nikla hai = guaranteed consistent state

    // Style #2: named builder (heap pe) — same kaam, bas builder ko
    // variable me rakha. (NOTE: is `builder` ko delete nahi kiya — chhota
    // demo hai isliye chal gaya; production me stack pe banao ya unique_ptr lo.)
    HttpRequestBuilder *builder = new HttpRequestBuilder();
    HttpRequest request2 = builder->withUrl("https://api.example.com")
                               .withMethod("GET")
                               .withHeader("Accept", "application/json")
                               .withQueryParams("key", "67890")
                               .withTimeout(30)
                               .build();
    request2.execute(); // ye bhi guaranteed valid

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  Executing POST request to https://api.example.com
//  Query Parameters:
//    key=12345
//  Headers:
//    Accept: application/json
//    Content-Type: application/json
//  Body: {"name": "Shubham"}
//  Timeout: 60 seconds
//  Request executed successfully!
//  Executing GET request to https://api.example.com
//  Query Parameters:
//    key=67890
//  Headers:
//    Accept: application/json
//  Timeout: 30 seconds
//  Request executed successfully!
//
//  Aage ka safar:
//   -> BuilderWithDirector.cpp : baar-baar wahi recipe? Director me daal do
//   -> StepBuilder.cpp         : steps ka ORDER bhi compile-time pe enforce karo
// ============================================================================
