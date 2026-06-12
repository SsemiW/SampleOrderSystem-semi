#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <string>
#include <string_view>

namespace Console {

enum class Color : WORD {
    Black       = 0,
    DarkBlue    = FOREGROUND_BLUE,
    DarkGreen   = FOREGROUND_GREEN,
    DarkCyan    = FOREGROUND_GREEN  | FOREGROUND_BLUE,
    DarkRed     = FOREGROUND_RED,
    DarkMagenta = FOREGROUND_RED    | FOREGROUND_BLUE,
    DarkYellow  = FOREGROUND_RED    | FOREGROUND_GREEN,
    Gray        = FOREGROUND_RED    | FOREGROUND_GREEN | FOREGROUND_BLUE,
    DarkGray    = FOREGROUND_INTENSITY,
    Blue        = FOREGROUND_BLUE   | FOREGROUND_INTENSITY,
    Green       = FOREGROUND_GREEN  | FOREGROUND_INTENSITY,
    Cyan        = FOREGROUND_GREEN  | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    Red         = FOREGROUND_RED    | FOREGROUND_INTENSITY,
    Magenta     = FOREGROUND_RED    | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    Yellow      = FOREGROUND_RED    | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    White       = FOREGROUND_RED    | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};

inline HANDLE stdOut() noexcept { return GetStdHandle(STD_OUTPUT_HANDLE); }

inline void setColor(Color fg, Color bg = Color::Black) {
    SetConsoleTextAttribute(stdOut(),
        static_cast<WORD>(fg) | (static_cast<WORD>(bg) << 4));
}

inline void resetColor() {
    SetConsoleTextAttribute(stdOut(), static_cast<WORD>(Color::Gray));
}

inline void moveCursor(SHORT x, SHORT y) {
    SetConsoleCursorPosition(stdOut(), { x, y });
}

inline void hideCursor() {
    CONSOLE_CURSOR_INFO ci{ 1, FALSE };
    SetConsoleCursorInfo(stdOut(), &ci);
}

inline void showCursor() {
    CONSOLE_CURSOR_INFO ci{ 10, TRUE };
    SetConsoleCursorInfo(stdOut(), &ci);
}

inline void clearScreen() {
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    GetConsoleScreenBufferInfo(stdOut(), &csbi);
    DWORD sz = static_cast<DWORD>(csbi.dwSize.X) * csbi.dwSize.Y;
    DWORD written = 0;
    COORD home{ 0, 0 };
    FillConsoleOutputCharacterA(stdOut(), ' ', sz, home, &written);
    FillConsoleOutputAttribute(stdOut(), csbi.wAttributes, sz, home, &written);
    moveCursor(0, 0);
}

inline void setTitle(std::string_view title) {
    SetConsoleTitleA(title.data());
}

inline void setWindowSize(SHORT cols, SHORT rows) {
    HANDLE h = stdOut();
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    GetConsoleScreenBufferInfo(h, &csbi);

    SMALL_RECT smallWin{ 0, 0, 1, 1 };
    SetConsoleWindowInfo(h, TRUE, &smallWin);

    COORD bufSize{ cols, static_cast<SHORT>(rows + 200) };
    SetConsoleScreenBufferSize(h, bufSize);

    SMALL_RECT winRect{ 0, 0, static_cast<SHORT>(cols - 1), static_cast<SHORT>(rows - 1) };
    SetConsoleWindowInfo(h, TRUE, &winRect);
}

} // namespace Console
