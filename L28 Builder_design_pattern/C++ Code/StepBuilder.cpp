// ============================================================================
//  StepBuilder.cpp  —  STEP BUILDER variant (Type-State Builder)
// ----------------------------------------------------------------------------
//  Step Builder = "builder ke steps ka ORDER hi COMPILE-TIME pe enforce kar
//                  do — har step SIRF agla valid step return karta hai, to
//                  galat order me likha code COMPILE HI NAHI hota!"
//
//  Classic builder (BuilderPattern.cpp) ki ek kami thi:
//    - .withMethod() bhool gaye? -> build() pe RUNTIME exception
//    - Compile ho gaya, chala, TAB pata chala. Production me? 💥
//  Step Builder me wahi galti COMPILE-TIME error ban jaati hai —
//  IDE me hi laal underline. Sabse safe builder variant!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  MAGIC KAISE? — har step ka return type AGLA step hai:                  │
//  │                                                                          │
//  │    getBuilder()   returns UrlStep&      -> sirf .withUrl() dikhega     │
//  │    withUrl()      returns MethodStep&   -> sirf .withMethod() dikhega  │
//  │    withMethod()   returns HeaderStep&   -> sirf .withHeader() dikhega  │
//  │    withHeader()   returns OptionalStep& -> .withBody()/.withTimeout()/ │
//  │                                            .build() — ab freedom hai   │
//  │                                                                          │
//  │    // Galat order try karo:                                             │
//  │    getBuilder().withMethod("POST")   // ❌ COMPILE ERROR!               │
//  │    // UrlStep me withMethod() hai hi nahi — compiler bolega            │
//  │    // "no member named 'withMethod' in 'UrlStep'"                      │
//  │                                                                          │
//  │  Required steps (url, method, header) SKIP karna IMPOSSIBLE hai —      │
//  │  unke bina build() tak pahunchne ka koi type-path hi nahi bacha!       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES is file me:
//    1. Product        -> HttpRequest            : final object (private ctor)
//    2. Step Interfaces-> UrlStep, MethodStep,   : har ek SIRF apna agla
//                         HeaderStep, OptionalStep  valid step expose karta hai
//    3. Concrete Builder-> HttpRequestStepBuilder : CHAARO interfaces ko
//                          (multiple inheritance!)  implement karta hai
//    4. Client         -> main()                 : chain likhta hai — order
//                                                  compiler khud enforce karega
//
//  ============================================================================
//   STEP TRANSITION TABLE — kaunse step ke baad kya allowed hai
//   (row = abhi kaunsa step/interface, columns = kya call kar sakte ho)
//  ----------------------------------------------------------------------------
//   Current step   | Allowed calls           | Return type    | Skip possible?
//   ---------------+-------------------------+----------------+----------------
//   UrlStep        | withUrl()               | MethodStep&    | ❌ (required)
//   MethodStep     | withMethod()            | HeaderStep&    | ❌ (required)
//   HeaderStep     | withHeader()            | OptionalStep&  | ❌ (required)
//   OptionalStep   | withBody()              | OptionalStep&  | ✅ (optional —
//                  | withTimeout()           | OptionalStep&  |    khud pe hi
//                  | build()                 | HttpRequest    |    loop karta hai)
//
//   ⭐ OptionalStep khud ko hi return karta hai — isliye body/timeout
//   kitni baar bhi, kisi bhi order me, ya bilkul skip — sab chalega.
//   Par url/method/header ke bina yahan tak POHONCH hi nahi sakte!
//
//   📌 MULTIPLE INHERITANCE ka trick:
//   HttpRequestStepBuilder CHAARO step-interfaces se inherit karta hai —
//   matlab wo har stage ka roop le sakta hai. Jab withUrl() `MethodStep&`
//   return karta hai to wo `*this` hi hai — par client ko us waqt SIRF
//   MethodStep ka chashma milta hai, to use sirf withMethod() dikhta hai.
//   Object same, VIEW badalta rehta hai — isi ko Type-State kehte hain!
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using namespace std;

// ============================================================================
//  SECTION 1: PRODUCT — HttpRequest (same as baaki files)
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

    // Private constructor — sirf step-builder (friend) bana sakta hai
    HttpRequest() {}

public:
    friend class HttpRequestStepBuilder;

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
//  SECTION 2: STEP INTERFACES — har ek SIRF agla step dikhata hai
// ----------------------------------------------------------------------------
//  Forward declarations pehle — kyunki UrlStep ka method MethodStep&
//  return karta hai jo abhi neeche define hoga (circular dependency ka
//  standard C++ solution).
// ============================================================================
class MethodStep;
class HeaderStep;
class OptionalStep;

// STEP 1 ka interface: is stage pe SIRF withUrl() call ho sakta hai.
// Aur uska return MethodStep& hai — matlab url dene ke baad hi aage badhoge!
class UrlStep
{
public:
    // Virtual destructor — base (step interface) pointer/reference se
    // kaam ho raha hai, isliye zaroori. `= default` modern C++ style hai
    // (khaali {} body wale se behtar — compiler ko clear intent milta hai).
    virtual ~UrlStep() = default;
    virtual MethodStep &withUrl(const string &url) = 0;
};

