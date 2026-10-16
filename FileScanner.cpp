// #ifndef NOMINMAX
// #define NOMINMAX
// #endif
// #ifndef WIN32_LEAN_AND_MEAN
// #define WIN32_LEAN_AND_MEAN
// #endif
 
// #include "FileScanner.h"
 
// #include <windows.h>
 
// #include <algorithm>
// #include <atomic>
// #include <chrono>
// #include <cmath>
// #include <cstdint>
// #include <cstdio>
// #include <filesystem>
// #include <fstream>
// #include <mutex>
// #include <set>
// #include <thread>
// #include <unordered_map>
 
// namespace fs = std::filesystem;
 
// // ------------------------------------------------------------
// // Settings
// // ------------------------------------------------------------
 
// namespace
// {
//     const unsigned long long MAX_CONTENT_BYTES = 4ULL * 1024 * 1024;    // bytes inspected per file
//     const unsigned long long MAX_HASH_BYTES    = 64ULL * 1024 * 1024;   // files larger than this are not hashed
//     const std::size_t        MAX_FINDINGS      = 5000;
 
//     // Heuristic score needed before a file is reported as Suspicious.
//     // Tune this: lower = more detections, more false positives.
//     const int SUSPICIOUS_THRESHOLD = 3;
 
//     // --------------------------------------------------------
//     // String helpers
//     // --------------------------------------------------------
 
//     std::string ToUtf8(const std::wstring& w)
//     {
//         if (w.empty()) return std::string();
 
//         const int len = WideCharToMultiByte(
//             CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
 
//         std::string out(len, '\0');
//         WideCharToMultiByte(
//             CP_UTF8, 0, w.c_str(), (int)w.size(), &out[0], len, nullptr, nullptr);
 
//         return out;
//     }
 
//     std::wstring FromUtf8(const std::string& s)
//     {
//         if (s.empty()) return std::wstring();
 
//         const int len = MultiByteToWideChar(
//             CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
 
//         std::wstring out(len, L'\0');
//         MultiByteToWideChar(
//             CP_UTF8, 0, s.c_str(), (int)s.size(), &out[0], len);
 
//         return out;
//     }
 
//     std::string LowerAscii(std::string s)
//     {
//         for (char& c : s)
//             if (c >= 'A' && c <= 'Z')
//                 c = static_cast<char>(c + 32);
 
//         return s;
//     }
 
//     std::string NowTimeOfDay()
//     {
//         SYSTEMTIME st{};
//         GetLocalTime(&st);
 
//         char text[16];
//         std::snprintf(text, sizeof(text), "%02d:%02d:%02d",
//                       (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
 
//         return text;
//     }
 
//     std::string EnvVar(const char* name)
//     {
//         char buffer[MAX_PATH * 2];
//         const DWORD n = GetEnvironmentVariableA(name, buffer, (DWORD)sizeof(buffer));
 
//         if (n == 0 || n >= sizeof(buffer))
//             return std::string();
 
//         return std::string(buffer, n);
//     }
 
//     // --------------------------------------------------------
//     // SHA-256 (self-contained, no extra libraries)
//     // --------------------------------------------------------
 
//     class Sha256
//     {
//     public:
//         Sha256() { reset(); }
 
//         void reset()
//         {
//             static const uint32_t init[8] = {
//                 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
//                 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
 
//             for (int i = 0; i < 8; ++i) state[i] = init[i];
 
//             bitLen = 0;
//             bufLen = 0;
//         }
 
//         void update(const unsigned char* data, std::size_t n)
//         {
//             for (std::size_t i = 0; i < n; ++i)
//             {
//                 buf[bufLen++] = data[i];
 
//                 if (bufLen == 64)
//                 {
//                     transform(buf);
//                     bufLen = 0;
//                 }
//             }
 
//             bitLen += (uint64_t)n * 8ULL;
//         }
 
//         std::string finalHex()
//         {
//             const uint64_t totalBits = bitLen;
 
//             unsigned char pad = 0x80;
//             update(&pad, 1);
 
//             unsigned char zero = 0;
//             while (bufLen != 56)
//                 update(&zero, 1);
 
//             unsigned char lenBytes[8];
//             for (int i = 0; i < 8; ++i)
//                 lenBytes[i] = (unsigned char)(totalBits >> (56 - 8 * i));
 
//             update(lenBytes, 8);
 
//             static const char* hex = "0123456789abcdef";
//             std::string out;
 
//             for (int i = 0; i < 8; ++i)
//                 for (int shift = 28; shift >= 0; shift -= 4)
//                     out.push_back(hex[(state[i] >> shift) & 0xF]);
 
//             return out;
//         }
 
//     private:
//         static uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
 
//         void transform(const unsigned char* b)
//         {
//             static const uint32_t K[64] = {
//                 0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
//                 0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
//                 0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
//                 0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
//                 0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
//                 0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
//                 0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
//                 0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2 };
 
//             uint32_t w[64];
 
//             for (int i = 0; i < 16; ++i)
//                 w[i] = ((uint32_t)b[i * 4] << 24) | ((uint32_t)b[i * 4 + 1] << 16) |
//                        ((uint32_t)b[i * 4 + 2] << 8) | (uint32_t)b[i * 4 + 3];
 
//             for (int i = 16; i < 64; ++i)
//             {
//                 const uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
//                 const uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
//                 w[i] = w[i - 16] + s0 + w[i - 7] + s1;
//             }
 
//             uint32_t a = state[0], bb = state[1], c = state[2], d = state[3];
//             uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
 
//             for (int i = 0; i < 64; ++i)
//             {
//                 const uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
//                 const uint32_t ch = (e & f) ^ (~e & g);
//                 const uint32_t t1 = h + S1 + ch + K[i] + w[i];
//                 const uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
//                 const uint32_t maj = (a & bb) ^ (a & c) ^ (bb & c);
//                 const uint32_t t2 = S0 + maj;
 
//                 h = g; g = f; f = e; e = d + t1;
//                 d = c; c = bb; bb = a; a = t1 + t2;
//             }
 
//             state[0] += a; state[1] += bb; state[2] += c; state[3] += d;
//             state[4] += e; state[5] += f;  state[6] += g; state[7] += h;
//         }
 
//         uint32_t      state[8];
//         uint64_t      bitLen;
//         unsigned char buf[64];
//         std::size_t   bufLen;
//     };
 
//     // Shannon entropy (0..8). Packed / encrypted data is close to 8.
//     double Entropy(const std::vector<unsigned char>& data, std::size_t limit)
//     {
//         const std::size_t n = std::min(limit, data.size());
//         if (n == 0) return 0.0;
 
//         unsigned long long counts[256] = {};
 
//         for (std::size_t i = 0; i < n; ++i)
//             ++counts[data[i]];
 
//         double entropy = 0.0;
 
//         for (int i = 0; i < 256; ++i)
//         {
//             if (counts[i] == 0) continue;
 
//             const double p = (double)counts[i] / (double)n;
//             entropy -= p * std::log2(p);
//         }
 
//         return entropy;
//     }
 
