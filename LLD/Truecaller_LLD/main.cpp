// ============================================================================
//  main.cpp  —  Truecaller LLD ka DEMO
// ----------------------------------------------------------------------------
//  Dekho main sirf FACADE (`TruecallerSystem`) se baat karta hai — 6 services
//  aur strategy me se ek ka bhi naam yahan nahi hai. Yahi Facade ka point.
//
//  Demo ke hisse:
//     1. Register (2 aam user + 1 business)
//     2. ⭐ CALLER ID — anjaan number vs jaana-pehchana number
//     3. ⭐ Contact sync — local naam bacha rehta hai, directory naam ADD hota hai
//     4. ⭐ Spam report — profile REGISTER ke bina hi ban jaata hai
//     5. Block — personal faisla (sirf block karne wale ko dikhta hai)
//     6. Search — naam prefix se
//     7. Call log — newest pehle
//     8. ⭐ SAFETY DEMOS — teen fixed bugs ka live proof
//
//  📌 `tagToString`/`callTypeToString` yahan hain, enum files me nahi — kyunki
//     ye PRESENTATION hai. Enum ko ye nahi pata hona chahiye ki use kaise
//     chhaapa jayega (UI, JSON, log — sabka format alag).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

#include "core/TruecallerSystem.h"
#include "enums/CallType.h"
#include "enums/CallerTag.h"
#include "enums/ReportReason.h"
#include "models/ContactEntry.h"

using namespace std;
using namespace truecaller_lld;

// ⭐ `default:` nahi — nayi enum value add karo to compiler warning dega.
static string tagToString(CallerTag tag) {
    switch (tag) {
        case CallerTag::PERSONAL:
            return "PERSONAL";
        case CallerTag::BUSINESS:
            return "BUSINESS";
        case CallerTag::SPAM:
            return "SPAM";
        case CallerTag::TELEMARKETER:
            return "TELEMARKETER";
        case CallerTag::UNKNOWN:
            return "UNKNOWN";
    }
    return "UNKNOWN";
}

static string callTypeToString(CallType type) {
    switch (type) {
        case CallType::INCOMING:
            return "INCOMING";
        case CallType::OUTGOING:
            return "OUTGOING";
        case CallType::MISSED:
            return "MISSED";
    }
    return "INCOMING";
}

// Caller ID screen jaisa output — teeno flags saath me dikhte hain.
static void printLookup(const CallerLookupResult& r) {
    cout << "  phone=" << r.phone << " name=" << r.displayName << " tag=" << tagToString(r.tag)
         << " spamScore=" << r.spamScore << " isSpam=" << (r.isSpam ? "yes" : "no")
         << " blocked=" << (r.isBlocked ? "yes" : "no")
         << " inDirectory=" << (r.foundInDirectory ? "yes" : "no") << "\n";
}

