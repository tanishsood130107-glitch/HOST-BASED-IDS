#include "PasswordLock.h"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>
#include <string>

namespace
{
    // --------------------------------------------------------
    // SHA-256 (same implementation as the file scanner)
    // --------------------------------------------------------

    class Sha256
    {
    public:
        Sha256() { reset(); }
 
        void reset()
        {
            static const uint32_t init[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
 
            for (int i = 0; i < 8; ++i) 
            {
                state[i] = init[i];
            }
 
            bitLen = 0;
            bufLen = 0;
        }
 
        void update(const unsigned char* data, std::size_t n)
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                buf[bufLen++] = data[i];
 
                if (bufLen == 64)
                {
                    transform(buf);
                    bufLen = 0;
                }
            }
 
            bitLen += (uint64_t)n * 8ULL;
        }
 
        std::string finalHex()
        {
            const uint64_t totalBits = bitLen;
 
            unsigned char pad = 0x80;
            update(&pad, 1);
 
            unsigned char zero = 0;
            while (bufLen != 56)
            {
                update(&zero, 1);
            }
            unsigned char lenBytes[8];
            for (int i = 0; i < 8; ++i)
            {
                lenBytes[i] = (unsigned char)(totalBits >> (56 - 8 * i));
            }
            update(lenBytes, 8);
 
            static const char* hex = "0123456789abcdef";
            std::string out;
 
            for (int i = 0; i < 8; ++i)
            {   
                for (int shift = 28; shift >= 0; shift -= 4)
                {

                   out.push_back(hex[(state[i] >> shift) & 0xF]);
                }
            }
            return out;
        }
 
    private:
        static uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
 
        void transform(const unsigned char* b)
        {
            static const uint32_t K[64] = {
                0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
                0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
                0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
                0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
                0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
                0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
                0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
                0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2 };
 
            uint32_t w[64];
 
            for (int i = 0; i < 16; ++i)
            {
                w[i] = ((uint32_t)b[i * 4] << 24) | ((uint32_t)b[i * 4 + 1] << 16) |
                    ((uint32_t)b[i * 4 + 2] << 8) | (uint32_t)b[i * 4 + 3];
            }
 
            for (int i = 16; i < 64; ++i)
            {
                const uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
                const uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
                w[i] = w[i - 16] + s0 + w[i - 7] + s1;
            }
 
            uint32_t a = state[0], bb = state[1], c = state[2], d = state[3];
            uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
 
            for (int i = 0; i < 64; ++i)
            {
                const uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
                const uint32_t ch = (e & f) ^ (~e & g);
                const uint32_t t1 = h + S1 + ch + K[i] + w[i];
                const uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
                const uint32_t maj = (a & bb) ^ (a & c) ^ (bb & c);
                const uint32_t t2 = S0 + maj;
 
                h = g; g = f; f = e; e = d + t1;
                d = c; c = bb; bb = a; a = t1 + t2;
            }
 
            state[0] += a; state[1] += bb; state[2] += c; state[3] += d;
            state[4] += e; state[5] += f;  state[6] += g; state[7] += h;
        }
 
        uint32_t      state[8];
        uint64_t      bitLen;
        unsigned char buf[64];
        std::size_t   bufLen;
    };

    std::string Sha256Hex(const std::string& text)
    {
        Sha256 sha;
        sha.update(reinterpret_cast<const unsigned char*>(text.data()), text.size());
        return sha.finalHex();
    }

    const int ROUNDS = 30000;

    std::string Stretch(const std::string& salt, const std::string& password)
    {
        std::string h = Sha256Hex(salt + password);

        for (int i = 0; i < ROUNDS; ++i)
            h = Sha256Hex(h + salt + password);

        return h;
    }

    bool SameText(const std::string& a, const std::string& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        unsigned char diff = 0;

        for (std::size_t i = 0; i < a.size(); ++i)
        {
            diff |= static_cast<unsigned char>(a[i] ^ b[i]);
        }

        return diff == 0;
    }

    std::string RandomSalt()
    {
        std::random_device rd;

        std::seed_seq seed{
            rd(), rd(), rd(), rd(),
            static_cast<unsigned>(std::chrono::high_resolution_clock::now()
                .time_since_epoch().count()),
            static_cast<unsigned>(std::chrono::system_clock::now()
                .time_since_epoch().count())
        };

        std::mt19937_64 rng(seed);

        static const char* hex = "0123456789abcdef";
        std::string out;

        for (int i = 0; i < 32; ++i)
            out.push_back(hex[rng() & 0xF]);

        return out;
    }
}

PasswordLock::PasswordLock(const std::string& filePath)
    : path(filePath)
{
    load();
}

void PasswordLock::load()
{
    configured = false;

    std::ifstream in(path);
    std::string header;

    if (!in || !std::getline(in, header) || header != "HIDSLOCK1")
    { 
        return;
    }
    std::string s, h;

    std::getline(in, s);
    std::getline(in, h);

   
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ')) s.pop_back();
    while (!h.empty() && (h.back() == '\r' || h.back() == ' ')) h.pop_back();

    if (s.size() == 32 && h.size() == 64)
    {
        salt       = s;
        hash       = h;
        configured = true;
    }
}

bool PasswordLock::save() const
{
    std::ofstream out(path, std::ios::trunc);

    if (!out)
        return false;

    out << "HIDSLOCK1\n" << salt << "\n" << hash << "\n";
    return static_cast<bool>(out);
}

int PasswordLock::lockoutSecondsLeft() const
{
    const auto now = std::chrono::steady_clock::now();

    if (now >= lockedUntil)
        return 0;

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        lockedUntil - now).count();

    return static_cast<int>((ms + 999) / 1000);
}

PasswordLock::Result PasswordLock::verify(const std::string& password)
{
    if (!configured)
        return Result::NotSet;

    if (lockoutSecondsLeft() > 0)
        return Result::LockedOut;

    if (SameText(Stretch(salt, password), hash))
    {
        failures = 0;
        return Result::Ok;
    }

    ++failures;

    if (failures % 3 == 0)
    {
        const int level = failures / 3;     
        int seconds = 30;

        for (int i = 1; i < level && seconds < 300; ++i)
            seconds *= 2;

        if (seconds > 300)
            seconds = 300;

        lockedUntil = std::chrono::steady_clock::now() +
                      std::chrono::seconds(seconds);
    }

    return Result::Wrong;
}

bool PasswordLock::setPassword(const std::string& newPassword, std::string& message)
{
    if (newPassword.size() < MIN_LENGTH)
    {
        message = "Password must be at least " +
                  std::to_string(MIN_LENGTH) + " characters.";
        return false;
    }

    const std::string oldSalt = salt;
    const std::string oldHash = hash;
    const bool        oldConf = configured;

    salt = RandomSalt();
    hash = Stretch(salt, newPassword);

    if (!save())
    {
        salt       = oldSalt;
        hash       = oldHash;
        configured = oldConf;
        message    = "Could not write the password file.";
        return false;
    }

    configured = true;
    failures   = 0;
    return true;
}