//     // --------------------------------------------------------
//     // Signatures
//     // --------------------------------------------------------
 
//     // The EICAR test string is built from two pieces so this source file
//     // itself is not flagged by real antivirus products.
//     std::string EicarSignature()
//     {
//         return std::string("X5O!P%@AP[4\\PZX54(P^)7CC)7}$") +
//                "EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
//     }
 
//     const char* const EICAR_SHA256 =
//         "275a021bbfb6489e54d471899f7db9d1663fc695ec2fe2a2c4538aabf651fd0f";
 
//     // Text/byte patterns, matched case-insensitively against the first
//     // 4 MB of a file. Each pattern adds "weight" to the file's score.
//     struct Signature
//     {
//         const char* needle;   // lowercase
//         int         weight;
//         const char* label;
//     };
 
//     const Signature SIGNATURES[] = {
//         // Credential theft tools
//         { "mimikatz",                       5, "Mimikatz credential tool" },
//         { "sekurlsa::logonpasswords",       5, "Credential dumping command" },
//         { "invoke-mimikatz",                5, "Invoke-Mimikatz script" },
 
//         // Ransomware behaviour
//         { "vssadmin delete shadows",        5, "Deletes shadow copies (ransomware behaviour)" },
//         { "wbadmin delete catalog",         4, "Deletes backup catalog" },
//         { "recoveryenabled no",             4, "Disables Windows recovery" },
//         { "all your files have been encrypted", 4, "Ransom note text" },
 
//         // Download-and-run / obfuscated PowerShell
//         { "-encodedcommand",                2, "PowerShell encoded command" },
//         { "downloadstring(",                2, "Downloads and runs remote script" },
//         { "-windowstyle hidden",            2, "Hidden window execution" },
//         { "-executionpolicy bypass",        1, "Bypasses PowerShell policy" },
//         { "frombase64string",               1, "Base64 decoding" },
//         { "net.webclient",                  1, "Web download object" },
//         { "invoke-expression",              1, "Dynamic code execution" },
 
//         // Persistence
//         { "currentversion\\run",            2, "Writes to Run key (persistence)" },
 
//         // Process injection API combination (each worth 1)
//         { "createremotethread",             1, "Process injection API" },
//         { "writeprocessmemory",             1, "Process injection API" },
//         { "virtualallocex",                 1, "Process injection API" },
//     };
 
//     // --------------------------------------------------------
//     // Extension sets
//     // --------------------------------------------------------
 
//     const std::set<std::string> EXECUTABLE_EXT = {
//         ".exe", ".dll", ".scr", ".com", ".bat", ".cmd", ".ps1", ".vbs",
//         ".vbe", ".js", ".jse", ".wsf", ".hta", ".jar", ".msi", ".lnk",
//         ".pif", ".cpl", ".sys"
//     };
 
//     // Extensions that run code when double-clicked
//     const std::set<std::string> RUNNABLE_EXT = {
//         ".exe", ".scr", ".com", ".bat", ".cmd", ".ps1", ".vbs", ".vbe",
//         ".js", ".jse", ".wsf", ".hta", ".pif", ".lnk"
//     };
 
//     // Files that should never contain a program
//     const std::set<std::string> DECOY_EXT = {
//         ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".pdf", ".txt", ".doc",
//         ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".rtf", ".csv", ".mp3",
//         ".mp4", ".log"
//     };
 
//     // Extra types worth reading (macro documents, scripts)
//     const std::set<std::string> EXTRA_SCAN_EXT = {
//         ".py", ".psm1", ".docm", ".xlsm", ".pptm", ".reg", ".sh", ".vb"
//     };
 
//     bool In(const std::set<std::string>& s, const std::string& v)
//     {
//         return s.find(v) != s.end();
//     }
 
//     // --------------------------------------------------------
//     // Evidence collected for one file
//     // --------------------------------------------------------
 
//     struct Evidence
//     {
//         int score = 0;
//         int topWeight = 0;
//         std::string topLabel;
//         std::vector<std::string> labels;
 
//         bool malicious = false;
//         std::string malwareName;
 
//         void add(int weight, const std::string& label)
//         {
//             score += weight;
//             labels.push_back(label);
 
//             if (weight > topWeight)
//             {
//                 topWeight = weight;
//                 topLabel = label;
//             }
//         }
//     };
// }
 
// // ------------------------------------------------------------
// // Impl
// // ------------------------------------------------------------
 
// struct FileScanner::Impl
// {
//     std::atomic<bool> running{ false };
//     std::atomic<bool> cancelFlag{ false };
//     std::atomic<bool> finished{ false };
//     std::atomic<bool> cancelled{ false };
 
//     std::atomic<unsigned long long> filesScanned{ 0 };
//     std::atomic<unsigned long long> filesSkipped{ 0 };
//     std::atomic<unsigned long long> bytesScanned{ 0 };
//     std::atomic<int> malicious{ 0 };
//     std::atomic<int> suspicious{ 0 };
//     std::atomic<double> elapsedFinal{ 0.0 };
 
//     std::chrono::steady_clock::time_point startTime;
 
//     mutable std::mutex mutex;                 // guards the members below
//     std::vector<ScanFinding> findings;
//     std::string currentFile;
//     std::string target;
//     ScanType type = ScanType::Quick;
 
//     std::thread worker;
 
//     // Worker-thread only
//     std::unordered_map<std::string, std::string> knownHashes;   // sha256 -> name
//     std::vector<unsigned char> chunk;
//     std::wstring selfPath;
//     unsigned long long fileCounter = 0;
 
//     Impl() { chunk.resize(1 << 20); }
 
//     // ---- Known-bad hashes ----
 
//     void loadHashes()
//     {
//         knownHashes.clear();
//         knownHashes[EICAR_SHA256] = "EICAR-Test-File";
 
//         // Optional user list: "<sha256> <name>" per line, '#' = comment
//         std::ifstream in("hids_hashes.txt");
//         std::string line;
 
//         while (std::getline(in, line))
//         {
//             if (line.empty() || line[0] == '#') continue;
 
//             const std::size_t space = line.find_first_of(" \t");
//             std::string hash = LowerAscii(line.substr(0, space));
//             std::string name = "Known-Malware";
 
//             if (space != std::string::npos)
//             {
//                 const std::size_t start = line.find_first_not_of(" \t", space);
//                 if (start != std::string::npos)
//                 {
//                     name = line.substr(start);
//                     while (!name.empty() && (name.back() == '\r' || name.back() == ' '))
//                         name.pop_back();
//                 }
//             }
 
//             if (hash.size() == 64)
//                 knownHashes[hash] = name;
//         }
//     }
 
//     // ---- Which folders to scan ----
 
//     std::vector<fs::path> buildRoots(ScanType t, const std::string& customPath)
//     {
//         std::vector<fs::path> roots;
//         std::set<std::string> seen;
 
//         auto addRoot = [&](const fs::path& p)
//         {
//             std::error_code ec;
//             if (p.empty() || !fs::exists(p, ec)) return;
 
