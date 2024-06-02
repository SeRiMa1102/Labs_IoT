#include <QCoreApplication>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <algorithm> // Для std::remove_if
#include <cctype>    // Для std::isspace
#include <regex>     // Для std::regex_replace

double arithmeticOp(double left, const std::string& op, double right) {
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) throw std::invalid_argument("Division by zero");
        return left / right;
    }
    throw std::invalid_argument("Invalid arithmetic operator");
}

bool comparisonOp(double left, const std::string& op, double right) {
    if (op == "<") return left < right;
    if (op == ">") return left > right;
    if (op == "<=") return left <= right;
    if (op == ">=") return left >= right;
    if (op == "==") return left == right;
    if (op == "!=") return left != right;
    throw std::invalid_argument("Invalid comparison operator");
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    while (true) {
        std::string input;
        std::cout << "Enter expression or 'exit': ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        // Удаление пробелов
        input.erase(std::remove_if(input.begin(), input.end(), [](unsigned char c) { return std::isspace(c); }), input.end());

        // Вставка пробелов для удобного считывания из потока
        input = std::regex_replace(input, std::regex(R"(([\+\-\*/<>]=?|==|!=))"), " $1 ");

        std::istringstream stream(input);
        double num_left, num_right;
        std::string op;
        stream >> num_left >> op >> num_right;

        try {
            if (stream.fail() || !stream.eof()) {
                throw std::invalid_argument("Invalid input format");
            }

            if (op == "+" || op == "-" || op == "*" || op == "/") {
                double result = arithmeticOp(num_left, op, num_right);
                std::cout << "Result: " << result << std::endl;
            } else if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
                bool result = comparisonOp(num_left, op, num_right);
                std::cout << "Result: " << std::boolalpha << result << std::endl;
            } else {
                throw std::invalid_argument("Unknown operator");
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
