

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
//         const fs::path localAppData = FromUtf8(EnvVar("LOCALAPPDATA"));

//         if (!localAppData.empty())
//             addRoot(localAppData / L"Temp");
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

// // ------------------------------------------------------------
// // Signature list (for the Logs & Reports page)
// // ------------------------------------------------------------

// int FileScanner::suspiciousThreshold()
// {
//     return SUSPICIOUS_THRESHOLD;
// }

// std::vector<SignatureInfo> FileScanner::getSignatures()
// {
//     std::vector<SignatureInfo> list;

//     auto add = [&list](const char* category, const std::string& name,
//                        const std::string& pattern, int weight, bool instant)
//     {
//         SignatureInfo s;
//         s.category         = category;
//         s.name             = name;
//         s.pattern          = pattern;
//         s.weight           = weight;
//         s.instantMalicious = instant;
//         list.push_back(s);
//     };

//     // ---- Known-bad hashes ----
//     add("Known hash", "EICAR-Test-File", EICAR_SHA256, 10, true);

//     {
//         // Optional user list "hids_hashes.txt": "<sha256> <name>" per line
//         int custom = 0;
//         std::ifstream in("hids_hashes.txt");
//         std::string line;

//         while (std::getline(in, line))
//         {
//             const std::size_t first = line.find_first_not_of(" \t\r");

//             if (first == std::string::npos || line[first] == '#')
//                 continue;

//             if (line.size() - first >= 64)
//                 ++custom;
//         }

//         add("Known hash", "Custom hash list",
//             "hids_hashes.txt (" + std::to_string(custom) + " hash(es) loaded)",
//             10, true);
//     }

//     // ---- Content signatures ----
//     add("File content", "EICAR antivirus test signature",
//         "Standard EICAR test string", 10, true);

//     for (const Signature& sig : SIGNATURES)
//         add("File content", sig.label, sig.needle, sig.weight, false);

//     // ---- Content heuristics ----
//     add("File content", "Executable disguised as document/image",
//         "Windows program (MZ header) with a .jpg/.pdf/.txt/.docx... extension", 4, false);
//     add("File content", "High entropy (possibly packed or encrypted)",
//         "Windows program with entropy >= 7.4", 2, false);

//     // ---- File name / location heuristics ----
//     add("File name / location", "Right-to-left override character in file name",
//         "Unicode U+202E in the file name", 4, false);
//     add("File name / location", "Double extension",
//         "Runnable file named like invoice.pdf.exe", 4, false);
//     add("File name / location", "Executable in a temp folder",
//         "...\\AppData\\Local\\Temp\\ or ...\\Windows\\Temp\\", 1, false);
//     add("File name / location", "Executable in the Startup folder",
//         "...\\Start Menu\\Programs\\Startup\\", 2, false);
//     add("File name / location", "Executable in the Public folder",
//         "...\\Users\\Public\\", 1, false);
//     add("File name / location", "Hidden executable file",
//         "Executable with the Hidden attribute", 2, false);

//     return list;
// }





#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "FileScanner.h"

#include <windows.h>
#include <wincrypt.h>
#include <softpub.h>
#include <wintrust.h>
#include <mscat.h>

