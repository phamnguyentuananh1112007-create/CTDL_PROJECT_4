#include "Console.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <conio.h>
#else
#include <cstdlib>
#endif

#include <algorithm>
#include <cctype>

namespace Console
{
    void clear()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void pause()
    {
        std::cout << "\nNhan phim bat ky de tiep tuc...";
        std::cout.flush();
#ifdef _WIN32
        _getch();
#else
        std::cin.get();
#endif
        std::cout << '\n';
    }

    void setColor(Color color)
    {
#ifdef _WIN32
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE),
            static_cast<WORD>(color));
#endif
    }

    void title(const std::string& text, Color color, std::ostream& output)
    {
        const std::size_t width = 56;
        const std::string line(width, '=');
        setColor(color);
        output << line << '\n';

        const std::string shown = fit(text, width);
        const std::size_t left = (width > shown.size()) ? (width - shown.size()) / 2 : 0;
        output << std::string(left, ' ') << shown << '\n';
        output << line << '\n';
        setColor(Color::Default);
    }

    void section(const std::string& text, std::ostream& output)
    {
        output << "\n--- " << fit(text, 48) << " ---\n";
    }

    void menu(const std::vector<std::string>& items,
              const std::string& backLabel,
              std::ostream& output)
    {
        for (std::size_t i = 0; i < items.size(); ++i)
        {
            output << (i + 1) << ". " << items[i] << '\n';
        }
        output << "0. " << backLabel << '\n';
        output << "----------------------------------------\n";
    }

    void success(const std::string& message, std::ostream& output)
    {
        setColor(Color::Green);
        output << "[+] " << message << '\n';
        setColor(Color::Default);
    }

    void error(const std::string& message, std::ostream& output)
    {
        setColor(Color::Red);
        output << "[-] " << message << '\n';
        setColor(Color::Default);
    }

    void warning(const std::string& message, std::ostream& output)
    {
        setColor(Color::Yellow);
        output << "[!] " << message << '\n';
        setColor(Color::Default);
    }

    void info(const std::string& message, std::ostream& output)
    {
        setColor(Color::Cyan);
        output << "[i] " << message << '\n';
        setColor(Color::Default);
    }

    bool yesNo(const std::string& question, bool& canceled,
               std::istream& input, std::ostream& output)
    {
        (void)input;
        canceled = false;

        while (true)
        {
            output << question << " (yes/no, ESC huy): ";
            std::string answer;

#ifdef _WIN32
            // Read a text answer while allowing ESC to cancel.
            const char first = static_cast<char>(_getch());
            if (static_cast<unsigned char>(first) == 27)
            {
                canceled = true;
                output << '\n';
                return false;
            }
            output << first;
            answer.push_back(first);

            char ch = 0;
            while ((ch = static_cast<char>(_getch())) != '\r')
            {
                if (static_cast<unsigned char>(ch) == 27)
                {
                    canceled = true;
                    output << '\n';
                    return false;
                }
                if (ch == '\b')
                {
                    if (!answer.empty())
                    {
                        answer.pop_back();
                        output << "\b \b";
                    }
                }
                else
                {
                    answer.push_back(ch);
                    output << ch;
                }
            }
            output << '\n';
#else
            if (!std::getline(input, answer))
            {
                canceled = true;
                return false;
            }
#endif

            std::string normalized;
            normalized.reserve(answer.size());
            for (unsigned char ch : answer)
            {
                normalized.push_back(static_cast<char>(std::tolower(ch)));
            }

            while (!normalized.empty() &&
                   (normalized.back() == ' ' || normalized.back() == '\t'))
            {
                normalized.pop_back();
            }

            if (normalized == "yes" || normalized == "y")
            {
                return true;
            }
            if (normalized == "no" || normalized == "n")
            {
                return false;
            }

            warning("Lua chon khong hop le. Hay nhap yes hoac no.");
        }
    }

    std::string fit(const std::string& text, std::size_t width)
    {
        if (text.size() <= width)
        {
            return text;
        }
        if (width <= 3)
        {
            return text.substr(0, width);
        }
        return text.substr(0, width - 3) + "...";
    }
}