//             const std::string key = LowerAscii(ToUtf8(p.wstring()));
//             if (seen.insert(key).second)
//                 roots.push_back(p);
//         };
 
//         if (t == ScanType::Custom)
//         {
//             addRoot(fs::path(FromUtf8(customPath)));
//             return roots;
//         }
 
//         if (t == ScanType::Full)
//         {
//             const DWORD mask = GetLogicalDrives();
 
//             for (int i = 0; i < 26; ++i)
//             {
//                 if (!(mask & (1u << i))) continue;
 
//                 char root[4] = { (char)('A' + i), ':', '\\', '\0' };
 
//                 if (GetDriveTypeA(root) == DRIVE_FIXED)
//                     addRoot(fs::path(root));
//             }
 
//             return roots;
//         }
 
//         // Quick scan
//         const fs::path profile = FromUtf8(EnvVar("USERPROFILE"));
 
//         if (!profile.empty())
//         {
//             addRoot(profile / L"Downloads");
//             addRoot(profile / L"Desktop");
//             addRoot(profile / L"Documents");
//         }
 
//         addRoot(FromUtf8(EnvVar("TEMP")));
//         addRoot(FromUtf8(EnvVar("APPDATA")));
//         addRoot(FromUtf8(EnvVar("LOCALAPPDATA")) / L"Temp");
//         addRoot(FromUtf8(EnvVar("PUBLIC")));
//         addRoot(fs::path(L"C:\\Windows\\Temp"));
//         addRoot(fs::path(L"HIDS_Monitor"));
 
//         return roots;
//     }
 
//     static bool shouldSkipDir(const fs::path& dir)
//     {
//         const std::string p = LowerAscii(ToUtf8(dir.wstring()));
 
//         return p.find("\\windows\\winsxs") != std::string::npos ||
//                p.find("system volume information") != std::string::npos;
//     }
 
//     static bool isReparsePoint(const fs::path& p)
//     {
//         const DWORD attr = GetFileAttributesW(p.c_str());
//         return attr != INVALID_FILE_ATTRIBUTES &&
//                (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
//     }
 
//     // ---- Directory walk (explicit stack, never throws) ----
 
//     void scanRoot(const fs::path& root)
//     {
//         std::error_code ec;
 
//         if (fs::is_regular_file(root, ec))
//         {
//             scanFile(root);
//             return;
//         }
 
//         std::vector<fs::path> stack;
//         stack.push_back(root);
 
//         while (!stack.empty() && !cancelFlag)
//         {
//             const fs::path dir = stack.back();
//             stack.pop_back();
 
//             if (shouldSkipDir(dir))
//                 continue;
 
//             std::error_code dirEc;
//             fs::directory_iterator it(
//                 dir, fs::directory_options::skip_permission_denied, dirEc);
//             const fs::directory_iterator end;
 
//             if (dirEc)
//                 continue;
 
//             for (; it != end && !cancelFlag; it.increment(dirEc))
//             {
//                 if (dirEc) break;
 
//                 const fs::directory_entry& entry = *it;
//                 std::error_code e;
 
//                 if (entry.is_directory(e))
//                 {
//                     if (!isReparsePoint(entry.path()))
//                         stack.push_back(entry.path());
//                 }
//                 else if (entry.is_regular_file(e))
//                 {
//                     scanFile(entry.path());
//                 }
//             }
//         }
//     }
 
//     // ---- Checks based on the name / location only ----
 
//     void checkName(const fs::path& p, const std::string& ext, Evidence& ev)
//     {
//         const std::wstring fileName = p.filename().wstring();
 
//         // Right-to-left override: "invoice_txt.exe" shown as "invoiceexe.txt"
//         if (fileName.find(L'\u202E') != std::wstring::npos)
//             ev.add(4, "Right-to-left override character in file name");
 
//         // Double extension: invoice.pdf.exe
//         if (In(RUNNABLE_EXT, ext))
//         {
//             const std::wstring stem = p.stem().wstring();
//             const std::size_t dot = stem.find_last_of(L'.');
 
//             if (dot != std::wstring::npos)
//             {
//                 const std::string inner = LowerAscii(ToUtf8(stem.substr(dot)));
 
//                 if (In(DECOY_EXT, inner))
//                     ev.add(4, "Double extension (" + inner + ext + ")");
//             }
//         }
 
//         // Location checks (only for files that can run code)
//         if (In(EXECUTABLE_EXT, ext))
//         {
//             const std::string path = LowerAscii(ToUtf8(p.wstring()));
 
//             if (path.find("\\appdata\\local\\temp\\") != std::string::npos ||
//                 path.find("\\windows\\temp\\") != std::string::npos)
//                 ev.add(1, "Executable file in a temp folder");
 
//             if (path.find("\\start menu\\programs\\startup\\") != std::string::npos)
//                 ev.add(2, "Executable in the Startup folder");
 
//             if (path.find("\\users\\public\\") != std::string::npos)
//                 ev.add(1, "Executable in the Public folder");
 
//             const DWORD attr = GetFileAttributesW(p.c_str());
 
//             if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_HIDDEN))
//                 ev.add(2, "Hidden executable file");
//         }
//     }
 
//     // ---- Checks based on the file content ----
 
//     void checkContent(const std::vector<unsigned char>& buf,
//                       const std::string& ext, Evidence& ev)
//     {
//         const std::string raw(buf.begin(), buf.end());
 
//         // 1) EICAR test signature (always Malicious)
//         if (raw.find(EicarSignature()) != std::string::npos)
//         {
//             ev.malicious = true;
//             ev.malwareName = "EICAR-Test-File";
//             ev.add(10, "EICAR antivirus test signature");
//         }
 
//         // 2) Is it really a Windows program? ("MZ" header)
//         const bool isPE = buf.size() >= 2 && buf[0] == 'M' && buf[1] == 'Z';
 
//         if (isPE && In(DECOY_EXT, ext))
//             ev.add(4, "Executable disguised as " + ext + " file");
 
//         if (isPE && Entropy(buf, 1 << 20) >= 7.4)
//             ev.add(2, "High entropy (possibly packed or encrypted)");
 
//         // 3) Suspicious strings (case-insensitive)
//         std::string lower = raw;
//         for (char& c : lower)
//             if (c >= 'A' && c <= 'Z')
//                 c = static_cast<char>(c + 32);
 
//         for (const Signature& sig : SIGNATURES)
//         {
//             if (lower.find(sig.needle) == std::string::npos)
//                 continue;
 
//             ev.add(sig.weight, sig.label);
//         }
//     }
 
//     // ---- One file ----
 
//     void setCurrent(const fs::path& p)
//     {
//         std::lock_guard<std::mutex> lock(mutex);
//         currentFile = ToUtf8(p.wstring());
//     }
 
//     void scanFile(const fs::path& p)
//     {
//         if (cancelFlag) return;
 
//         if ((++fileCounter % 25) == 0)
//             setCurrent(p);
 
//         // Never scan ourselves (our own signature table would match)
//         if (!selfPath.empty() &&
//             LowerAscii(ToUtf8(p.wstring())) == LowerAscii(ToUtf8(selfPath)))
//             return;
 
