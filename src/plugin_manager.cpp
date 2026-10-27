#include "plugin_manager.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <cmath>

namespace fs = std::filesystem;

PluginManager::~PluginManager() {
    // Освобождаем DLL в обратном порядке загрузки.
    for (auto it = handles.rbegin(); it != handles.rend(); ++it) {
        FreeLibrary(*it);
    }
}

void PluginManager::LoadPlugins(const std::filesystem::path& directory) {
    // Если папки plugins нет - предупреждаем (но не падаем, просто без доп. функций).
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Plugins directory not found: " << directory.string() << "\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory)) { // Нерекуррентная итерация по верхнему уровню папки.
        // Нас интересуют только .dll
        if (entry.path().extension() != ".dll") {
            continue;
        }

        // LoadLibraryW принимает широкую строку (используем wstring() для корректной работы с Unicode-путями).
        HMODULE handle = LoadLibraryW(entry.path().wstring().c_str());
        if (!handle) {
            std::cerr << "Failed to load " << entry.path() << "\n";
            continue;
        }

        // Ищем обязательные экспорты. GetProcAddress возвращает FARPROC - приводим к нужному типу.
        auto getName = reinterpret_cast<const char*(*)()>(GetProcAddress(handle, "GetFunctionName"));
        auto compute = reinterpret_cast<ComputeFunc>(GetProcAddress(handle, "Compute"));

        // Если хотя бы одного экспорта нет - DLL не соответствует условиям.
        if (!getName || !compute) {
            std::cerr << "Warning: " << entry.path() << " does not export required functions\n";
            FreeLibrary(handle);
            continue;
        }

        const char* name = getName();
        // ПРоверкаа getname на валидность
        if (!name || name[0] == '\0') {
            std::cerr << "Warning: " << entry.path() << " returned empty name\n";
            FreeLibrary(handle);
            continue;
        }

        // Защита от дубликатов: две DLL не могут регистрировать одно и то же имя.
        if (functions.find(name) != functions.end()) {
            std::cerr << "Warning: Duplicate function '" << name << "' will be ignored\n";
            FreeLibrary(handle);
            continue;
        }

        // Всё хорошо - запоминаем по имени функции указатель на неё и дескриптор.
        functions[name] = compute;
        handles.push_back(handle);

        std::cout << "Plugin: Loaded function '" << name << "' from " << entry.path().filename() << "\n";
    }

    if (functions.empty()) {
        std::cerr << "Warning: No valid plugins loaded\n";
    }
}

bool PluginManager::HasFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

double PluginManager::CallFunction(const std::string& name, double arg) const {
    auto it = functions.find(name);
    if (it == functions.end()) {
        throw std::runtime_error("Unknown function: " + name);
    }
    double result = it->second(arg);

    // Проверяем на NaN: так плагин сообщает о недопустимом аргументе.
    if (std::isnan(result)) {
        throw std::domain_error("Function '" + name + "' returned NaN for argument " + std::to_string(arg));
    }
    return result;
}