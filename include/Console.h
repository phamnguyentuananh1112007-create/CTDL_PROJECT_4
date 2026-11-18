#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace Console
{
    enum class Color
    {
        Default = 7,
        Green = 10,
        Cyan = 11,
        Red = 12,
        Magenta = 13,
        Yellow = 14,
        White = 15
    };

    void clear();
    void pause();
    void setColor(Color color);

    void title(const std::string& text,
               Color color = Color::Default,
               std::ostream& output = std::cout);
    void section(const std::string& text, std::ostream& output = std::cout);
    void menu(const std::vector<std::string>& items,
              const std::string& backLabel = "Quay lai",
              std::ostream& output = std::cout);

    void success(const std::string& message, std::ostream& output = std::cout);
    void error(const std::string& message, std::ostream& output = std::cout);
    void warning(const std::string& message, std::ostream& output = std::cout);
    void info(const std::string& message, std::ostream& output = std::cout);

    // true = yes; false = no; canceled = ESC
    bool yesNo(const std::string& question, bool& canceled,
               std::istream& input = std::cin,
               std::ostream& output = std::cout);

    std::string fit(const std::string& text, std::size_t width);
}
