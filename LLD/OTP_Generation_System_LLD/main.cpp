#include <bits/stdc++.h>

#include "core/OTPGenerationSystem.h"

using namespace std;
using namespace otp_lld;

int main() {
  OTPGenerationSystem system;

  cout << "=== OTP Generation System LLD Demo ===\n\n";

  string otpLogin = system.sendOtp("U1", "+91-8168447388", OTPChannel::SMS,
                                   OTPPurpose::LOGIN);

  cout << "\n--- Wrong OTP attempt ---\n";
  system.verifyOtp(otpLogin, "000000");

  string correctCode = system.getOtpCodeForDemo(otpLogin);
  cout << "\n--- Correct OTP verify ---\n";
  system.verifyOtp(otpLogin, correctCode);

  cout << "\n--- Re-verify same OTP (should fail - already used) ---\n";
  try {
    system.verifyOtp(otpLogin, correctCode);
  } catch (const exception &ex) {
    cout << "Expected: " << ex.what() << "\n";
  }

  cout << "\n--- Status before resend ---\n";
  system.printOtpStatus(otpLogin);

  cout << "\n--- Resend invalidates old pending & sends new ---\n";
  string otpResent = system.resendOtp(otpLogin);
  system.printOtpStatus(otpLogin);
  system.printOtpStatus(otpResent);

  cout << "\n--- Email OTP for payment ---\n";
  string otpPay = system.sendOtp("U1", "user@bank.com", OTPChannel::EMAIL,
                                 OTPPurpose::PAYMENT);

  cout << "\n--- Rate limit demo (3 allowed per 10 min window in code) ---\n";
  try {
    system.sendOtp("U2", "+91-1111111111", OTPChannel::SMS, OTPPurpose::SIGNUP);
    system.sendOtp("U2", "+91-1111111111", OTPChannel::SMS, OTPPurpose::SIGNUP);
    system.sendOtp("U2", "+91-1111111111", OTPChannel::SMS, OTPPurpose::SIGNUP);
    system.sendOtp("U2", "+91-1111111111", OTPChannel::SMS, OTPPurpose::SIGNUP);
  } catch (const exception &ex) {
    cout << "Expected rate limit: " << ex.what() << "\n";
  }

  string payCode = system.getOtpCodeForDemo(otpPay);
  cout << "\n--- Verify payment OTP ---\n";
  system.verifyOtp(otpPay, payCode);

  cout << "\n--- Validation errors ---\n";
  try {
    system.sendOtp("", "+91-000", OTPChannel::SMS, OTPPurpose::LOGIN);
  } catch (const exception &ex) {
    cout << "Expected: " << ex.what() << "\n";
  }

  cout << "\nOTP_Generation_System_LLD demo completed.\n";
  return 0;
}
