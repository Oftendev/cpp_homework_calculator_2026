#include <iostream>
#include <string>
#include <windows.h>
#include <filesystem>
#include <iterator>
#include "plugin_manager.h"
#include "parser.h"

namespace fs = std::filesystem;

// Возвращает папку, в которой лежит текущий .exe (Мне нужно искать плагины рядом с исполняемым файлом,иа не относительно текущей рабочей директории.
static fs::path GetExecutableDirectory() {
    // Буфер под путь.
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buffer, static_cast<DWORD>(std::size(buffer))); // nullptr - текущий исполняемый файл
    if (len == 0 || len == std::size(buffer)) {
        // Если функция не смогла записать путь - вернём CWD
        return fs::current_path();
    }
    // parent_path() отбрасывает имя файла и оставляет путь к папке.
    return fs::path(buffer).parent_path();
}

int main() {
    // Ищем плагины рядом с .exe, а не относительно CWD.
    fs::path pluginDir = GetExecutableDirectory() / "plugins";

    PluginManager pm;
    pm.LoadPlugins(pluginDir);
    std::cout << "Calculator with plugins. Enter expression (empty line to exit):\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break; // ДЛя выхода.
        }

        try {
            Parser parser(line, pm);
            double result = parser.Parse();
            std::cout << "Result: " << result << "\n";
        } catch (const std::exception& e) {
            // Ловим все ошибки.
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}