//         std::error_code ec;
//         const unsigned long long size = fs::file_size(p, ec);
 
//         if (ec)
//         {
//             ++filesSkipped;
//             return;
//         }
 
//         const std::string ext = LowerAscii(ToUtf8(p.extension().wstring()));
 
//         Evidence ev;
//         checkName(p, ext, ev);
 
//         std::string sha256;
 
//         const bool wantContent =
//             size > 0 &&
//             (size <= 256 || In(EXECUTABLE_EXT, ext) ||
//              In(EXTRA_SCAN_EXT, ext) || In(DECOY_EXT, ext));
 
//         if (wantContent)
//         {
//             FILE* f = _wfopen(p.c_str(), L"rb");
 
//             if (f == nullptr)
//             {
//                 ++filesSkipped;
//             }
//             else
//             {
//                 std::vector<unsigned char> buf;
//                 buf.reserve((std::size_t)std::min<unsigned long long>(size, MAX_CONTENT_BYTES));
 
//                 const bool hashing = size <= MAX_HASH_BYTES;
//                 Sha256 sha;
//                 unsigned long long total = 0;
 
//                 while (!cancelFlag)
//                 {
//                     const std::size_t n = std::fread(chunk.data(), 1, chunk.size(), f);
//                     if (n == 0) break;
 
//                     if (buf.size() < MAX_CONTENT_BYTES)
//                     {
//                         const std::size_t take = (std::size_t)std::min<unsigned long long>(
//                             n, MAX_CONTENT_BYTES - buf.size());
//                         buf.insert(buf.end(), chunk.begin(), chunk.begin() + take);
//                     }
 
//                     if (hashing)
//                         sha.update(chunk.data(), n);
 
//                     total += n;
 
//                     // Big file: we only need the first 4 MB
//                     if (!hashing && buf.size() >= MAX_CONTENT_BYTES)
//                         break;
//                 }
 
//                 std::fclose(f);
//                 bytesScanned += total;
 
//                 if (cancelFlag) return;
 
//                 if (hashing && total == size)
//                 {
//                     sha256 = sha.finalHex();
 
//                     const auto known = knownHashes.find(sha256);
//                     if (known != knownHashes.end())
//                     {
//                         ev.malicious = true;
//                         ev.malwareName = known->second;
//                         ev.add(10, "SHA-256 matches known malware (" + known->second + ")");
//                     }
//                 }
 
//                 checkContent(buf, ext, ev);
//             }
//         }
 
//         ++filesScanned;
 
//         // ---- Decide ----
//         if (!ev.malicious && ev.score < SUSPICIOUS_THRESHOLD)
//             return;
 
//         ScanFinding finding;
//         finding.path   = ToUtf8(p.wstring());
//         finding.size   = size;
//         finding.sha256 = sha256;
//         finding.time   = NowTimeOfDay();
//         finding.score  = ev.score;
 
//         for (std::size_t i = 0; i < ev.labels.size(); ++i)
//         {
//             if (i > 0) finding.reason += "; ";
//             finding.reason += ev.labels[i];
//         }
 
//         if (ev.malicious)
//         {
//             finding.level = "Malicious";
//             finding.name  = ev.malwareName;
//             ++malicious;
//         }
//         else
//         {
//             finding.level = "Suspicious";
//             finding.name  = ev.topLabel;
//             ++suspicious;
//         }
 
//         std::lock_guard<std::mutex> lock(mutex);
 
//         if (findings.size() < MAX_FINDINGS)
//             findings.push_back(std::move(finding));
//     }
 
//     // ---- Worker thread ----
 
//     void run(ScanType t, std::string customPath)
//     {
//         SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
 
//         wchar_t exe[MAX_PATH * 2] = {};
//         GetModuleFileNameW(nullptr, exe, (DWORD)(sizeof(exe) / sizeof(exe[0])));
//         selfPath = exe;
 
//         loadHashes();
 
//         const std::vector<fs::path> roots = buildRoots(t, customPath);
 
//         for (const fs::path& root : roots)
//         {
//             if (cancelFlag) break;
//             scanRoot(root);
//         }
 
//         const double elapsed = std::chrono::duration<double>(
//             std::chrono::steady_clock::now() - startTime).count();
 
//         {
//             std::lock_guard<std::mutex> lock(mutex);
//             currentFile.clear();
//         }
 
//         cancelled    = cancelFlag.load();
//         elapsedFinal = elapsed;
//         finished     = true;
//         running      = false;
//     }
// };
 
// // ------------------------------------------------------------
// // Public API
// // ------------------------------------------------------------
 
// FileScanner::FileScanner() : impl(new Impl()) {}
 
// FileScanner::~FileScanner() { shutdown(); }
 
// bool FileScanner::start(ScanType type, const std::string& customPath)
// {
//     if (impl->running)
//         return false;
 
//     if (impl->worker.joinable())
//         impl->worker.join();
 
//     {
//         std::lock_guard<std::mutex> lock(impl->mutex);
 
//         impl->findings.clear();
//         impl->currentFile.clear();
//         impl->type = type;
 
//         switch (type)
//         {
//         case ScanType::Quick:  impl->target = "Quick scan: user folders, temp, startup"; break;
//         case ScanType::Full:   impl->target = "Full scan: all fixed drives"; break;
//         case ScanType::Custom: impl->target = "Custom scan: " + customPath; break;
//         }
//     }
 
//     impl->filesScanned = 0;
//     impl->filesSkipped = 0;
//     impl->bytesScanned = 0;
//     impl->malicious    = 0;
//     impl->suspicious   = 0;
//     impl->fileCounter  = 0;
//     impl->cancelFlag   = false;
//     impl->cancelled    = false;
//     impl->finished     = false;
//     impl->elapsedFinal = 0.0;
//     impl->startTime    = std::chrono::steady_clock::now();
//     impl->running      = true;
 
//     impl->worker = std::thread(&Impl::run, impl.get(), type, customPath);
//     return true;
// }
 
// void FileScanner::cancel()
// {
//     impl->cancelFlag = true;
// }
 
// void FileScanner::shutdown()
// {
//     impl->cancelFlag = true;
 
//     if (impl->worker.joinable())
//         impl->worker.join();
// }
 
// bool FileScanner::isRunning() const
// {
//     return impl->running;
// }
 
// ScanStatus FileScanner::getStatus() const
// {
//     ScanStatus s;
 
//     s.running       = impl->running;
//     s.finished      = impl->finished;
//     s.cancelled     = impl->cancelled;
//     s.filesScanned  = impl->filesScanned;
//     s.filesSkipped  = impl->filesSkipped;
//     s.bytesScanned  = impl->bytesScanned;
//     s.malicious     = impl->malicious;
//     s.suspicious    = impl->suspicious;
 
//     s.elapsedSeconds = s.running
//         ? std::chrono::duration<double>(
//               std::chrono::steady_clock::now() - impl->startTime).count()
//         : impl->elapsedFinal.load();
 
