// ============================================================================
//  WithoutBuilder.cpp  —  BUILDER ke BINA (problem samajhne ke liye)
// ----------------------------------------------------------------------------
//  Ye file jaan-bujh kar DARD wala tareeka dikhati hai, taaki BuilderPattern.cpp
//  ka fix samajh aaye. Pehle problem feel karo!
//
//  SCENARIO: HttpRequest banana hai — 6 fields hain (url, method, headers,
//  queryParams, body, timeout). Kuch required hain, kuch optional.
//  Object banane ke DO tareeke try kiye — dono me alag dard hai:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  TAREEKA #1 — TELESCOPING CONSTRUCTORS (constructor ki seedhi):         │
//  │                                                                          │
//  │    HttpRequest(url)                                    // 1-arg         │
//  │    HttpRequest(url, method)                            // 2-arg         │
//  │    HttpRequest(url, method, timeout)                   // 3-arg         │
//  │    HttpRequest(url, method, timeout, headers)          // 4-arg         │
//  │    HttpRequest(url, method, timeout, headers, params)  // 5-arg         │
//  │    HttpRequest(url, method, timeout, headers, params, body) // 6-arg!   │
//  │                                                                          │
//  │  1) Har combination ke liye NAYA constructor — 6 fields = kitne combos?!│
//  │  2) Same-type args ka order bhoolna easy: ("GET", url) likh diya to?    │
//  │     Compile ho jayega, RUNTIME pe garbage — silent bug! 💀              │
//  │  3) "Sirf url + body chahiye" wala combo hai hi nahi — ya to 6-arg      │
//  │     use karo ya naya constructor likho                                  │
//  │                                                                          │
//  │  TAREEKA #2 — SETTERS (mutable object):                                 │
//  │                                                                          │
//  │    req->setMethod("POST"); req->setBody(...); req->setTimeout(60);      │
//  │                                                                          │
//  │  1) Object har setter ke beech ADHURA (half-built) hai — kisi ne beech  │
//  │     me execute() kar diya to inconsistent state me chal jayega!         │
//  │  2) Koi required field bhoola? Koi check nahi — seedha runtime surprise │
//  │  3) Object kabhi bhi baad me BADLA ja sakta hai (mutable) — thread      │
//  │     safety aur predictability dono gayi                                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ============================================================================
//   TEENO PROBLEMS ka scoreboard (Builder inhe kaise fix karta hai)
//  ----------------------------------------------------------------------------
//   Problem                  | Is file me           | Builder ka fix
//   -------------------------+----------------------+------------------------
//   Telescoping constructors | 6 constructors 😵    | EK fluent chain —
//                            |                      |  jo chahiye wahi set karo
//   Same-type args ka order  | (url, method) vs     | Har value NAAM se jaati
//                            |  (method, url) —     |  hai: .withUrl(), .withMethod()
//                            |  compiler chup       |  — galat jagah possible nahi
//   Half-built object        | Setters ke beech     | Object build() ke BAAD hi
//                            |  adhura object       |  milta hai — pehle exist
//                            |  use ho sakta hai    |  hi nahi karta
//   Required fields bhoolna  | Koi validation nahi  | build() me validation —
//                            |                      |  exception at creation time
//   Mutability               | Setters se kabhi bhi | Product IMMUTABLE —
//                            |  badla ja sakta hai  |  ban gaya to ban gaya
//
//   FIX dekhne ke liye -> BuilderPattern.cpp (classic fluent builder)
//   aur aage:            BuilderWithDirector.cpp, StepBuilder.cpp
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// ============================================================================
//  HttpRequest — bina Builder ke (constructors ka jungle + setters ka khatra)
// ============================================================================
class HttpRequest
{
private:
    string url;    // REQUIRED — par koi enforce nahi kar raha!
    string method; // REQUIRED
    map<string, string> headers;      // optional
    map<string, string> queryParams;  // optional
    string body;                      // optional
    int timeout;   // REQUIRED

public:
    // ------------------ TELESCOPING CONSTRUCTORS ------------------
    // Neeche 6 constructors hain — har naye optional field combination ke
    // liye ek aur. Isi "badhti hui seedhi" ko TELESCOPING kehte hain.
    // 6 fields ke saare combinations chahiye hote to 2^6 = 64 constructors
    // banane padte! Ye scale ho hi nahi sakta.

    // 1-arg: sirf url — baaki sab defaults
    HttpRequest(const string &url)
    {
        this->url = url;
        this->method = "GET"; // Default method
        this->timeout = 30;   // Default timeout
    }

