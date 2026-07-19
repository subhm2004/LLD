// ============================================================================
//  BuilderWithDirector.cpp  —  BUILDER + DIRECTOR variant
// ----------------------------------------------------------------------------
//  Director = "ek reusable RECIPE class jo builder ke steps ko ek FIXED
//              sequence me call karti hai — taaki common request types
//              banane ka tareeka EK jagah likha ho, har client me
//              copy-paste na ho."
//
//  Asli duniya ka example: restaurant ka combo menu 🍔
//    - Builder = kitchen (kuch bhi bana sakta hai, step-by-step)
//    - Director = combo menu ("Meal #2" bolo -> fixed recipe se banega)
//    - Custom order dena ho to seedha kitchen (builder) se bhi bol sakte ho!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  DIRECTOR KYUN? — bina iske kya hota:                                   │
//  │                                                                          │
//  │    // "JSON POST request" har jagah aise banta:                         │
//  │    // File A me:                                                        │
//  │    HttpRequestBuilder().withUrl(u).withMethod("POST")                   │
//  │        .withHeader("Content-Type", "application/json")                  │
//  │        .withHeader("Accept", "application/json").withBody(b).build();   │
//  │    // File B me: WAHI 5 lines phir se...                                │
//  │    // File C me: WAHI 5 lines, par kisi ne Accept header BHOOL diya! 💀 │
//  │                                                                          │
//  │  Recipe har jagah copy-paste = duplication + inconsistency ka khatra.   │
//  │                                                                          │
//  │  DIRECTOR se: recipe EK jagah —                                         │
//  │    HttpRequestDirector::createJsonPostRequest(url, body);   // bas!     │
//  │  Recipe badli (naya header add karna hai)? -> EK jagah edit, sab        │
//  │  callers ko naya version apne aap milega. ✅                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES is file me:
//    1. Product   -> HttpRequest         : final object (private ctor)
//    2. Builder   -> HttpRequestBuilder  : steps + build() — "KAISE banana hai"
//    3. Director  -> HttpRequestDirector : recipes — "KYA banana hai, kis
//                                          order me" (builder ko use karta hai)
//    4. Client    -> main()              : ya to Director ki recipe use kare,
//                                          ya seedha builder se custom banaye
//
//  ============================================================================
//   DIRECTOR KI RECIPES — is file me do ready-made recipes:
//  ----------------------------------------------------------------------------
//   Recipe                  | Kya set karti hai                | Kab use karo
//   ------------------------+----------------------------------+---------------
//   createGetRequest(url)   | url + method=GET                 | simple GET —
//                           |                                  |  2 hi cheezein
//   createJsonPostRequest   | url + method=POST +              | JSON API POST —
//    (url, jsonBody)        |  Content-Type + Accept headers + |  standard combo
//                           |  body                            |
//
//   📌 SABSE BADA RULE — BUILDER banata hai, DIRECTOR order karta hai:
//   Director khud kuch NAHI banata — wo bas builder ke steps ko sahi
//   sequence me call karta hai. Separation of concerns:
//     Builder  = "kaise banana hai" ka expert (construction)
//     Director = "kya-kya lagana hai" ka expert (recipe/sequence)
//   Aur dono OPTIONAL bhi hain — custom request chahiye to client seedha
//   builder use kar sakta hai (main me pehla example yahi hai!).
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using namespace std;

// ============================================================================
//  SECTION 1: PRODUCT — HttpRequest (same as BuilderPattern.cpp)
// ----------------------------------------------------------------------------
//  Private constructor + friend builder — object sirf builder se banega,
//  aur ban jaane ke baad immutable rahega.
// ============================================================================
class HttpRequest {
private:
    string url;
    string method;
    map<string, string> headers;
    map<string,string> queryParams;
    string body;
    int timeout; // in seconds

    // Private constructor — sirf builder (friend) access kar sakta hai
    HttpRequest() { }

public:
    friend class HttpRequestBuilder;

    // Request execute — validation ki zaroorat nahi, build() kar chuka hai
    void execute() {
        cout << "Executing " << method << " request to " << url << endl;

        if (!queryParams.empty()) {
            cout << "Query Parameters:" << endl;
            for (const auto& param : queryParams) {
                cout << "  " << param.first << "=" << param.second << endl;
            }
        }

        cout << "Headers:" << endl;
        for (const auto& header : headers) {
            cout << "  " << header.first << ": " << header.second << endl;
        }

        if (!body.empty()) {
            cout << "Body: " << body << endl;
        }

        cout << "Timeout: " << timeout << " seconds" << endl;
        cout << "Request executed successfully!" << endl;
    }
};

// ============================================================================
//  SECTION 2: BUILDER — HttpRequestBuilder (same fluent style)
// ----------------------------------------------------------------------------
//  Har with*() method `return *this;` karta hai -> method chaining.
//  Detail explanation ke liye BuilderPattern.cpp dekho — yahan focus
//  Director pe hai.
// ============================================================================
class HttpRequestBuilder {
private:
    HttpRequest req; // ban-raha-hua product

public:
    // Fluent methods — builder ka reference wapas => chain possible
    HttpRequestBuilder& withUrl(const string& u) {
        req.url = u; return *this;
    }

