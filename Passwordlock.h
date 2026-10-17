#pragma once

// PasswordLock: one password that protects Logs & Reports and End Task.
//  - The password itself is never stored: only a random salt and a
//    stretched SHA-256 hash, kept in "hids_lock.dat" next to the exe.
//  - After every 3 wrong attempts the lock waits 30 s (doubling up to 5 min).
// No windows.h in here, so it is safe to include anywhere.

#include <chrono>
#include <cstddef>
#include <string>

class PasswordLock
{
public:
    enum class Result { Ok, Wrong, LockedOut, NotSet };

    static constexpr std::size_t MIN_LENGTH = 6;

    explicit PasswordLock(const std::string& filePath = "hids_lock.dat");

    bool isConfigured() const { return configured; }

    // Checks a password (counts failures and applies the lockout).
    Result verify(const std::string& password);

    // Creates or replaces the password. The caller must already have
    // checked the old password when one exists.
    bool setPassword(const std::string& newPassword, std::string& message);

    int lockoutSecondsLeft() const;    // 0 = not locked out
    int failedAttempts() const { return failures; }

private:
    void load();
    bool save() const;

    std::string path;
    std::string salt;
    std::string hash;
    bool        configured = false;

    int failures = 0;
    std::chrono::steady_clock::time_point lockedUntil;
};
 