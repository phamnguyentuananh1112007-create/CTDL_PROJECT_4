#include "Application.h"
#include "Console.h"

#include <filesystem>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Application
{
    bool prepareWorkingDirectory()
    {
#ifdef _WIN32
        char path[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);
        if (length == 0 || length == MAX_PATH)
        {
            return false;
        }

        const std::filesystem::path executable(path);
        const std::filesystem::path project = executable.parent_path().parent_path();
        return SetCurrentDirectoryA(project.string().c_str()) != 0;
#else
        return std::filesystem::current_path().has_root_path();
#endif
    }

    void startScreen(const std::string& title)
    {
        Console::clear();
        Console::title(title);
    }

    void runLoop(const std::string& title,
                 const std::function<bool(int&)>& readChoice,
                 const std::function<void(int)>& handleChoice,
                 int exitChoice)
    {
        int choice = -1;
        while (true)
        {
            startScreen(title);
            if (!readChoice(choice))
            {
                continue;
            }
            if (choice == exitChoice)
            {
                return;
            }
            handleChoice(choice);
        }
    }
}