#ifdef _MSC_VER
#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "crypt32.lib")
#endif
// MinGW users: link with -lwintrust -lcrypt32

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

    // A validly signed file (Authenticode / Windows catalog) is NOT reported
    // unless its score reaches this value or it is a known-bad hash.
    const int TRUSTED_SIGNED_MAX_SCORE = 8;

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

    // Case-sensitive byte search inside the first `limit` bytes.
    bool ContainsBytes(const std::vector<unsigned char>& data, std::size_t limit, const char* needle)
    {
        const std::size_t n = std::min(limit, data.size());
        const std::size_t m = std::strlen(needle);
        if (m == 0 || n < m) return false;

        const unsigned char* first = data.data();
        const unsigned char* last  = first + n;
        const unsigned char* pat   = reinterpret_cast<const unsigned char*>(needle);

        return std::search(first, last, pat, pat + m) != last;
    }

    // --------------------------------------------------------
    // Authenticode / catalog signature check
    // --------------------------------------------------------

    // Returns true when the file carries a valid embedded signature OR is
    // covered by a valid Windows catalog (this is how System32 files are signed).
    // No network access is used (revocation checks are off, cache-only).
    bool IsTrustedSigned(const fs::path& p)
    {
        const wchar_t* file = p.c_str();
        GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
        HWND noWindow = static_cast<HWND>(INVALID_HANDLE_VALUE);

        // 1) Embedded signature
        {
            WINTRUST_FILE_INFO fi{};
            fi.cbStruct       = sizeof(fi);
            fi.pcwszFilePath  = file;

            WINTRUST_DATA wd{};
            wd.cbStruct            = sizeof(wd);
            wd.dwUIChoice          = WTD_UI_NONE;
            wd.fdwRevocationChecks = WTD_REVOKE_NONE;
            wd.dwUnionChoice       = WTD_CHOICE_FILE;
            wd.pFile               = &fi;
            wd.dwStateAction       = WTD_STATEACTION_VERIFY;
            wd.dwProvFlags         = WTD_CACHE_ONLY_URL_RETRIEVAL | WTD_REVOCATION_CHECK_NONE;

            const LONG r = WinVerifyTrust(noWindow, &action, &wd);

            wd.dwStateAction = WTD_STATEACTION_CLOSE;
            WinVerifyTrust(noWindow, &action, &wd);

            if (r == ERROR_SUCCESS)
                return true;
        }

        // 2) Catalog signature
        HANDLE h = CreateFileW(file, GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (h == INVALID_HANDLE_VALUE)
            return false;

        bool trusted = false;
        HCATADMIN admin = nullptr;

        if (CryptCATAdminAcquireContext(&admin, nullptr, 0))
        {
            DWORD hashLen = 0;
            CryptCATAdminCalcHashFromFileHandle(h, &hashLen, nullptr, 0);

            if (hashLen > 0)
            {
                std::vector<BYTE> hash(hashLen);

                if (CryptCATAdminCalcHashFromFileHandle(h, &hashLen, hash.data(), 0))
                {
                    HCATINFO cat = CryptCATAdminEnumCatalogFromHash(
                        admin, hash.data(), hashLen, 0, nullptr);

                    if (cat)
                    {
                        CATALOG_INFO ci{};
                        ci.cbStruct = sizeof(ci);

                        if (CryptCATCatalogInfoFromContext(cat, &ci, 0))
                        {
                            std::wstring tag;
                            tag.reserve(hashLen * 2);
                            static const wchar_t* hx = L"0123456789ABCDEF";

                            for (BYTE b : hash)
                            {
                                tag.push_back(hx[b >> 4]);
                                tag.push_back(hx[b & 0x0F]);
                            }

                            WINTRUST_CATALOG_INFO wci{};
                            wci.cbStruct               = sizeof(wci);
                            wci.pcwszCatalogFilePath   = ci.wszCatalogFile;
                            wci.pcwszMemberTag         = tag.c_str();
                            wci.pcwszMemberFilePath    = file;
                            wci.hMemberFile            = h;
                            wci.pbCalculatedFileHash   = hash.data();
                            wci.cbCalculatedFileHash   = hashLen;
                            wci.hCatAdmin              = admin;

                            WINTRUST_DATA cd{};
                            cd.cbStruct            = sizeof(cd);
                            cd.dwUIChoice          = WTD_UI_NONE;
                            cd.fdwRevocationChecks = WTD_REVOKE_NONE;
                            cd.dwUnionChoice       = WTD_CHOICE_CATALOG;
                            cd.pCatalog            = &wci;
                            cd.dwStateAction       = WTD_STATEACTION_VERIFY;
                            cd.dwProvFlags         = WTD_CACHE_ONLY_URL_RETRIEVAL | WTD_REVOCATION_CHECK_NONE;

                            const LONG r = WinVerifyTrust(noWindow, &action, &cd);

                            cd.dwStateAction = WTD_STATEACTION_CLOSE;
                            WinVerifyTrust(noWindow, &action, &cd);

                            trusted = (r == ERROR_SUCCESS);
                        }

                        CryptCATAdminReleaseCatalogContext(admin, cat, 0);
                    }
                }
            }

            CryptCATAdminReleaseContext(admin, 0);
        }

        CloseHandle(h);
        return trusted;
    }

    // True when the file has the "downloaded from the internet" stream (ZoneId=3).
    bool HasInternetMark(const fs::path& p)
    {
        const std::wstring ads = p.wstring() + L":Zone.Identifier";
        FILE* f = _wfopen(ads.c_str(), L"rb");

        if (f == nullptr)
            return false;

        char buf[256] = {};
        const std::size_t n = std::fread(buf, 1, sizeof(buf) - 1, f);
        std::fclose(f);

        return std::string(buf, n).find("ZoneId=3") != std::string::npos;
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

    // Text/byte patterns, matched against the first 4 MB of a file AFTER it has
    // been normalised: lowercased, runs of whitespace collapsed to one space,
    // cmd carets (^) removed, UTF-16LE scripts converted to plain text.
    // So every needle must be lowercase with single spaces.
    //
    // "group": when several rules describe the same behaviour, give them the
    // same group name. Only the highest weight of each group is counted, so
    // one command can no longer be scored twice.
    //
    // Rules in group "note" (ransom-note text) are also applied to text/document
    // files; every other rule only runs on programs and scripts.
    struct Signature
    {
        const char* needle;   // lowercase
        int         weight;
        const char* label;
        const char* group = nullptr;
    };

    const Signature SIGNATURES[] = {

        // ---- Credential theft (mimikatz family) ----
        { "mimikatz",                       5, "Mimikatz credential tool",                         "mimikatz" },
        { "invoke-mimikatz",                5, "Invoke-Mimikatz script",                           "mimikatz" },
        { "sekurlsa::logonpasswords",       5, "Credential dumping command",                       "mimikatz" },
        { " sekurlsa",                      4, "Credential-tool module indicator",                 "mimikatz" },
        { "sekurlsa::pth",                  5, "Pass-the-hash command indicator",                  "mimikatz" },
        { "lsadump::sam",                   5, "SAM credential-dumping command",                   "mimikatz" },
        { "privilege::debug",               3, "Requests debug privilege in credential tooling",   "mimikatz" },
        { "token::elevate",                 4, "Token elevation command indicator",                "mimikatz" },
        { "meterpreter",                    4, "Meterpreter payload indicator" },

        // ---- Credential theft (LSASS / registry hives) ----
        { "procdump -ma lsass",             5, "Dumps LSASS process memory",                       "lsass" },
        { "procdump.exe -ma lsass",         5, "Dumps LSASS process memory",                       "lsass" },
        { "comsvcs.dll, minidump",          5, "Uses MiniDump through comsvcs",                    "lsass" },
        { "comsvcs.dll,minidump",           5, "Uses MiniDump through comsvcs",                    "lsass" },
        { "comsvcs.dll,#24",                5, "Uses MiniDump (ordinal) through comsvcs",          "lsass" },
        { "rundll32.exe comsvcs.dll",       3, "Rundll32 invoking comsvcs",                        "lsass" },
        { "reg save hklm\\sam",             4, "Exports the SAM registry hive",                    "hivedump" },
        { "reg save hklm\\security",        4, "Exports the SECURITY registry hive",               "hivedump" },
        { "sam.save",                       2, "Possible copied SAM database" },
        { "system.save",                    2, "Possible copied SYSTEM registry hive" },
        { "security.save",                  2, "Possible copied SECURITY registry hive" },
        { "ntds.dit",                       2, "Active Directory database reference" },

        // ---- Ransomware: shadow copies / backups / recovery ----
        { "vssadmin delete shadows",        5, "Deletes shadow copies (ransomware behaviour)",     "shadow" },
        { "vssadmin.exe delete shadows",    5, "Deletes shadow copies (ransomware behaviour)",     "shadow" },
        { "wmic shadowcopy delete",         5, "Deletes volume shadow copies",                     "shadow" },
        { "delete shadows /all /quiet",     5, "Deletes all shadow copies",                        "shadow" },
        { "vssadmin resize shadowstorage",  2, "Modifies shadow-copy storage",                     "shadow" },

        { "wbadmin delete catalog",         4, "Deletes backup catalog",                           "backup" },
        { "wbadmin.exe delete catalog",     4, "Deletes backup catalog",                           "backup" },
        { "wbadmin delete systemstatebackup", 5, "Deletes system-state backups",                   "backup" },
        { "wbadmin.exe delete systemstatebackup", 5, "Deletes system-state backups",               "backup" },
        { "wbadmin delete backup",          4, "Deletes Windows backup",                           "backup" },
        { "wbadmin.exe delete backup",      4, "Deletes Windows backup",                           "backup" },

        { "recoveryenabled no",             4, "Disables Windows recovery",                        "recovery" },
        { "bcdedit /set {default} recoveryenabled no", 5, "Disables Windows recovery configuration", "recovery" },
        { "bcdedit.exe /set {default} recoveryenabled no", 5, "Disables Windows recovery configuration", "recovery" },

        { "bcdedit /set {default} bootstatuspolicy ignoreallfailures", 4, "Changes boot recovery failure policy", "bootpolicy" },
        { "bcdedit.exe /set {default} bootstatuspolicy ignoreallfailures", 4, "Changes boot recovery failure policy", "bootpolicy" },

        { "cipher /w:",                     2, "Secure-delete free space command indicator" },

        // Ransom-note text (also checked in .txt / document files)
        { "all your files have been encrypted", 4, "Ransom note text",                             "note" },
        { "your files have been encrypted", 4, "Ransom note text",                                 "note" },
        { "your documents have been encrypted", 4, "Possible ransomware note text",                "note" },

        // ---- PowerShell / script execution ----
        { "-encodedcommand",                2, "PowerShell encoded command",                       "psenc" },
        { "powershell -enc ",               2, "Encoded PowerShell invocation",                    "psenc" },
        { "downloadstring(",                2, "Downloads and runs remote script" },
        { "-windowstyle hidden",            2, "Hidden window execution",                          "hiddenps" },
        { " -nop -w hidden",                2, "PowerShell no-profile hidden execution arguments", "hiddenps" },
        { " -noprofile -windowstyle hidden", 2, "Hidden PowerShell execution arguments",           "hiddenps" },
        { "-executionpolicy bypass",        1, "Bypasses PowerShell policy" },
        { "frombase64string",               1, "Base64 decoding" },
        { "net.webclient",                  1, "Web download object" },
        { "invoke-expression",              1, "Dynamic code execution" },
        { "invoke-obfuscation",             4, "PowerShell obfuscation framework indicator" },
        { "iex (new-object net.webclient)", 4, "PowerShell download-and-execute pattern" },
        { "start-bitstransfer",             2, "PowerShell BITS transfer command" },
        { "certutil -urlcache -split -f",   3, "Certutil remote file retrieval pattern" },
        { "bitsadmin /transfer",            2, "BITS command-line transfer" },
        { "mshta http",                     4, "MSHTA launching remote content" },
        { "regsvr32 /s /n /u /i:",          4, "Regsvr32 scriptlet execution pattern" },
        { "rundll32 javascript:",           5, "Rundll32 JavaScript execution pattern" },
        { "forfiles /p",                    1, "Forfiles command; context-dependent indicator" },
        { "wscript.shell",                  1, "Windows Script Host shell automation",             "wscript" },
        { "createobject(\"wscript.shell\")", 1, "Script creates Windows shell object",             "wscript" },
        { "shell.application",              1, "Shell automation object reference" },
        { "curl -o ",                       1, "Command-line download indicator; context-dependent" },

        // ---- Defence evasion / log tampering ----
        { "set-mppreference -disablerealtimemonitoring", 5, "Attempts to disable Defender monitoring" },
        { "set-mppreference -disableintrusionpreventionsystem", 4, "Changes Defender intrusion-prevention setting" },
        { "add-mppreference -exclusionpath", 3, "Adds a Defender exclusion path" },
        { "add-mppreference -exclusionprocess", 3, "Adds a Defender process exclusion" },
        { "net stop windefend",             4, "Attempts to stop Microsoft Defender service",      "defstop" },
        { "sc stop windefend",              4, "Attempts to stop Microsoft Defender service",      "defstop" },
        { "taskkill /f /im sense.exe",      4, "Attempts to terminate endpoint security process" },
        { "wevtutil cl security",           5, "Clears the Security event log",                    "wevtsec" },
        { "wevtutil.exe cl security",       5, "Clears the Security event log",                    "wevtsec" },
        { "wevtutil cl system",             4, "Clears the System event log",                      "wevtsys" },
        { "wevtutil.exe cl system",         4, "Clears the System event log",                      "wevtsys" },
        { "wevtutil sl security /e:false",  5, "Disables Security event logging" },
        { "auditpol /clear",                4, "Clears audit policy configuration" },

        // ---- Persistence ----
        { "currentversion\\run",            2, "Writes to Run key (persistence)",                  "runkey" },
        { "reg add hkcu\\software\\microsoft\\windows\\currentversion\\run", 2, "Adds a current-user Run key", "runkey" },
        { "reg add hklm\\software\\microsoft\\windows\\currentversion\\run", 2, "Adds a machine Run key",      "runkey" },
        { "\\software\\microsoft\\windows\\currentversion\\runonce", 2, "References a RunOnce persistence key", "runkey" },
        { "\\system\\currentcontrolset\\services\\", 1, "References Windows service configuration" },
        { "schtasks.exe /create",           2, "Creates a scheduled task" },
        { "sc.exe create",                  2, "Creates a Windows service" },
        { "new-service -name",              2, "PowerShell service creation" },
        { "wmic /namespace:\\\\root\\subscription", 4, "WMI event-subscription namespace access",  "wmisub" },
        { "commandlineeventconsumer",       4, "WMI command-line event consumer",                  "wmisub" },
        { "activescripteventconsumer",      4, "WMI script event consumer",                        "wmisub" },

        // ---- Privilege escalation / UAC bypass ----
        { "fodhelper.exe",                  2, "Potential UAC bypass technique indicator" },
        { "computerdefaults.exe",           2, "Potential UAC bypass technique indicator" },
        { "eventvwr.exe",                   2, "Potential UAC bypass technique indicator" },

        // ---- Network / remote execution ----
        { "psexec \\\\",                    2, "PsExec remote execution command pattern" },
        { "paexec",                         2, "PAExec remote execution tool indicator" },
        { "winrs -r:",                      2, "Windows remote shell invocation" },
        { "enter-pssession",                2, "PowerShell remote session command" },
        { "invoke-command -computername",   2, "PowerShell remote command invocation" },
        { "netsh advfirewall firewall add rule", 2, "Adds a Windows Firewall rule" },
        { "portproxy add v4tov4",           2, "Configures Windows port forwarding" },

        // ---- Process injection API combination (each worth 1) ----
        { "createremotethread",             1, "Process injection API" },
        { "writeprocessmemory",             1, "Process injection API" },
        { "virtualallocex",                 1, "Process injection API" },
    };

    // --------------------------------------------------------
    // Extension sets
    // --------------------------------------------------------

    const std::set<std::string> EXECUTABLE_EXT = {
        ".exe", ".dll", ".scr", ".com", ".bat", ".cmd", ".ps1", ".vbs",
        ".vbe", ".js", ".jse", ".wsf", ".wsh", ".hta", ".jar", ".msi", ".lnk",
        ".pif", ".cpl", ".sys", ".chm", ".xll", ".appx", ".msix"
    };

    // Extensions that run code when double-clicked
    const std::set<std::string> RUNNABLE_EXT = {
        ".exe", ".scr", ".com", ".bat", ".cmd", ".ps1", ".vbs", ".vbe",
        ".js", ".jse", ".wsf", ".wsh", ".hta", ".pif", ".lnk"
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

    // Types that can carry an Authenticode / catalog signature
    const std::set<std::string> SIGNABLE_EXT = {
        ".exe", ".dll", ".sys", ".scr", ".cpl", ".ocx", ".msi", ".ps1", ".psm1"
    };

    // Extensions used by common ransomware families
    const char* const RANSOM_EXT[] = {
        ".locked", ".encrypted", ".crypt", ".wncry", ".lockbit", ".ryk", ".locky"
    };

    // File names typically used for ransom notes (lowercase fragments)
    const char* const RANSOM_NOTE_NAMES[] = {
        "readme_to_decrypt", "how_to_decrypt", "how_to_recover",
        "restore_files", "decrypt_instructions", "recover_your_files"
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
    std::unordered_set<std::string> allowHashes;                // trusted sha256
    std::vector<std::string> allowPrefixes;                     // trusted path prefixes (lowercase)
    std::vector<unsigned char> chunk;
    std::wstring selfPath;
    std::string selfSha256;
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

    // ---- Trusted files (allowlist) ----
    //
    // Optional "hids_allowlist.txt", one entry per line, '#' = comment:
    //   - a 64-character SHA-256  -> that exact file is never reported
    //   - anything else           -> treated as a path prefix, e.g.
    //                                C:\Program Files\MyCompany\
    // Known-bad hashes always win over the allowlist.

    void loadAllowlist()
    {
        allowHashes.clear();
        allowPrefixes.clear();

        std::ifstream in("hids_allowlist.txt");
        std::string line;

        while (std::getline(in, line))
        {
            const std::size_t a = line.find_first_not_of(" \t\r\n");
            if (a == std::string::npos || line[a] == '#') continue;

            const std::size_t b = line.find_last_not_of(" \t\r\n");
            const std::string item = LowerAscii(line.substr(a, b - a + 1));

            const bool isHash = item.size() == 64 &&
                item.find_first_not_of("0123456789abcdef") == std::string::npos;

            if (isHash)
                allowHashes.insert(item);
            else
                allowPrefixes.push_back(item);
        }
    }

    bool isAllowedPath(const std::string& lowerPath) const
    {
        for (const std::string& prefix : allowPrefixes)
            if (lowerPath.compare(0, prefix.size(), prefix) == 0)
                return true;

        return false;
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
        addRoot(FromUtf8(EnvVar("APPDATA")));   // includes the per-user Startup folder

        const fs::path localAppData = FromUtf8(EnvVar("LOCALAPPDATA"));
        if (!localAppData.empty())
            addRoot(localAppData / L"Temp");

        // All-users Startup folder
        const fs::path programData = FromUtf8(EnvVar("PROGRAMDATA"));
        if (!programData.empty())
            addRoot(programData / L"Microsoft" / L"Windows" / L"Start Menu" /
                    L"Programs" / L"Startup");

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

        // Ransomware-style extension and ransom-note file name
        for (const char* r : RANSOM_EXT)
        {
            if (ext == r)
            {
                ev.add(2, "Ransomware-style file extension (" + ext + ")");
                break;
            }
        }

        const std::string lowerName = LowerAscii(ToUtf8(fileName));

        for (const char* n : RANSOM_NOTE_NAMES)
        {
            if (lowerName.find(n) != std::string::npos)
            {
                ev.add(3, "Ransom-note style file name");
                break;
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

        // Mark of the Web (only for files that run when double-clicked)
        if (In(RUNNABLE_EXT, ext) && HasInternetMark(p))
            ev.add(1, "Downloaded from the internet (Mark of the Web)");
    }

    // ---- Checks based on the file content ----

    void checkContent(const std::vector<unsigned char>& buf,
                      const std::string& ext, Evidence& ev)
    {
        std::string text(buf.begin(), buf.end());

        // 1) EICAR test signature (case-sensitive, so before lowercasing; always Malicious)
        if (text.find(EicarSignature()) != std::string::npos)
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

        // Common packer section names live in the PE header
        if (isPE &&
            (ContainsBytes(buf, 4096, "UPX0") || ContainsBytes(buf, 4096, "UPX1") ||
             ContainsBytes(buf, 4096, ".vmp0") || ContainsBytes(buf, 4096, ".themida")))
            ev.add(2, "Packer section name (UPX / VMProtect / Themida)");

        // 3) UTF-16LE text (PowerShell 5.1 Out-File default): drop the zero bytes
        if (buf.size() >= 2 && buf[0] == 0xFF && buf[1] == 0xFE)
        {
            std::string narrow;
            narrow.reserve(buf.size() / 2);

            for (std::size_t i = 2; i < buf.size(); i += 2)
                narrow.push_back((char)buf[i]);

            text.swap(narrow);
        }

        // 4) Normalise: lowercase, collapse whitespace, drop cmd carets
        std::string norm;
        norm.reserve(text.size());
        bool lastSpace = false;

        for (char c : text)
        {
            if (c == '^') continue;

            if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                if (!lastSpace) norm.push_back(' ');
                lastSpace = true;
                continue;
            }

            lastSpace = false;
            norm.push_back((c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c);
        }

        // 5) Signatures. Programs/scripts get every rule; other files
        //    (text, documents...) only get the ransom-note rules.
        const bool codeLike = isPE || In(EXECUTABLE_EXT, ext) || In(EXTRA_SCAN_EXT, ext);

        std::unordered_map<std::string, const Signature*> best;   // best hit per group

        for (const Signature& sig : SIGNATURES)
        {
            const bool noteRule = sig.group && std::strcmp(sig.group, "note") == 0;

            if (!codeLike && !noteRule)
                continue;

            const auto hit = std::search(
                norm.begin(), norm.end(),
                std::boyer_moore_horspool_searcher(
                    sig.needle, sig.needle + std::strlen(sig.needle)));

            if (hit == norm.end())
                continue;

            if (sig.group)
            {
                const Signature*& slot = best[sig.group];

                if (slot == nullptr || sig.weight > slot->weight)
                    slot = &sig;
            }
            else
            {
                ev.add(sig.weight, sig.label);
            }
        }

        for (const auto& kv : best)
            ev.add(kv.second->weight, kv.second->label);
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

        const std::string lowerPath = LowerAscii(ToUtf8(p.wstring()));

        // Never scan ourselves (our own signature table would match)
        if (!selfPath.empty() && lowerPath == LowerAscii(ToUtf8(selfPath)))
            return;

        // User-trusted folders
        if (isAllowedPath(lowerPath))
        {
            ++filesScanned;
            return;
        }

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

                bool knownBad = false;

                if (hashing && total == size)
                {
                    sha256 = sha.finalHex();

                    const auto known = knownHashes.find(sha256);
                    if (known != knownHashes.end())
                    {
                        knownBad = true;
                        ev.malicious = true;
                        ev.malwareName = known->second;
                        ev.add(10, "SHA-256 matches known malware (" + known->second + ")");
                    }

                    if (!knownBad)
                    {
                        // A copy of this program itself, or a file the user trusts
                        if ((!selfSha256.empty() && sha256 == selfSha256) ||
                            allowHashes.count(sha256) != 0)
                        {
                            ++filesScanned;
                            return;
                        }
                    }
                }

                checkContent(buf, ext, ev);
            }
        }

        ++filesScanned;

        // ---- Decide ----
        if (!ev.malicious && ev.score < SUSPICIOUS_THRESHOLD)
            return;

        // Validly signed programs are trusted unless the evidence is very strong
        if (!ev.malicious &&
            ev.score < TRUSTED_SIGNED_MAX_SCORE &&
            In(SIGNABLE_EXT, ext) &&
            IsTrustedSigned(p))
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

        // Hash of our own program, so copies of it are not reported either
        selfSha256.clear();

        if (!selfPath.empty())
        {
            FILE* f = _wfopen(selfPath.c_str(), L"rb");

            if (f != nullptr)
            {
                Sha256 s;
                std::size_t n = 0;

                while ((n = std::fread(chunk.data(), 1, chunk.size(), f)) > 0)
                    s.update(chunk.data(), n);

                std::fclose(f);
                selfSha256 = s.finalHex();
            }
        }

        loadHashes();
        loadAllowlist();

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

// ------------------------------------------------------------
// Signature list (for the Logs & Reports page)
// ------------------------------------------------------------

int FileScanner::suspiciousThreshold()
{
    return SUSPICIOUS_THRESHOLD;
}

std::vector<SignatureInfo> FileScanner::getSignatures()
{
    std::vector<SignatureInfo> list;

    auto add = [&list](const char* category, const std::string& name,
                       const std::string& pattern, int weight, bool instant)
    {
        SignatureInfo s;
        s.category         = category;
        s.name             = name;
        s.pattern          = pattern;
        s.weight           = weight;
        s.instantMalicious = instant;
        list.push_back(s);
    };

    // ---- Known-bad hashes ----
    add("Known hash", "EICAR-Test-File", EICAR_SHA256, 10, true);

    {
        // Optional user list "hids_hashes.txt": "<sha256> <name>" per line
        int custom = 0;
        std::ifstream in("hids_hashes.txt");
        std::string line;

        while (std::getline(in, line))
        {
            const std::size_t first = line.find_first_not_of(" \t\r");

            if (first == std::string::npos || line[first] == '#')
                continue;

            if (line.size() - first >= 64)
                ++custom;
        }

        add("Known hash", "Custom hash list",
            "hids_hashes.txt (" + std::to_string(custom) + " hash(es) loaded)",
            10, true);
    }

    // ---- Content signatures ----
    add("File content", "EICAR antivirus test signature",
        "Standard EICAR test string", 10, true);

    for (const Signature& sig : SIGNATURES)
        add("File content", sig.label, sig.needle, sig.weight, false);

    // ---- Content heuristics ----
    add("File content", "Executable disguised as document/image",
        "Windows program (MZ header) with a .jpg/.pdf/.txt/.docx... extension", 4, false);
    add("File content", "High entropy (possibly packed or encrypted)",
        "Windows program with entropy >= 7.4", 2, false);
    add("File content", "Packer section name (UPX / VMProtect / Themida)",
        "UPX0, UPX1, .vmp0 or .themida in the PE header", 2, false);

    // ---- File name / location heuristics ----
    add("File name / location", "Right-to-left override character in file name",
        "Unicode U+202E in the file name", 4, false);
    add("File name / location", "Double extension",
        "Runnable file named like invoice.pdf.exe", 4, false);
    add("File name / location", "Ransomware-style file extension",
        ".locked / .encrypted / .crypt / .wncry / .lockbit / .ryk / .locky", 2, false);
    add("File name / location", "Ransom-note style file name",
        "readme_to_decrypt, how_to_decrypt, how_to_recover, restore_files...", 3, false);
    add("File name / location", "Executable in a temp folder",
        "...\\AppData\\Local\\Temp\\ or ...\\Windows\\Temp\\", 1, false);
    add("File name / location", "Executable in the Startup folder",
        "...\\Start Menu\\Programs\\Startup\\", 2, false);
    add("File name / location", "Executable in the Public folder",
        "...\\Users\\Public\\", 1, false);
    add("File name / location", "Hidden executable file",
        "Executable with the Hidden attribute", 2, false);
    add("File name / location", "Downloaded from the internet (Mark of the Web)",
        "Runnable file with Zone.Identifier ZoneId=3", 1, false);

    return list;
}