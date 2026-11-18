#pragma once

#include <functional>
#include <string>

namespace Application
{
    bool prepareWorkingDirectory();
    void startScreen(const std::string& title);
    void runLoop(const std::string& title,
                 const std::function<bool(int&)>& readChoice,
                 const std::function<void(int)>& handleChoice,
                 int exitChoice = 0);
}
