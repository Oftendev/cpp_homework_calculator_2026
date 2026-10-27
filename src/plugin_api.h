#pragma once

// Макрос для экспорта/импорта функций из DLL.
// При сборке плагина определяется PLUGIN_EXPORTS, поэтому функции экспортируются(добавляются в таблицу эскпорта dll файла), используя атрибут __declspec(dllexport).

#ifdef _WIN32
    #ifdef PLUGIN_EXPORTS
        #define PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGIN_API __declspec(dllimport) // Вне плагина макрос раскрывается в dllimport, но они не вызываюстся напрямую - мы получаем их адреса через GetProcAddress, поэтому dllimport здесь не применяется. (хост не линкуется с dll напрямую, а грузит через LoadLibraryW)
    #endif
#else
    #define PLUGIN_API
#endif

// Каждый плагин ОБЯЗАН экспортировать ровно эти две функции с C-именованием (во избежание mangling).
extern "C" {
    // Возвращает имя функции, которое будет использоваться в выражениях.
    PLUGIN_API const char* GetFunctionName();

    // Вычисляет значение для одного аргумента. Если аргумент недопустим, функция возвращает NaN.
    PLUGIN_API double Compute(double arg);
}