int main() {
    cout << "====== Truecaller LLD Demo ======\n\n";

    TruecallerSystem system;

    // ---- (1) Registration --------------------------------------------------
    // Register hote hi directory me PhoneProfile bhi ban jaata hai (tag
    // PERSONAL/BUSINESS ke saath) — isi liye ye numbers lookup me "jaane-pehchane" hain.
    const string aliceId =
        system.registerUser("Alice Sharma", "9876543210", "alice@example.com");
    const string bobId = system.registerUser("Bob Telecom", "9123456780", "", true); // business
    system.registerUser("Ravi Kumar", "9988776655");

    cout << "Registered Alice: " << aliceId << "\n";
    cout << "Registered Bob (business): " << bobId << "\n\n";

    // ---- (2) ⭐ CALLER ID — app ka core feature ---------------------------
    cout << "--- Caller ID lookup ---\n";
    // Anjaan number: directory me hai hi nahi -> inDirectory=no, name=Unknown
    CallerLookupResult unknown = system.identifyCaller(aliceId, "9000011122");
    cout << "Unknown number for Alice:\n";
    printLookup(unknown);

    // Jaana-pehchana number: Ravi registered hai -> naam + PERSONAL tag
    CallerLookupResult known = system.identifyCaller(aliceId, "9988776655");
    cout << "Known number (Ravi) for Alice:\n";
    printLookup(known);

    // ---- (3) ⭐ Contact sync — ENRICH, replace nahi ------------------------
    cout << "\n--- Contact sync ---\n";
    vector<ContactEntry> uploaded = {ContactEntry("Mom", "9876500001"),   // directory me nahi
                                     ContactEntry("Ravi", "9988776655"), // -> "Ravi Kumar"
                                     ContactEntry("Office", "9123456780")}; // -> "Bob Telecom"
    vector<ContactEntry> enriched = system.syncContacts(aliceId, uploaded);
    for (const ContactEntry& c : enriched) {
        // ⭐ local naam ("Mom"/"Office") HAMESHA bacha rehta hai —
        //    directory naam uske SAATH dikhta hai, uski jagah nahi.
        cout << "  local=" << c.getLocalName() << " phone=" << c.getPhone();
        if (c.hasDirectoryMatch()) {
            cout << " -> directory: " << c.getDirectoryName();
        }
        cout << "\n";
    }

    // ---- (4) ⭐ Spam report — crowd-sourced directory ka demo --------------
    cout << "\n--- Spam report ---\n";
    // 9000011122 REGISTERED NAHI hai — phir bhi report se uska profile ban jaata
    // hai ("Reported Number", score 35 SCAM se). Yahi Truecaller ka core idea hai.
    system.reportSpam(aliceId, "9000011122", ReportReason::SCAM);
    // ⭐ Bob se lookup kar rahe hain — spam score GLOBAL hai, isliye usko bhi dikhta hai
    CallerLookupResult spamCaller = system.identifyCaller(bobId, "9000011122");
    cout << "After spam reports:\n";
    printLookup(spamCaller); // score=35 (<70) -> isSpam abhi bhi "no"

    // ---- (5) Block — PERSONAL faisla ---------------------------------------
    cout << "\n--- Block ---\n";
    system.blockNumber(aliceId, "9000011122");
    // ⭐ Alice ke liye blocked=yes. Bob ke liye wahi number blocked=no rehta
    //    (upar wale output me dekho) — block global nahi hota.
    CallerLookupResult blocked = system.identifyCaller(aliceId, "9000011122");
    printLookup(blocked);

    // ---- (6) Search — naam prefix (case-insensitive) -----------------------
    cout << "\n--- Search ---\n";
    auto byName = system.searchByName("Ali", 5); // "Ali" -> "Alice Sharma"
    cout << "Name prefix 'Ali': " << byName.size() << " hit(s)\n";
    for (const PhoneProfile& p : byName) {
        cout << "  " << p.getDisplayName() << " " << p.getPhone() << "\n";
    }

    // ---- (7) Call log — ⭐ newest sabse UPAR -------------------------------
    cout << "\n--- Call log ---\n";
    system.logCall(aliceId, "9988776655", CallType::INCOMING); // CALL_1 (sabse purani)
    system.logCall(aliceId, "9000011122", CallType::MISSED);   // CALL_2
    system.logCall(aliceId, "9123456780", CallType::OUTGOING); // CALL_3 (sabse nayi)

    // ⭐ Output CALL_3, CALL_2, CALL_1 aayega — teeno ka timestamp SAME second
    //    ka hai, phir bhi order sahi hai (sequence tie-break ki wajah se).
    vector<CallLogEntry> history = system.getCallHistory(aliceId, 5);
    for (const CallLogEntry& entry : history) {
        cout << "  " << entry.getCallId() << " " << callTypeToString(entry.getType()) << " "
             << entry.getRemotePhone() << " ts=" << entry.getTimestampEpochSec() << "\n";
    }

    cout << "\nDemo complete.\n";

    // ==========================================================================
    //  (8) ⭐ SAFETY DEMOS — teen fixed bugs ka LIVE PROOF
    //      (teeno pehle FAIL karte the — design doc me poora writeup hai)
    // ==========================================================================
    cout << "\n====== Safety checks (fixed bugs ka proof) ======\n";

    // --- (a) Call history me NEWEST sabse upar aani chahiye -------------------
    // Pehle: teeno ka timestamp same second -> sort tie-break nahi kar paata
    //        -> insertion order (PURANI pehle) aata tha.
    {
        const bool ok = !history.empty() && history.front().getCallId() == "CALL_3";
        cout << "[1] history ka pehla = " << (history.empty() ? "-" : history.front().getCallId())
             << " -> " << (ok ? "✅ newest pehle" : "❌ oldest pehle") << "\n";
    }

    // --- (b) Register karne se spam reports MITNE nahi chahiye ---------------
    // Attack: spammer ka number report ho jaye, phir wo khud us number se
    // register kar le -> pehle poora profile overwrite hota tha, score 0. 😱
    {
        TruecallerSystem s2;
        const string u = s2.registerUser("Reporter", "9111100001");
        s2.reportSpam(u, "9000099999", ReportReason::SCAM);
        s2.reportSpam(u, "9000099999", ReportReason::SCAM); // score 70 -> SPAM tag
        const int before = s2.identifyCaller(u, "9000099999").spamScore;

        s2.registerUser("Totally Legit Corp", "9000099999", "", true); // spammer register
        const CallerLookupResult after = s2.identifyCaller(u, "9000099999");

        cout << "[2] spamScore register se pehle=" << before << " baad me=" << after.spamScore
             << " tag=" << tagToString(after.tag) << " -> "
             << (after.spamScore == before ? "✅ reports bache (evasion band)"
                                           : "❌ reports mit gaye")
             << "\n";
    }

    // --- (c) Search deterministic aur sorted honi chahiye --------------------
    // Pehle: unordered_map ka random traversal + beech me `break` -> results
    //        arbitrary order me, aur limit lagne pe arbitrary N milte the.
    {
        TruecallerSystem s3;
        const char* names[] = {"Aman Delta", "Aman Bravo", "Aman Echo", "Aman Alpha",
                               "Aman Charlie"};
        for (int i = 0; i < 5; ++i) {
            s3.registerUser(names[i], "900000000" + to_string(i + 1)); // 10 digits
        }
        auto res = s3.searchByName("Aman", 3); // ⭐ sorted ke BAAD top 3
        cout << "[3] top-3: ";
        bool sorted = true;
        for (size_t i = 0; i < res.size(); ++i) {
            cout << "[" << res[i].getDisplayName() << "] ";
            if (i > 0 && res[i - 1].getDisplayName() > res[i].getDisplayName()) {
                sorted = false;
            }
        }
        cout << "-> " << (sorted ? "✅ sorted + deterministic" : "❌ random order") << "\n";
    }

    return 0;
}
