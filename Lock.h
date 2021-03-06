#ifndef   LOCK_H
#define   LOCK_H

#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#include <Windows.h>
#include <Psapi.h>
#include <tchar.h>
#else
#include <unistd.h>
#endif

std::string lockFilePath(const char* currentFileName)
{
    static std::string file = std::filesystem::current_path().string() + "/" + currentFileName + ".lock";
    return file;
}

int processId()
{
#ifdef _MSC_VER
    return GetCurrentProcessId();
#else
    return ::getpid();
#endif
}

std::vector<std::string> processList()
{
#ifdef _MSC_VER
    DWORD aProcesses[1024], cbNeeded;

    // Returns zero on failure but we ignore because we're brave enough
    EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);

    DWORD cProcesses = cbNeeded / sizeof(DWORD);

    std::vector<std::string> result;
    for (DWORD i = 0; i < cProcesses; i++) {
        result.push_back(std::to_string(aProcesses[i]));
    }
    return result;
#else
    std::vector<std::string> processes;
    for (const auto& p : std::filesystem::directory_iterator("/proc")) {
        processes.push_back(p.path().filename());
    }
    return processes;
#endif
}

// Writes current app PID to lock file
void lockProcess(const char* currentFileName)
{
    std::fstream lockFile(lockFilePath(currentFileName), std::ios::out | std::ios::app);
    lockFile << processId() << std::endl;
    lockFile.close();
}

// Removes current PID from lock file
void unlockProcess(const char* currentFileName)
{
    std::fstream lockFile;
    lockFile.open(lockFilePath(currentFileName), std::ios::in);

    std::vector<std::string> ids;
    std::string id;
    while (std::getline(lockFile, id)) {
        if (std::stoi(id) != processId()) {
            ids.push_back(id);
        }
    }
    lockFile.close();
    lockFile.open(lockFilePath(currentFileName), std::ios::out | std::ios::trunc);
    for (const auto& id : ids) {
        lockFile << id << std::endl;
    }
    lockFile.close();
}

// Returns wether there is another instance of the app with different PID running
bool isOnlyInstance(const char* currentFileName)
{
    std::fstream lockFile(lockFilePath(currentFileName), std::ios::in);
    std::vector<std::string> ids;
    std::string id;
    while (std::getline(lockFile, id)) {
        ids.push_back(id);
    }
    lockFile.close();

    auto procs = processList();
    for (auto id : ids) {
        if (std::find(procs.cbegin(), procs.cend(), id) != procs.cend()) {
            return false;
        }
    }
    return true;
}

// Removes PIDs from lock file that are not running anymore, this mainly cleanups after
// crashes since unlockProcess would not be called
void cleanLockFile(const char* currentFileName)
{
    std::fstream lockFile;
    lockFile.open(lockFilePath(currentFileName), std::ios::in);
    std::vector<std::string> ids;
    std::string id;
    while (std::getline(lockFile, id)) {
        ids.push_back(id);
    }
    lockFile.close();

    lockFile.open(lockFilePath(currentFileName), std::ios::out | std::ios::trunc);
    auto procs = processList();
    for (auto id : ids) {
        if (std::find(procs.cbegin(), procs.cend(), id) != procs.cend()) {
            lockFile << id << std::endl;
        }
    }
    lockFile.close();
}

#endif