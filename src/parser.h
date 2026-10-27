#pragma once
#include <string>
#include "plugin_manager.h"

class Parser {
public:
    // input - строка выражения, pluginManager для вызова плагинных функций.
    Parser(const std::string& input, const PluginManager& pluginManager);

    // Разбирает и вычисляет выражение
    double Parse();

private:
    // Методы рекурсивного спуска (грамматика ниже).
    double ParseExpression(); // expression = term {(+|-) term }
    double ParseTerm(); // term = power {(*|/) power }
    double ParsePower(); // power = unary[^ power]
    double ParseUnary(); // unary = (+|-) unary | primary
    double ParsePrimary(); // primary = '('expression')' | number | identifier'('expression')'

    // Вспомогательные методы для работы с позицией в строке.
    void SkipWhitespace(); // Сдвигает pos вперёд, пока не встретит непробельный символ.
    char Peek() const; // Возвращает текущий символ, не сдвигая pos.
    bool Match(char c); // Если символ совпадает - true и сдвиг pos, иначе false и не двигай.
    void Expect(char c); // Если символ совпадает - true и сдвиг pos, иначе - исключение.

    std::string input; // входная строка
    size_t pos = 0; // текущая позиция
    const PluginManager& pluginManager; // ссылка на менеджер плагинов
};