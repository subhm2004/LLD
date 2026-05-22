#include <iostream>
#include <string>

#include "core/TruecallerSystem.h"
#include "enums/CallType.h"
#include "enums/CallerTag.h"
#include "enums/ReportReason.h"
#include "models/ContactEntry.h"

using namespace std;
using namespace truecaller_lld;

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

static void printLookup(const CallerLookupResult& r) {
    cout << "  phone=" << r.phone << " name=" << r.displayName << " tag=" << tagToString(r.tag)
         << " spamScore=" << r.spamScore << " isSpam=" << (r.isSpam ? "yes" : "no")
         << " blocked=" << (r.isBlocked ? "yes" : "no")
         << " inDirectory=" << (r.foundInDirectory ? "yes" : "no") << "\n";
}

int main() {
    cout << "====== Truecaller LLD Demo ======\n\n";

    TruecallerSystem system;

    const string aliceId =
        system.registerUser("Alice Sharma", "9876543210", "alice@example.com");
    const string bobId = system.registerUser("Bob Telecom", "9123456780", "", true);
    system.registerUser("Ravi Kumar", "9988776655");

    cout << "Registered Alice: " << aliceId << "\n";
    cout << "Registered Bob (business): " << bobId << "\n\n";

    cout << "--- Caller ID lookup ---\n";
    CallerLookupResult unknown = system.identifyCaller(aliceId, "9000011122");
    cout << "Unknown number for Alice:\n";
    printLookup(unknown);

    CallerLookupResult known = system.identifyCaller(aliceId, "9988776655");
    cout << "Known number (Ravi) for Alice:\n";
    printLookup(known);

    cout << "\n--- Contact sync ---\n";
    vector<ContactEntry> uploaded = {ContactEntry("Mom", "9876500001"),
                                     ContactEntry("Ravi", "9988776655"),
                                     ContactEntry("Office", "9123456780")};
    vector<ContactEntry> enriched = system.syncContacts(aliceId, uploaded);
    for (const ContactEntry& c : enriched) {
        cout << "  local=" << c.getLocalName() << " phone=" << c.getPhone();
        if (c.hasDirectoryMatch()) {
            cout << " -> directory: " << c.getDirectoryName();
        }
        cout << "\n";
    }

    cout << "\n--- Spam report ---\n";
    system.reportSpam(aliceId, "9000011122", ReportReason::SCAM);
    CallerLookupResult spamCaller = system.identifyCaller(bobId, "9000011122");
    cout << "After spam reports:\n";
    printLookup(spamCaller);

    cout << "\n--- Block ---\n";
    system.blockNumber(aliceId, "9000011122");
    CallerLookupResult blocked = system.identifyCaller(aliceId, "9000011122");
    printLookup(blocked);

    cout << "\n--- Search ---\n";
    auto byName = system.searchByName("Ali", 5);
    cout << "Name prefix 'Ali': " << byName.size() << " hit(s)\n";
    for (const PhoneProfile& p : byName) {
        cout << "  " << p.getDisplayName() << " " << p.getPhone() << "\n";
    }

    cout << "\n--- Call log ---\n";
    system.logCall(aliceId, "9988776655", CallType::INCOMING);
    system.logCall(aliceId, "9000011122", CallType::MISSED);
    system.logCall(aliceId, "9123456780", CallType::OUTGOING);

    vector<CallLogEntry> history = system.getCallHistory(aliceId, 5);
    for (const CallLogEntry& entry : history) {
        cout << "  " << entry.getCallId() << " " << callTypeToString(entry.getType()) << " "
             << entry.getRemotePhone() << " ts=" << entry.getTimestampEpochSec() << "\n";
    }

    cout << "\nDemo complete.\n";
    return 0;
}
