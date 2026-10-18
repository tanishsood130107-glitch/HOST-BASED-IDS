#pragma once
#include <chrono>
#include <cstddef>
#include <string>

class PasswordLock
{
private:
    void load();
    bool save() const;

    std::string path;
    std::string salt;
    std::string hash;
    bool        configured = false;

    int failures = 0;
    std::chrono::steady_clock::time_point lockedUntil;

public:
    enum class Result { Ok, Wrong, LockedOut, NotSet };
    static constexpr std::size_t MIN_LENGTH = 6;
    explicit PasswordLock(const std::string& filePath = "hids_lock.dat");
    bool isConfigured() const { return configured; }

    Result verify(const std::string& password);//CHECKS FOR THE PASSWORD

    bool setPassword(const std::string& newPassword, std::string& message);//USED FOR CREATING AND CHANGING THE PASSWORD

    int lockoutSecondsLeft() const;   
    int failedAttempts() const { return failures; }

};
 