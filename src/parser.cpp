#include "parser.h"
#include <cctype>
#include <cmath>
#include <stdexcept>

Parser::Parser(const std::string& input, const PluginManager& pluginManager): input(input), pluginManager(pluginManager) {}

double Parser::Parse() {
    double value = ParseExpression();
    SkipWhitespace();
    if (pos != input.size()) {
        throw std::runtime_error("Unexpected characters at position " + std::to_string(pos));
    }
    return value;
}

// expression = term {(+|-) term}
double Parser::ParseExpression() {
    double left = ParseTerm();
    while (true) {
        SkipWhitespace();
        if (Match('+')) {
            left += ParseTerm();
        } else if (Match('-')) {
            left -= ParseTerm();
        } else {
            break;
        }
    }
    return left;
}

// term = power {(*|/) power}
double Parser::ParseTerm() {
    double left = ParsePower();
    while (true) {
        SkipWhitespace();
        if (Match('*')) {
            left *= ParsePower();
        } else if (Match('/')) {
            double right = ParsePower();
            if (right == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            left /= right;
        } else {
            break;
        }
    }
    return left;
}

// power = unary {^ power}
// ^ - право-ассоциативный: 2^3^2 = 2^(3^2)
double Parser::ParsePower() {
    double base = ParseUnary();
    SkipWhitespace();
    if (Match('^')) {
        double exponent = ParsePower(); // рекурсивный вызов для право-ассоциативности
        return std::pow(base, exponent);
    }
    return base;
}

// unary = (+|-) unary | primary
double Parser::ParseUnary() {
    SkipWhitespace();
    if (Match('+')) {
        return ParseUnary();
    }
    if (Match('-')) {
        return -ParseUnary();
    }
    return ParsePrimary();
}

// primary = '('expression')' | number | identificator'('expression')' 
double Parser::ParsePrimary() {
    SkipWhitespace();
    // 1) Скобки
    if (Match('(')) {
        double val = ParseExpression();
        Expect(')');
        return val;
    }
    // 2) Число (целое или дробное)
    if (std::isdigit(Peek()) || Peek() == '.') {
        size_t start = pos;
        while (pos < input.size() && (std::isdigit(input[pos]) || input[pos] == '.')) {
            ++pos;
        }
        return std::stod(input.substr(start, pos - start));
    }

    // 3) Функция из плагина: имя'('выражение')'
    if (std::isalpha(Peek())) {
        size_t start = pos;
        while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_')) {
            ++pos;
        }
        std::string name = input.substr(start, pos - start);
        SkipWhitespace();
        Expect('(');
        double arg = ParseExpression();
        Expect(')');

        if (!pluginManager.HasFunction(name)) {
            throw std::runtime_error("Unknown function: " + name);
        }
        return pluginManager.CallFunction(name, arg);
    }

    throw std::runtime_error("Unexpected character '" + std::string(1, Peek()) + "' at position " + std::to_string(pos));
}



void Parser::SkipWhitespace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        ++pos;
    }
}

char Parser::Peek() const {
    return (pos < input.size()) ? input[pos] : '\0';
}

bool Parser::Match(char c) {
    SkipWhitespace();
    if (Peek() == c) {
        ++pos;
        return true;
    }
    return false;
}

void Parser::Expect(char c) {
    SkipWhitespace();
    if (Peek() != c) {
        throw std::runtime_error(std::string("Expected '") + c + "' at position " + std::to_string(pos));
    }
    ++pos;
}