//     std::lock_guard<std::mutex> lock(impl->mutex);
//     s.type        = impl->type;
//     s.currentFile = impl->currentFile;
//     s.target      = impl->target;
 
//     return s;
// }
 
// std::vector<ScanFinding> FileScanner::getFindings() const
// {
//     std::lock_guard<std::mutex> lock(impl->mutex);
//     return impl->findings;
// }
 
// bool FileScanner::exportReport(std::string& outPath) const
// {
//     const ScanStatus status = getStatus();
//     const std::vector<ScanFinding> list = getFindings();
 
//     SYSTEMTIME st{};
//     GetLocalTime(&st);
 
//     char name[64];
//     std::snprintf(name, sizeof(name),
//         "HIDS_Scan_Report_%04d%02d%02d_%02d%02d%02d.txt",
//         (int)st.wYear, (int)st.wMonth, (int)st.wDay,
//         (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
 
//     std::ofstream out(name);
//     if (!out) return false;
 
//     out << "HIDS FILE SCAN REPORT\n";
//     out << "=====================\n";
//     out << "Date:          " << st.wYear << "-" << st.wMonth << "-" << st.wDay
//         << " " << NowTimeOfDay() << "\n";
//     out << "Scan:          " << status.target << "\n";
//     out << "Status:        " << (status.cancelled ? "Stopped by user" : "Completed") << "\n";
//     out << "Files scanned: " << status.filesScanned << "\n";
//     out << "Files skipped: " << status.filesSkipped << "\n";
//     out << "Data read:     " << (status.bytesScanned / (1024 * 1024)) << " MB\n";
//     out << "Duration:      " << (int)status.elapsedSeconds << " s\n";
//     out << "Malicious:     " << status.malicious << "\n";
//     out << "Suspicious:    " << status.suspicious << "\n\n";
 
//     if (list.empty())
//         out << "No threats or suspicious files were found.\n";
 
//     for (const ScanFinding& f : list)
//     {
//         out << "[" << f.level << "] " << f.name << "\n";
//         out << "  File:   " << f.path << "\n";
//         out << "  Reason: " << f.reason << "\n";
//         out << "  Size:   " << f.size << " bytes\n";
//         if (!f.sha256.empty())
//             out << "  SHA256: " << f.sha256 << "\n";
//         out << "\n";
//     }
 
//     outPath = name;
//     return true;
// }


#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
 
#include "FileScanner.h"
 
#include <windows.h>
 
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>
 
namespace fs = std::filesystem;
 
// ------------------------------------------------------------
// Settings
// ------------------------------------------------------------
 
namespace
{
    const unsigned long long MAX_CONTENT_BYTES = 4ULL * 1024 * 1024;    // bytes inspected per file
    const unsigned long long MAX_HASH_BYTES    = 64ULL * 1024 * 1024;   // files larger than this are not hashed
    const std::size_t        MAX_FINDINGS      = 5000;
 
    // Heuristic score needed before a file is reported as Suspicious.
    // Tune this: lower = more detections, more false positives.
    const int SUSPICIOUS_THRESHOLD = 3;
 
    // --------------------------------------------------------
    // String helpers
    // --------------------------------------------------------
 
