// ============================================================================
//  main.cpp  —  GPay LLD ka DEMO
// ----------------------------------------------------------------------------
//  Dekho main sirf FACADE (`GPaySystem`) se baat karta hai — services,
//  strategies, factories me se ek ka bhi naam yahan nahi hai. Yahi Facade ka point.
//
//  Demo ke hisse:
//     1. Register + bank link (setup)
//     2. ⭐ P2P via UPI — dono taraf ka balance badalta hua dikhta hai
//     3. ⭐ Scan & Pay — QR string parse hoke merchant ko paisa
//     4. ⭐ Request money — do-step collect flow (request -> fulfill)
//     5. ⭐ Wallet rail — WAHI sendMoney, sirf rail badla (Strategy ka jaadu)
//     6. History — sent + received dono, newest pehle
//     7. ⭐ SAFETY DEMOS — teen bugs jo fix hue, unka live proof
//
//  📌 `statusToString`/`typeToString` yahan hain, enum files me nahi — kyunki ye
//     PRESENTATION hai. Enum ko ye nahi pata hona chahiye ki use kaise chhaapa
//     jayega (UI, JSON, log — sabka format alag hoga).
// ============================================================================
#include <iostream>
#include <string>

#include "core/GPaySystem.h"
#include "enums/PaymentRail.h"
#include "enums/TransactionStatus.h"
#include "enums/TransactionType.h"

using namespace std;
using namespace gpay_lld;

// ⭐ `default:` nahi — nayi enum value add karo to compiler warning dega.
static string statusToString(TransactionStatus status) {
  switch (status) {
  case TransactionStatus::INITIATED:
    return "INITIATED";
  case TransactionStatus::SUCCESS:
    return "SUCCESS";
  case TransactionStatus::FAILED:
    return "FAILED";
  case TransactionStatus::PENDING:
    return "PENDING";
  }
  return "UNKNOWN";
}

static string typeToString(TransactionType type) {
  switch (type) {
  case TransactionType::P2P_SEND:
    return "P2P_SEND";
  case TransactionType::QR_PAY:
    return "QR_PAY";
  case TransactionType::REQUEST_MONEY:
    return "REQUEST_MONEY";
  case TransactionType::REQUEST_FULFILL:
    return "REQUEST_FULFILL";
  }
  return "UNKNOWN";
}