    // 2-arg: url + method
    HttpRequest(const string &url, string method)
    {
        this->url = url;
        this->method = method;
        this->timeout = 30;
    }

    // 3-arg: url + method + timeout
    // DHYAN DO: ab bhi "url + timeout" (bina method) wala combo impossible
    // hai — string,int ka overload string,string se takra jaata!
    HttpRequest(const string &url, string method, int timeout)
    {
        this->url = url;
        this->method = method;
        this->timeout = timeout;
    }

    // 4-arg: + headers
    HttpRequest(const string &url, string method, int timeout, map<string, string> headers)
    {
        this->url = url;
        this->method = method;
        this->timeout = timeout;
        this->headers = headers;
    }

    // 5-arg: + queryParams
    // Yahan naya khatra: headers aur queryParams DONO map<string,string>
    // hain — order ulta kar do to compiler ek word nahi bolega! 💀
    HttpRequest(const string &url, string method, int timeout,
                map<string, string> headers, map<string, string> queryParams)
    {
        this->url = url;
        this->method = method;
        this->timeout = timeout;
        this->headers = headers;
        this->queryParams = queryParams;
    }

    // 6-arg: sab kuch — "the full telescope" 🔭
    // Call karte waqt yaad rakho: url, method, timeout, headers, params, body
    // — ek bhi position idhar-udhar hui to silent bug.
    HttpRequest(const string &url, string method, int timeout,
                map<string, string> headers, map<string, string> queryParams, string body)
    {
        this->url = url;
        this->method = method;
        this->timeout = timeout;
        this->headers = headers;
        this->queryParams = queryParams;
        this->body = body;
    }

    // ------------------ SETTERS (mutable object problem) ------------------
    // Ye setters "flexibility" jaise dikhte hain par asli me KHATRA hain:
    //   1. Inke beech object ADHURA hai — koi bhi use kar sakta hai
    //   2. Object kabhi bhi baad me badla ja sakta hai — immutability zero
    //   3. Required field set karna BHOOL jao to koi nahi rokega
    void setUrl(const string &url)
    {
        this->url = url;
    }

    void setMethod(string method)
    {
        this->method = method;
    }

    void addHeader(const string &key, const string &value)
    {
        headers[key] = value;
    }

    void addQueryParam(const string &key, const string &value)
    {
        queryParams[key] = value;
    }

    void setBody(const string &body)
    {
        this->body = body;
    }

    void setTimeout(int timeout)
    {
        this->timeout = timeout;
    }

    // Request execute karo — par validation kahan hai?!
    // Har jagah (har method me) alag-alag validation karni padegi —
    // "scattered validation" problem. Builder me ye EK jagah hoti hai: build()
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
//  CLIENT CODE — main() : dono dard live dekho
// ============================================================================
int main()
{
    // ---------- DARD #1: Telescoping constructors ----------
    // Teen alag constructors use karne pade — aur imagine karo agar
    // 5-arg wala use karna hota: kaunsa map pehle aata hai, headers ya
    // params? Docs kholne padenge har baar!
    HttpRequest *request1 = new HttpRequest("https://api.example.com");
    HttpRequest *request2 = new HttpRequest("https://api.example.com", "POST");
    HttpRequest *request3 = new HttpRequest("https://api.example.com", "PUT", 60);

    // ---------- DARD #2: Setters (mutable + half-built) ----------
    // In 6 lines ke DAURAAN request4 adhura hai. Agar line 3 ke baad
    // kisi ne request4->execute() kar diya hota? Bina body, bina timeout
    // ke chal jaata — koi rok nahi sakta tha!
    HttpRequest *request4 = new HttpRequest("https://api.example.com");
    request4->setMethod("POST");
    request4->addHeader("Content-Type", "application/json");
    request4->addQueryParam("key", "12345");
    request4->setBody("{\"name\": \"Shubham\"}");
    request4->setTimeout(60);

    // Aur agar upar koi important setter BHOOL jaate? Koi compile error
    // nahi, koi exception nahi — bas galat request chali jaati (inconsistent
    // state problem). Builder me build() ye pakad leta.
    request4->execute();

    delete request1;
    delete request2;
    delete request3;
    delete request4;

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  Executing POST request to https://api.example.com
//  Query Parameters:
//    key=12345
//  Headers:
//    Content-Type: application/json
//  Body: {"name": "Shubham"}
//  Timeout: 60 seconds
//  Request executed successfully!
//
//  Chal to gaya... par sirf isliye ki humne KOI galti nahi ki. Design ne
//  humein galti se BACHAYA nahi — bas dua ke bharose chal raha hai! ❌
//  Fix dekho -> BuilderPattern.cpp
// ============================================================================