    HttpRequestBuilder& withMethod(string method) {
        req.method = method;
        return *this;
    }

    HttpRequestBuilder& withHeader(const string& key, const string& value) {
        req.headers[key] = value;
        return *this;
    }

    HttpRequestBuilder& withQueryParams(const string& key, const string& value) {
        req.queryParams[key] = value;
        return *this;
    }

    HttpRequestBuilder& withBody(const string& body) {
        req.body = body;
        return *this;
    }

    HttpRequestBuilder& withTimeout(int timeout) {
        req.timeout = timeout;
        return *this;
    }

    // Terminating method — validation + final product
    HttpRequest build() {
        if (req.url.empty()) {
            throw runtime_error("URL cannot be empty");
        }
        return req;
    }
};

// ============================================================================
//  SECTION 3: DIRECTOR — HttpRequestDirector (is file ka naya kirdaar!)
// ----------------------------------------------------------------------------
//  Common request types ki READY-MADE RECIPES. Har recipe ke andar builder
//  ke steps ka ek FIXED, tested sequence hai.
//
//  Static methods rakhe hain — Director ko koi state nahi chahiye, wo
//  bas recipe execute karta hai. (Chaaho to non-static bhi bana sakte ho
//  jisme builder inject ho — GoF ka classic version waisa hai.)
// ============================================================================
class HttpRequestDirector {
public:
    // RECIPE #1: Simple GET request — sirf url chahiye, baaki recipe me fix
    static HttpRequest createGetRequest(const string& url) {
        return HttpRequestBuilder()
                .withUrl(url)
                .withMethod("GET")
                .build();
    }

    // RECIPE #2: JSON POST request — url + body do, saare standard
    // headers recipe khud laga degi. Ab "Accept header bhool gaye"
    // wala bug ho hi nahi sakta — recipe EK jagah hai, tested hai!
    static HttpRequest createJsonPostRequest(const string& url, const string& jsonBody) {
        return HttpRequestBuilder()
            .withUrl(url)
            .withMethod("POST")
            .withHeader("Content-Type", "application/json")
            .withHeader("Accept", "application/json")
            .withBody(jsonBody)
            .build();
    }
};

// ============================================================================
//  SECTION 4: CLIENT — main() : dono raste dikhaye
// ----------------------------------------------------------------------------
//  Rasta A: custom request -> seedha builder (full control)
//  Rasta B: standard request -> Director ki recipe (ek line, zero galti)
// ============================================================================
int main() {

    // RASTA A: Custom request — Director ki koi recipe fit nahi hoti
    // (query param + custom timeout chahiye), to seedha builder se banao.
    // Director hone ka matlab ye NAHI ki builder direct use nahi kar sakte!
    HttpRequest normalRequest = HttpRequestBuilder()
        .withUrl("https://api.example.com")
        .withMethod("POST")
        .withHeader("Content-Type", "application/json")
        .withHeader("Accept", "application/json")
        .withQueryParams("key", "12345")
        .withBody("{\"name\": \"Aditya\"}")
        .withTimeout(60)
        .build();

        normalRequest.execute(); // build() se aaya = consistent state

    cout <<"\n----------------------------\n";

    // RASTA B1: Standard GET — Director ki recipe, EK line me kaam khatam!
    // Compare karo: builder se yehi banane me 4 lines lagti.
    HttpRequest getRequest = HttpRequestDirector::createGetRequest("https://api.example.com/users");
    getRequest.execute();

    cout <<"\n----------------------------\n";

    // RASTA B2: Standard JSON POST — url + body do, headers ki tension
    // Director ki recipe pe chhod do. Har jagah SAME consistent request!
    HttpRequest postRequest = HttpRequestDirector::createJsonPostRequest(
        "https://api.example.com/users",
        "{\"name\": \"Aditya\", \"email\": \"aditya@example.com\"}");
    postRequest.execute();
}

// ============================================================================
//  EXPECTED OUTPUT (3 requests — custom, GET recipe, JSON POST recipe):
// ----------------------------------------------------------------------------
//  Executing POST request to https://api.example.com
//  Query Parameters:
//    key=12345
//  Headers:
//    Accept: application/json
//    Content-Type: application/json
//  Body: {"name": "Aditya"}
//  Timeout: 60 seconds
//  Request executed successfully!
//
//  ----------------------------
//  Executing GET request to https://api.example.com/users
//  Headers:
//  Timeout: 0 seconds        <- recipe ne timeout set nahi kiya tha!
//  Request executed successfully!
//
//  ----------------------------
//  Executing POST request to https://api.example.com/users
//  Headers:
//    Accept: application/json
//    Content-Type: application/json
//  Body: {"name": "Aditya", "email": "aditya@example.com"}
//  Timeout: 0 seconds
//  Request executed successfully!
//
//  ⚠️ EK SEEKH: GET recipe me timeout set nahi hua to garbage/0 aa gaya —
//  recipe me bhi defaults soch-samajh ke rakhne chahiye (.withTimeout(30)
//  recipe me add karna better hota). Director recipe ki quality = output
//  ki quality!
// ============================================================================