// STEP 2 ka interface: url mil chuka, ab SIRF method set ho sakta hai
class MethodStep
{
public:
    virtual ~MethodStep() {}
    virtual HeaderStep &withMethod(string method) = 0;
};

// STEP 3 ka interface: method mil chuka, ab SIRF header
class HeaderStep
{
public:
    virtual ~HeaderStep() {}
    virtual OptionalStep &withHeader(string key, string value) = 0;
};

// STEP 4 ka interface: saare REQUIRED steps ho gaye — ab freedom!
// Dhyan do: withBody/withTimeout OptionalStep& (khud ko) return karte
// hain — isliye ye dono kitni baar bhi, kisi order me, ya skip bhi ho
// sakte hain. Aur build() yahan pehli baar available hua — pehle ke
// kisi step me build() tha hi nahi, isliye adhura build IMPOSSIBLE!
class OptionalStep
{
public:
    virtual ~OptionalStep() {}
    virtual OptionalStep &withBody(const string &body) = 0;
    virtual OptionalStep &withTimeout(int timeout) = 0;
    virtual HttpRequest build() = 0;
};

// ============================================================================
//  SECTION 3: CONCRETE STEP BUILDER — chaaro interfaces ek saath!
// ----------------------------------------------------------------------------
//  MULTIPLE INHERITANCE: ye EK class chaaro steps ka roop hai. Har method
//  `*this` hi return karta hai — par return TYPE ke through client ko
//  sirf agle step ka view milta hai. Object same, chashma alag!
// ============================================================================
class HttpRequestStepBuilder : public UrlStep,
                               public MethodStep,
                               public HeaderStep,
                               public OptionalStep
{

private:
    HttpRequest req; // ban-raha-hua product

public:
    // UrlStep ka implementation — url set karo, MethodStep ka view do
    MethodStep &withUrl(const string &url) override
    {
        req.url = url;
        return *this;   // wahi object, par ab "MethodStep" ke roop me
    }

    // MethodStep ka implementation — method set karo, HeaderStep ka view do
    HeaderStep &withMethod(string method) override
    {
        req.method = method;
        return *this;
    }

    // HeaderStep ka implementation — header set karo, ab OptionalStep unlock!
    OptionalStep &withHeader(string key, string value) override
    {
        req.headers[key] = value;
        return *this;
    }

    // OptionalStep ke implementations — khud ko hi return (free zone)
    OptionalStep &withBody(const string &body) override
    {
        req.body = body;
        return *this;
    }

    OptionalStep &withTimeout(int timeout) override
    {
        req.timeout = timeout;
        return *this;
    }

    // Terminating method — yahan tak pahunche ho matlab url+method+header
    // pakka set hain (type-system ne guarantee di!). Validation ab bhi
    // rakhi hai — defense in depth.
    HttpRequest build() override
    {
        if (req.url.empty())
        {
            throw runtime_error("URL cannot be empty");
        }
        return req;
    }

    // ENTRY POINT: chain yahan se shuru hoti hai. Return type UrlStep& hai
    // — isliye pehla call withUrl() ke alawa KUCH ho hi nahi sakta!
    // (NOTE: `new` kiya hai par kabhi delete nahi hota — memory leak.
    //  Demo ke liye theek; production me static local ya unique_ptr +
    //  by-value chain better hota.)
    static UrlStep &getBuilder()
    {
        return *(new HttpRequestStepBuilder());
    }
};

// ============================================================================
//  SECTION 4: CLIENT — main()
// ----------------------------------------------------------------------------
//  Chain ka order dekho: url -> method -> header -> (optional...) -> build
//  Ye order COMPILER enforce kar raha hai — galat likh ke dekho, compile
//  hi nahi hoga! Try karo: withUrl() se pehle withMethod() likh ke. 😉
// ============================================================================
int main()
{
    HttpRequest stepRequest = HttpRequestStepBuilder::getBuilder()
                                  .withUrl("https://api.example.com/products")   // STEP 1 (required — pehla hona HI tha)
                                  .withMethod("POST")                            // STEP 2 (required)
                                  .withHeader("Content-Type", "application/json")// STEP 3 (required)
                                  .withBody("{\"product\": \"Laptop\", \"price\": 49999}") // optional
                                  .withTimeout(45)                               // optional
                                  .build();                                      // TAB product mila

    stepRequest.execute();
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  Executing POST request to https://api.example.com/products
//  Headers:
//    Content-Type: application/json
//  Body: {"product": "Laptop", "price": 49999}
//  Timeout: 45 seconds
//  Request executed successfully!
//
//  TEENO BUILDER VARIANTS KA COMPARISON (L28 ka safar):
//   Variant              | Required fields ki guarantee | Kab pata chalta hai
//   ---------------------+------------------------------+---------------------
//   BuilderPattern.cpp   | build() me runtime check     | RUNTIME (exception)
//   BuilderWithDirector  | recipe sahi ho to guaranteed | recipe pe depend
//   StepBuilder (ye file)| type-system se enforced      | COMPILE-TIME ✅ best!
//
//  Trade-off: Step Builder sabse safe hai par sabse zyada boilerplate
//  bhi (har step ka interface). Chhote objects pe classic builder kaafi;
//  critical objects (payment request!) pe Step Builder worth it.
// ============================================================================
