#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <windows.h>
#include <filesystem>

class PluginManager {
public:
    // Тип указателя на compute функцию плагина.
    using ComputeFunc = double(*)(double);

    PluginManager() = default;

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    // При разрушении освобождаем все загруженные DLL.
    ~PluginManager();

    // Сканируем directory, загружаем все dll и регистрируем функции. Если папка пуста или содержит битые DLL - выводим предупреждения, но не падаем.
    void LoadPlugins(const std::filesystem::path& directory);

    // Проверяем, зарегистрирована ли функция с таким именем.
    bool HasFunction(const std::string& name) const;

    // Вызываем функцию по имени. Бросаем исключение, если функции нет или она вернула NaN.
    double CallFunction(const std::string& name, double arg) const;

private:
    std::vector<HMODULE> handles; // дескрипторы загруженных DLL
    std::unordered_map<std::string, ComputeFunc> functions; // имя -> указатель на Compute
};