    std::string ToUtf8(const std::wstring& w)
    {
        if (w.empty()) return std::string();
 
        const int len = WideCharToMultiByte(
            CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
 
        std::string out(len, '\0');
        WideCharToMultiByte(
            CP_UTF8, 0, w.c_str(), (int)w.size(), &out[0], len, nullptr, nullptr);
 
        return out;
    }
 
    std::wstring FromUtf8(const std::string& s)
    {
        if (s.empty()) return std::wstring();
 
        const int len = MultiByteToWideChar(
            CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
 
        std::wstring out(len, L'\0');
        MultiByteToWideChar(
            CP_UTF8, 0, s.c_str(), (int)s.size(), &out[0], len);
 
        return out;
    }
 
    std::string LowerAscii(std::string s)
    {
        for (char& c : s)
            if (c >= 'A' && c <= 'Z')
                c = static_cast<char>(c + 32);
 
        return s;
    }
 
    std::string NowTimeOfDay()
    {
        SYSTEMTIME st{};
        GetLocalTime(&st);
 
        char text[16];
        std::snprintf(text, sizeof(text), "%02d:%02d:%02d",
                      (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
 
        return text;
    }
 
    std::string EnvVar(const char* name)
    {
        char buffer[MAX_PATH * 2];
        const DWORD n = GetEnvironmentVariableA(name, buffer, (DWORD)sizeof(buffer));
 
        if (n == 0 || n >= sizeof(buffer))
            return std::string();
 
        return std::string(buffer, n);
    }
 
    // --------------------------------------------------------
    // SHA-256 (self-contained, no extra libraries)
    // --------------------------------------------------------
 
    class Sha256
    {
    public:
        Sha256() { reset(); }
 
        void reset()
        {
            static const uint32_t init[8] = {
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
 
            for (int i = 0; i < 8; ++i) state[i] = init[i];
 
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
                update(&zero, 1);
 
            unsigned char lenBytes[8];
            for (int i = 0; i < 8; ++i)
                lenBytes[i] = (unsigned char)(totalBits >> (56 - 8 * i));
 
            update(lenBytes, 8);
 
            static const char* hex = "0123456789abcdef";
            std::string out;
 
            for (int i = 0; i < 8; ++i)
                for (int shift = 28; shift >= 0; shift -= 4)
                    out.push_back(hex[(state[i] >> shift) & 0xF]);
 
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
                w[i] = ((uint32_t)b[i * 4] << 24) | ((uint32_t)b[i * 4 + 1] << 16) |
                       ((uint32_t)b[i * 4 + 2] << 8) | (uint32_t)b[i * 4 + 3];
 
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
 
    // Shannon entropy (0..8). Packed / encrypted data is close to 8.
    double Entropy(const std::vector<unsigned char>& data, std::size_t limit)
    {
        const std::size_t n = std::min(limit, data.size());
        if (n == 0) return 0.0;
 
        unsigned long long counts[256] = {};
 
        for (std::size_t i = 0; i < n; ++i)
            ++counts[data[i]];
 
        double entropy = 0.0;
 
        for (int i = 0; i < 256; ++i)
        {
            if (counts[i] == 0) continue;
 
            const double p = (double)counts[i] / (double)n;
            entropy -= p * std::log2(p);
        }
 
        return entropy;
    }
 
    // --------------------------------------------------------
    // Signatures
    // --------------------------------------------------------
 
    // The EICAR test string is built from two pieces so this source file
    // itself is not flagged by real antivirus products.
    std::string EicarSignature()
    {
        return std::string("X5O!P%@AP[4\\PZX54(P^)7CC)7}$") +
               "EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
    }
 
    const char* const EICAR_SHA256 =
        "275a021bbfb6489e54d471899f7db9d1663fc695ec2fe2a2c4538aabf651fd0f";
 
    // Text/byte patterns, matched case-insensitively against the first
    // 4 MB of a file. Each pattern adds "weight" to the file's score.
    struct Signature
    {
        const char* needle;   // lowercase
        int         weight;
        const char* label;
    };
 
    const Signature SIGNATURES[] = {
        // Credential theft tools
        { "mimikatz",                       5, "Mimikatz credential tool" },
        { "sekurlsa::logonpasswords",       5, "Credential dumping command" },
        { "invoke-mimikatz",                5, "Invoke-Mimikatz script" },
 
        // Ransomware behaviour
        { "vssadmin delete shadows",        5, "Deletes shadow copies (ransomware behaviour)" },
        { "wbadmin delete catalog",         4, "Deletes backup catalog" },
        { "recoveryenabled no",             4, "Disables Windows recovery" },
        { "all your files have been encrypted", 4, "Ransom note text" },
 
        // Download-and-run / obfuscated PowerShell
        { "-encodedcommand",                2, "PowerShell encoded command" },
        { "downloadstring(",                2, "Downloads and runs remote script" },
        { "-windowstyle hidden",            2, "Hidden window execution" },
        { "-executionpolicy bypass",        1, "Bypasses PowerShell policy" },
        { "frombase64string",               1, "Base64 decoding" },
        { "net.webclient",                  1, "Web download object" },
        { "invoke-expression",              1, "Dynamic code execution" },
 
        // Persistence
        { "currentversion\\run",            2, "Writes to Run key (persistence)" },
 
        // Process injection API combination (each worth 1)
        { "createremotethread",             1, "Process injection API" },
        { "writeprocessmemory",             1, "Process injection API" },
        { "virtualallocex",                 1, "Process injection API" },
    };
 
    // --------------------------------------------------------
    // Extension sets
    // --------------------------------------------------------
 
    const std::set<std::string> EXECUTABLE_EXT = {
        ".exe", ".dll", ".scr", ".com", ".bat", ".cmd", ".ps1", ".vbs",
        ".vbe", ".js", ".jse", ".wsf", ".hta", ".jar", ".msi", ".lnk",
        ".pif", ".cpl", ".sys"
    };
 
    // Extensions that run code when double-clicked
    const std::set<std::string> RUNNABLE_EXT = {
        ".exe", ".scr", ".com", ".bat", ".cmd", ".ps1", ".vbs", ".vbe",
        ".js", ".jse", ".wsf", ".hta", ".pif", ".lnk"
    };
 
    // Files that should never contain a program
    const std::set<std::string> DECOY_EXT = {
        ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".pdf", ".txt", ".doc",
        ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".rtf", ".csv", ".mp3",
        ".mp4", ".log"
    };
 
    // Extra types worth reading (macro documents, scripts)
    const std::set<std::string> EXTRA_SCAN_EXT = {
        ".py", ".psm1", ".docm", ".xlsm", ".pptm", ".reg", ".sh", ".vb"
    };
 
    bool In(const std::set<std::string>& s, const std::string& v)
    {
        return s.find(v) != s.end();
    }
 
    // --------------------------------------------------------
    // Evidence collected for one file
    // --------------------------------------------------------
 
    struct Evidence
    {
        int score = 0;
        int topWeight = 0;
        std::string topLabel;
        std::vector<std::string> labels;
 
        bool malicious = false;
        std::string malwareName;
 
        void add(int weight, const std::string& label)
        {
            score += weight;
            labels.push_back(label);
 
            if (weight > topWeight)
            {
                topWeight = weight;
                topLabel = label;
            }
        }
    };
}
 
// ------------------------------------------------------------
// Impl
// ------------------------------------------------------------
 
struct FileScanner::Impl
{
    std::atomic<bool> running{ false };
    std::atomic<bool> cancelFlag{ false };
    std::atomic<bool> finished{ false };
    std::atomic<bool> cancelled{ false };
 
    std::atomic<unsigned long long> filesScanned{ 0 };
    std::atomic<unsigned long long> filesSkipped{ 0 };
    std::atomic<unsigned long long> bytesScanned{ 0 };
    std::atomic<int> malicious{ 0 };
    std::atomic<int> suspicious{ 0 };
    std::atomic<double> elapsedFinal{ 0.0 };
 
    std::chrono::steady_clock::time_point startTime;
 
    mutable std::mutex mutex;                 // guards the members below
    std::vector<ScanFinding> findings;
    std::string currentFile;
    std::string target;
    ScanType type = ScanType::Quick;
 
    std::thread worker;
 
    // Worker-thread only
    std::unordered_map<std::string, std::string> knownHashes;   // sha256 -> name
    std::vector<unsigned char> chunk;
    std::wstring selfPath;
    unsigned long long fileCounter = 0;
 
    Impl() { chunk.resize(1 << 20); }
 
    // ---- Known-bad hashes ----
 
    void loadHashes()
    {
        knownHashes.clear();
        knownHashes[EICAR_SHA256] = "EICAR-Test-File";
 
        // Optional user list: "<sha256> <name>" per line, '#' = comment
        std::ifstream in("hids_hashes.txt");
        std::string line;
 
        while (std::getline(in, line))
        {
            if (line.empty() || line[0] == '#') continue;
 
            const std::size_t space = line.find_first_of(" \t");
            std::string hash = LowerAscii(line.substr(0, space));
            std::string name = "Known-Malware";
 
            if (space != std::string::npos)
            {
                const std::size_t start = line.find_first_not_of(" \t", space);
                if (start != std::string::npos)
                {
                    name = line.substr(start);
                    while (!name.empty() && (name.back() == '\r' || name.back() == ' '))
                        name.pop_back();
                }
            }
 
            if (hash.size() == 64)
                knownHashes[hash] = name;
        }
    }
 
    // ---- Which folders to scan ----
 
    std::vector<fs::path> buildRoots(ScanType t, const std::string& customPath)
    {
        std::vector<fs::path> roots;
        std::set<std::string> seen;
 
        auto addRoot = [&](const fs::path& p)
        {
            std::error_code ec;
            if (p.empty() || !fs::exists(p, ec)) return;
 
            const std::string key = LowerAscii(ToUtf8(p.wstring()));
            if (seen.insert(key).second)
                roots.push_back(p);
        };
 
        if (t == ScanType::Custom)
        {
            addRoot(fs::path(FromUtf8(customPath)));
            return roots;
        }
 
        if (t == ScanType::Full)
        {
            const DWORD mask = GetLogicalDrives();
 
            for (int i = 0; i < 26; ++i)
            {
                if (!(mask & (1u << i))) continue;
 
                char root[4] = { (char)('A' + i), ':', '\\', '\0' };
 
                if (GetDriveTypeA(root) == DRIVE_FIXED)
                    addRoot(fs::path(root));
            }
 
            return roots;
        }
 
        // Quick scan
        const fs::path profile = FromUtf8(EnvVar("USERPROFILE"));
 
        if (!profile.empty())
        {
            addRoot(profile / L"Downloads");
            addRoot(profile / L"Desktop");
            addRoot(profile / L"Documents");
        }
 
        addRoot(FromUtf8(EnvVar("TEMP")));
        addRoot(FromUtf8(EnvVar("APPDATA")));
        addRoot(fs::path(FromUtf8(EnvVar("LOCALAPPDATA"))) / L"Temp");
        addRoot(FromUtf8(EnvVar("PUBLIC")));
        addRoot(fs::path(L"C:\\Windows\\Temp"));
        addRoot(fs::path(L"HIDS_Monitor"));
 
        return roots;
    }
 
    static bool shouldSkipDir(const fs::path& dir)
    {
        const std::string p = LowerAscii(ToUtf8(dir.wstring()));
 
        return p.find("\\windows\\winsxs") != std::string::npos ||
               p.find("system volume information") != std::string::npos;
    }
 
    static bool isReparsePoint(const fs::path& p)
    {
        const DWORD attr = GetFileAttributesW(p.c_str());
        return attr != INVALID_FILE_ATTRIBUTES &&
               (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
    }
 
    // ---- Directory walk (explicit stack, never throws) ----
 
    void scanRoot(const fs::path& root)
    {
        std::error_code ec;
 
        if (fs::is_regular_file(root, ec))
        {
            scanFile(root);
            return;
        }
 
        std::vector<fs::path> stack;
        stack.push_back(root);
 
        while (!stack.empty() && !cancelFlag)
        {
            const fs::path dir = stack.back();
            stack.pop_back();
 
            if (shouldSkipDir(dir))
                continue;
 
            std::error_code dirEc;
            fs::directory_iterator it(
                dir, fs::directory_options::skip_permission_denied, dirEc);
            const fs::directory_iterator end;
 
            if (dirEc)
                continue;
 
            for (; it != end && !cancelFlag; it.increment(dirEc))
            {
                if (dirEc) break;
 
                const fs::directory_entry& entry = *it;
                std::error_code e;
 
                if (entry.is_directory(e))
                {
                    if (!isReparsePoint(entry.path()))
                        stack.push_back(entry.path());
                }
                else if (entry.is_regular_file(e))
                {
                    scanFile(entry.path());
                }
            }
        }
    }
 
    // ---- Checks based on the name / location only ----
 
    void checkName(const fs::path& p, const std::string& ext, Evidence& ev)
    {
        const std::wstring fileName = p.filename().wstring();
 
        // Right-to-left override: "invoice_txt.exe" shown as "invoiceexe.txt"
        if (fileName.find(L'\u202E') != std::wstring::npos)
            ev.add(4, "Right-to-left override character in file name");
 
        // Double extension: invoice.pdf.exe
        if (In(RUNNABLE_EXT, ext))
        {
            const std::wstring stem = p.stem().wstring();
            const std::size_t dot = stem.find_last_of(L'.');
 
            if (dot != std::wstring::npos)
            {
                const std::string inner = LowerAscii(ToUtf8(stem.substr(dot)));
 
                if (In(DECOY_EXT, inner))
                    ev.add(4, "Double extension (" + inner + ext + ")");
            }
        }
 
        // Location checks (only for files that can run code)
        if (In(EXECUTABLE_EXT, ext))
        {
            const std::string path = LowerAscii(ToUtf8(p.wstring()));
 
            if (path.find("\\appdata\\local\\temp\\") != std::string::npos ||
                path.find("\\windows\\temp\\") != std::string::npos)
                ev.add(1, "Executable file in a temp folder");
 
            if (path.find("\\start menu\\programs\\startup\\") != std::string::npos)
                ev.add(2, "Executable in the Startup folder");
 
            if (path.find("\\users\\public\\") != std::string::npos)
                ev.add(1, "Executable in the Public folder");
 
            const DWORD attr = GetFileAttributesW(p.c_str());
 
            if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_HIDDEN))
                ev.add(2, "Hidden executable file");
        }
    }
 
    // ---- Checks based on the file content ----
 
    void checkContent(const std::vector<unsigned char>& buf,
                      const std::string& ext, Evidence& ev)
    {
        const std::string raw(buf.begin(), buf.end());
 
        // 1) EICAR test signature (always Malicious)
        if (raw.find(EicarSignature()) != std::string::npos)
        {
            ev.malicious = true;
            ev.malwareName = "EICAR-Test-File";
            ev.add(10, "EICAR antivirus test signature");
        }
 
        // 2) Is it really a Windows program? ("MZ" header)
        const bool isPE = buf.size() >= 2 && buf[0] == 'M' && buf[1] == 'Z';
 
        if (isPE && In(DECOY_EXT, ext))
            ev.add(4, "Executable disguised as " + ext + " file");
 
        if (isPE && Entropy(buf, 1 << 20) >= 7.4)
            ev.add(2, "High entropy (possibly packed or encrypted)");
 
        // 3) Suspicious strings (case-insensitive)
        std::string lower = raw;
        for (char& c : lower)
            if (c >= 'A' && c <= 'Z')
                c = static_cast<char>(c + 32);
 
        for (const Signature& sig : SIGNATURES)
        {
            if (lower.find(sig.needle) == std::string::npos)
                continue;
 
            ev.add(sig.weight, sig.label);
        }
    }
 
    // ---- One file ----
 
    void setCurrent(const fs::path& p)
    {
        std::lock_guard<std::mutex> lock(mutex);
        currentFile = ToUtf8(p.wstring());
    }
 
    void scanFile(const fs::path& p)
    {
        if (cancelFlag) return;
 
        if ((++fileCounter % 25) == 0)
            setCurrent(p);
 
        // Never scan ourselves (our own signature table would match)
        if (!selfPath.empty() &&
            LowerAscii(ToUtf8(p.wstring())) == LowerAscii(ToUtf8(selfPath)))
            return;
 
        std::error_code ec;
        const unsigned long long size = fs::file_size(p, ec);
 
        if (ec)
        {
            ++filesSkipped;
            return;
        }
 
        const std::string ext = LowerAscii(ToUtf8(p.extension().wstring()));
 
        Evidence ev;
        checkName(p, ext, ev);
 
        std::string sha256;
 
        const bool wantContent =
            size > 0 &&
            (size <= 256 || In(EXECUTABLE_EXT, ext) ||
             In(EXTRA_SCAN_EXT, ext) || In(DECOY_EXT, ext));
 
        if (wantContent)
        {
            FILE* f = _wfopen(p.c_str(), L"rb");
 
            if (f == nullptr)
            {
                ++filesSkipped;
            }
            else
            {
                std::vector<unsigned char> buf;
                buf.reserve((std::size_t)std::min<unsigned long long>(size, MAX_CONTENT_BYTES));
 
                const bool hashing = size <= MAX_HASH_BYTES;
                Sha256 sha;
                unsigned long long total = 0;
 
                while (!cancelFlag)
                {
                    const std::size_t n = std::fread(chunk.data(), 1, chunk.size(), f);
                    if (n == 0) break;
 
                    if (buf.size() < MAX_CONTENT_BYTES)
                    {
                        const std::size_t take = (std::size_t)std::min<unsigned long long>(
                            n, MAX_CONTENT_BYTES - buf.size());
                        buf.insert(buf.end(), chunk.begin(), chunk.begin() + take);
                    }
 
                    if (hashing)
                        sha.update(chunk.data(), n);
 
                    total += n;
 
                    // Big file: we only need the first 4 MB
                    if (!hashing && buf.size() >= MAX_CONTENT_BYTES)
                        break;
                }
 
                std::fclose(f);
                bytesScanned += total;
 
                if (cancelFlag) return;
 
                if (hashing && total == size)
                {
                    sha256 = sha.finalHex();
 
                    const auto known = knownHashes.find(sha256);
                    if (known != knownHashes.end())
                    {
                        ev.malicious = true;
                        ev.malwareName = known->second;
                        ev.add(10, "SHA-256 matches known malware (" + known->second + ")");
                    }
                }
 
                checkContent(buf, ext, ev);
            }
        }
 
        ++filesScanned;
 
        // ---- Decide ----
        if (!ev.malicious && ev.score < SUSPICIOUS_THRESHOLD)
            return;
 
        ScanFinding finding;
        finding.path   = ToUtf8(p.wstring());
        finding.size   = size;
        finding.sha256 = sha256;
        finding.time   = NowTimeOfDay();
        finding.score  = ev.score;
 
        for (std::size_t i = 0; i < ev.labels.size(); ++i)
        {
            if (i > 0) finding.reason += "; ";
            finding.reason += ev.labels[i];
        }
 
        if (ev.malicious)
        {
            finding.level = "Malicious";
            finding.name  = ev.malwareName;
            ++malicious;
        }
        else
        {
            finding.level = "Suspicious";
            finding.name  = ev.topLabel;
            ++suspicious;
        }
 
        std::lock_guard<std::mutex> lock(mutex);
 
        if (findings.size() < MAX_FINDINGS)
            findings.push_back(std::move(finding));
    }
 
    // ---- Worker thread ----
 
    void run(ScanType t, std::string customPath)
    {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
 
        wchar_t exe[MAX_PATH * 2] = {};
        GetModuleFileNameW(nullptr, exe, (DWORD)(sizeof(exe) / sizeof(exe[0])));
        selfPath = exe;
 
        loadHashes();
 
        const std::vector<fs::path> roots = buildRoots(t, customPath);
 
        for (const fs::path& root : roots)
        {
            if (cancelFlag) break;
            scanRoot(root);
        }
 
        const double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - startTime).count();
 
        {
            std::lock_guard<std::mutex> lock(mutex);
            currentFile.clear();
        }
 
        cancelled    = cancelFlag.load();
        elapsedFinal = elapsed;
        finished     = true;
        running      = false;
    }
};
 
// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------
 
FileScanner::FileScanner() : impl(new Impl()) {}
 
FileScanner::~FileScanner() { shutdown(); }
 
bool FileScanner::start(ScanType type, const std::string& customPath)
{
    if (impl->running)
        return false;
 
    if (impl->worker.joinable())
        impl->worker.join();
 
    {
        std::lock_guard<std::mutex> lock(impl->mutex);
 
        impl->findings.clear();
        impl->currentFile.clear();
        impl->type = type;
 
        switch (type)
        {
        case ScanType::Quick:  impl->target = "Quick scan: user folders, temp, startup"; break;
        case ScanType::Full:   impl->target = "Full scan: all fixed drives"; break;
        case ScanType::Custom: impl->target = "Custom scan: " + customPath; break;
        }
    }
 
    impl->filesScanned = 0;
    impl->filesSkipped = 0;
    impl->bytesScanned = 0;
    impl->malicious    = 0;
    impl->suspicious   = 0;
    impl->fileCounter  = 0;
    impl->cancelFlag   = false;
    impl->cancelled    = false;
    impl->finished     = false;
    impl->elapsedFinal = 0.0;
    impl->startTime    = std::chrono::steady_clock::now();
    impl->running      = true;
 
    impl->worker = std::thread(&Impl::run, impl.get(), type, customPath);
    return true;
}
 
void FileScanner::cancel()
{
    impl->cancelFlag = true;
}
 
void FileScanner::shutdown()
{
    impl->cancelFlag = true;
 
    if (impl->worker.joinable())
        impl->worker.join();
}
 
bool FileScanner::isRunning() const
{
    return impl->running;
}
 
ScanStatus FileScanner::getStatus() const
{
    ScanStatus s;
 
    s.running       = impl->running;
    s.finished      = impl->finished;
    s.cancelled     = impl->cancelled;
    s.filesScanned  = impl->filesScanned;
    s.filesSkipped  = impl->filesSkipped;
    s.bytesScanned  = impl->bytesScanned;
    s.malicious     = impl->malicious;
    s.suspicious    = impl->suspicious;
 
    s.elapsedSeconds = s.running
        ? std::chrono::duration<double>(
              std::chrono::steady_clock::now() - impl->startTime).count()
        : impl->elapsedFinal.load();
 
    std::lock_guard<std::mutex> lock(impl->mutex);
    s.type        = impl->type;
    s.currentFile = impl->currentFile;
    s.target      = impl->target;
 
    return s;
}
 
std::vector<ScanFinding> FileScanner::getFindings() const
{
    std::lock_guard<std::mutex> lock(impl->mutex);
    return impl->findings;
}
 
bool FileScanner::exportReport(std::string& outPath) const
{
    const ScanStatus status = getStatus();
    const std::vector<ScanFinding> list = getFindings();
 
    SYSTEMTIME st{};
    GetLocalTime(&st);
 
    char name[64];
    std::snprintf(name, sizeof(name),
        "HIDS_Scan_Report_%04d%02d%02d_%02d%02d%02d.txt",
        (int)st.wYear, (int)st.wMonth, (int)st.wDay,
        (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
 
    std::ofstream out(name);
    if (!out) return false;
 
    out << "HIDS FILE SCAN REPORT\n";
    out << "=====================\n";
    out << "Date:          " << st.wYear << "-" << st.wMonth << "-" << st.wDay
        << " " << NowTimeOfDay() << "\n";
    out << "Scan:          " << status.target << "\n";
    out << "Status:        " << (status.cancelled ? "Stopped by user" : "Completed") << "\n";
    out << "Files scanned: " << status.filesScanned << "\n";
    out << "Files skipped: " << status.filesSkipped << "\n";
    out << "Data read:     " << (status.bytesScanned / (1024 * 1024)) << " MB\n";
    out << "Duration:      " << (int)status.elapsedSeconds << " s\n";
    out << "Malicious:     " << status.malicious << "\n";
    out << "Suspicious:    " << status.suspicious << "\n\n";
 
    if (list.empty())
        out << "No threats or suspicious files were found.\n";
 
    for (const ScanFinding& f : list)
    {
        out << "[" << f.level << "] " << f.name << "\n";
        out << "  File:   " << f.path << "\n";
        out << "  Reason: " << f.reason << "\n";
        out << "  Size:   " << f.size << " bytes\n";
        if (!f.sha256.empty())
            out << "  SHA256: " << f.sha256 << "\n";
        out << "\n";
    }
 
    outPath = name;
    return true;
}