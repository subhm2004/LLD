// ============================================================================
//  BuilderPattern.cpp  —  Builder Pattern (Creational) : classic fluent builder
// ----------------------------------------------------------------------------
//  Builder = complex object ko step-by-step banao ek fluent API se
//  (.withUrl().withMethod()...), aur build() pe validate karke IMMUTABLE
//  product do. Telescoping constructors aur half-built mutable objects ka
//  problem solve hota hai. Yahan HttpRequest ko HttpRequestBuilder banata hai.
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using namespace std;

class HttpRequest {
private:
    string url;
    string method;
    map<string, string> headers;
    map<string,string> queryParams;
    string body;
    int timeout; // in seconds

    // Private constructor - can only be accessed by the Builder (isse kya hoga ki new keyboard se HttpRequest object nahi banega jse ki hum ne singleton me bhi kiya tha ctor ko private kr diya tha, sirf builder se hi banega)
    HttpRequest() { }

public:
    // Declare the Builder class as a friend (isse kya hoga ki builder class ke methods ko access milega private members ka)
    friend class HttpRequestBuilder;

    // Method to execute the HTTP request
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

class HttpRequestBuilder {
private:    
    HttpRequest req;

public:    
    // Method chaining bolte hai isko 
    // with laga kr naam dena ek namining convention hai builder pattern me, jisse pata chale ki ye method object ko modify kar rahi hai aur builder object return kar rahi hai
    // withUrl, withMethod, withHeader, withQueryParams, withBody, withTimeout methods return karte hai reference to the builder object itself (isse kya hoga ki hum ek hi line me multiple methods call kar sakte hai aur ek fluent API ban jata hai)
    HttpRequestBuilder& withUrl(const string& u) {
        req.url = u; 
        return *this;
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
    
    // Build method to create the immutable HttpRequest object
    HttpRequest build() {
        // Validation logic can be added here
        if (req.url.empty()) {
            throw runtime_error("URL cannot be empty");
        }
        return req;
    }
};


int main() {
    // Using Builder Pattern (nested class)
    HttpRequest request = HttpRequestBuilder()
        .withUrl("https://api.example.com")
        .withMethod("POST")
        .withHeader("Content-Type", "application/json")
        .withHeader("Accept", "application/json")
        .withQueryParams("key", "12345")
        .withBody("{\"name\": \"Aditya\"}")
        .withTimeout(60)
        .build();

    request.execute(); // Guaranteed to be in a consistent state
}