int main() {
  cout << "====== GPay LLD ======\n\n";

  GPaySystem gpay;

  // ---- (1) Setup: 3 users (2 log + 1 dukaan) --------------------------------
  const string aliceId =
      gpay.registerUser("Alice", "9876543210", "alice@okaxis", "1234");
  const string bobId =
      gpay.registerUser("Bob", "9123456780", "bob@okhdfc", "5678");
  const string merchantId =
      gpay.registerUser("Tea Stall", "9000011122", "teastall@okbiz", "0000");

  // Pehla linked account apne aap PRIMARY ban jaata hai (GPaySystem me).
  gpay.linkBankAccount(aliceId, "HDFC", "XX1234", 5000);
  gpay.linkBankAccount(bobId, "ICICI", "XX5678", 2000);
  gpay.linkBankAccount(merchantId, "SBI", "XX9999", 500);

  cout << "Alice bank balance: " << gpay.getPrimaryBankBalance(aliceId) << "\n";
  cout << "Bob bank balance: " << gpay.getPrimaryBankBalance(bobId) << "\n\n";

  gpay.addBeneficiary(aliceId, "Bob", "bob@okhdfc"); // saved contact

  // ---- (2) P2P via UPI ------------------------------------------------------
  // PIN "1234" verify hoga, "req_001" duplicate-request guard hai.
  cout << "--- P2P via UPI ---\n";
  Transaction t1 = gpay.sendMoney(aliceId, "bob@okhdfc", 250, "1234",
                                  PaymentRail::LINKED_BANK, "Lunch", "req_001");
  cout << "TXN " << t1.getTxnId() << " " << statusToString(t1.getStatus())
       << " amount=" << t1.getAmount() << "\n";
  // ⭐ 5000-250=4750 aur 2000+250=2250 — debit + credit dono hue
  cout << "Alice balance: " << gpay.getPrimaryBankBalance(aliceId) << "\n";
  cout << "Bob balance: " << gpay.getPrimaryBankBalance(bobId) << "\n\n";

  // ---- (3) Scan & Pay -------------------------------------------------------
  // QR string se payee VPA + amount + note nikalte hain (UpiUtils::parseUpiQr).
  cout << "--- Scan & Pay QR ---\n";
  const string qr = "upi://pay?pa=teastall@okbiz&am=120&tn=Chai";
  Transaction t2 = gpay.scanAndPay(bobId, qr, "5678", PaymentRail::LINKED_BANK);
  cout << "QR TXN " << t2.getTxnId() << " " << typeToString(t2.getType()) << " "
       << statusToString(t2.getStatus()) << "\n";
  cout << "Merchant balance: " << gpay.getPrimaryBankBalance(merchantId)
       << "\n\n"; // 500 + 120 = 620

  // ---- (4) Request money (collect flow) -------------------------------------
  // ⭐ Do step: merchant MAANGTA hai (paisa nahi hilta), phir Alice BHARTI hai.
  cout << "--- Request money ---\n";
  const string reqId =
      gpay.requestMoney(merchantId, "alice@okaxis", 80, "Refund tea");
  cout << "Request created: " << reqId << "\n"; // abhi tak paisa nahi hila
  // ⭐ Sirf Alice (payer) hi ise fulfill kar sakti hai — security check facade me.
  Transaction t3 = gpay.fulfillMoneyRequest(aliceId, reqId, "1234",
                                            PaymentRail::LINKED_BANK);
  cout << "Fulfilled: " << t3.getTxnId() << " "
       << statusToString(t3.getStatus()) << "\n\n";

  // ---- (5) ⭐ Wallet rail — STRATEGY ka asli demo ---------------------------
  // Dhyan do: method WAHI `sendMoney` hai. Sirf rail GPAY_WALLET kiya —
  // aur paisa bank ke bajaye wallet se gaya. Ek bhi `if` nahi likhna pada.
  cout << "--- Wallet rail (top-up style pay) ---\n";
  gpay.topUpWallet(aliceId, 200);
  Transaction t4 = gpay.sendMoney(aliceId, "bob@okhdfc", 50, "1234",
                                  PaymentRail::GPAY_WALLET, "From wallet");
  cout << "Wallet TXN " << t4.getTxnId()
       << " Alice wallet=" << gpay.getWalletBalance(aliceId)  // 200-50=150
       << " Bob wallet=" << gpay.getWalletBalance(bobId) << "\n\n"; // 0+50=50

  // ---- (6) History ----------------------------------------------------------
  // ⭐ Newest pehle (TXN_4, TXN_3, TXN_1). TXN_2 nahi dikhta kyunki wo
  //    Bob -> merchant ki thi, Alice usme kahin nahi thi.
  cout << "--- History (Alice) ---\n";
  for (const Transaction &t : gpay.getTransactionHistory(aliceId, 10)) {
    cout << "  " << t.getTxnId() << " " << typeToString(t.getType()) << " "
         << statusToString(t.getStatus()) << " " << t.getAmount() << " -> "
         << t.getPayeeUpi() << "\n";
  }

  cout << "\nHo gyi Transaction Complete Now bye bye.\n";

  // ==========================================================================
  //  (7) ⭐ SAFETY DEMOS — teen fixed bugs ka LIVE PROOF
  //      (in teeno me se koi bhi pehle FAIL karta tha — design doc me detail)
  // ==========================================================================
  cout << "\n====== Safety checks (fixed bugs ka proof) ======\n";

  // --- (a) Debit ho gaya par credit fail -> paisa WAPAS aana chahiye ---------
  // Charlie ne bank link nahi kiya -> uska credit throw karega. Pehle Alice ka
  // paisa kat ke GAYAB ho jaata tha; ab compensating reversal se wapas aata hai.
  {
    const double before = gpay.getPrimaryBankBalance(aliceId);
    gpay.registerUser("Charlie", "9555500001", "charlie@okicici", "1111");
    try {
      gpay.sendMoney(aliceId, "charlie@okicici", 300, "1234",
                     PaymentRail::LINKED_BANK);
    } catch (const exception &ex) {
      cout << "[1] transfer fail (expected): " << ex.what() << "\n";
    }
    const double after = gpay.getPrimaryBankBalance(aliceId);
    cout << "[1] Alice before=" << before << " after=" << after << " -> "
         << (before == after ? "✅ paisa WAPAS (reversal chala)"
                             : "❌ paisa gayab")
         << "\n";
  }

  // --- (b) Failed transfer daily limit nahi khaana chahiye -------------------
  {
    const double before = gpay.getRemainingDailyLimit(bobId);
    try {
      gpay.sendMoney(bobId, "alice@okaxis", 50000, "5678",
                     PaymentRail::LINKED_BANK); // limit ke andar, par balance se kahin zyada
    } catch (const exception &ex) {
      cout << "[2] transfer fail (expected): " << ex.what() << "\n";
    }
    const double after = gpay.getRemainingDailyLimit(bobId);
    cout << "[2] Bob limit before=" << before << " after=" << after << " -> "
         << (before == after ? "✅ limit WAPAS (release chala)"
                             : "❌ limit kat gaya")
         << "\n";
  }

  // --- (c) Fail hone ke baad SAME clientRequestId se retry chalni chahiye ----
  {
    try {
      gpay.sendMoney(bobId, "alice@okaxis", 50000, "5678",
                     PaymentRail::LINKED_BANK, "", "retry_me");
    } catch (const exception &) { /* fail hona hi tha */ }
    try {
      Transaction ok = gpay.sendMoney(bobId, "alice@okaxis", 10, "5678",
                                      PaymentRail::LINKED_BANK, "", "retry_me");
      cout << "[3] retry chali: " << ok.getTxnId()
           << " -> ✅ clientRequestId jala nahi\n";
    } catch (const exception &ex) {
      cout << "[3] retry BLOCKED: " << ex.what() << " -> ❌\n";
    }
  }

  return 0;
}
