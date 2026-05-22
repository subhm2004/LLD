#include <iostream>
#include <string>

#include "core/GPaySystem.h"
#include "enums/PaymentRail.h"
#include "enums/TransactionStatus.h"
#include "enums/TransactionType.h"

using namespace std;
using namespace gpay_lld;

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
  cout << "====== GPay LLD Demo ======\n\n";

  GPaySystem gpay;

  const string aliceId =
      gpay.registerUser("Alice", "9876543210", "alice@okaxis", "1234");
  const string bobId =
      gpay.registerUser("Bob", "9123456780", "bob@okhdfc", "5678");
  const string merchantId =
      gpay.registerUser("Tea Stall", "9000011122", "teastall@okbiz", "0000");

  gpay.linkBankAccount(aliceId, "HDFC", "XX1234", 5000);
  gpay.linkBankAccount(bobId, "ICICI", "XX5678", 2000);
  gpay.linkBankAccount(merchantId, "SBI", "XX9999", 500);

  cout << "Alice bank balance: " << gpay.getPrimaryBankBalance(aliceId) << "\n";
  cout << "Bob bank balance: " << gpay.getPrimaryBankBalance(bobId) << "\n\n";

  gpay.addBeneficiary(aliceId, "Bob", "bob@okhdfc");

  cout << "--- P2P via UPI ---\n";
  Transaction t1 = gpay.sendMoney(aliceId, "bob@okhdfc", 250, "1234",
                                  PaymentRail::LINKED_BANK, "Lunch", "req_001");
  cout << "TXN " << t1.getTxnId() << " " << statusToString(t1.getStatus())
       << " amount=" << t1.getAmount() << "\n";
  cout << "Alice balance: " << gpay.getPrimaryBankBalance(aliceId) << "\n";
  cout << "Bob balance: " << gpay.getPrimaryBankBalance(bobId) << "\n\n";

  cout << "--- Scan & Pay QR ---\n";
  const string qr = "upi://pay?pa=teastall@okbiz&am=120&tn=Chai";
  Transaction t2 = gpay.scanAndPay(bobId, qr, "5678", PaymentRail::LINKED_BANK);
  cout << "QR TXN " << t2.getTxnId() << " " << typeToString(t2.getType()) << " "
       << statusToString(t2.getStatus()) << "\n";
  cout << "Merchant balance: " << gpay.getPrimaryBankBalance(merchantId)
       << "\n\n";

  cout << "--- Request money ---\n";
  const string reqId =
      gpay.requestMoney(merchantId, "alice@okaxis", 80, "Refund tea");
  cout << "Request created: " << reqId << "\n";
  Transaction t3 = gpay.fulfillMoneyRequest(aliceId, reqId, "1234",
                                            PaymentRail::LINKED_BANK);
  cout << "Fulfilled: " << t3.getTxnId() << " "
       << statusToString(t3.getStatus()) << "\n\n";

  cout << "--- Wallet rail (top-up style pay) ---\n";
  gpay.topUpWallet(aliceId, 200);
  Transaction t4 = gpay.sendMoney(aliceId, "bob@okhdfc", 50, "1234",
                                  PaymentRail::GPAY_WALLET, "From wallet");
  cout << "Wallet TXN " << t4.getTxnId()
       << " Alice wallet=" << gpay.getWalletBalance(aliceId)
       << " Bob wallet=" << gpay.getWalletBalance(bobId) << "\n\n";

  cout << "--- History (Alice) ---\n";
  for (const Transaction &t : gpay.getTransactionHistory(aliceId, 10)) {
    cout << "  " << t.getTxnId() << " " << typeToString(t.getType()) << " "
         << statusToString(t.getStatus()) << " " << t.getAmount() << " -> "
         << t.getPayeeUpi() << "\n";
  }

  cout << "\nHo gyi Transaction Complete Now bye bye.\n";
  return 0;
}
