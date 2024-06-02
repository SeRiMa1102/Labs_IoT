#include <QCoreApplication>
#include <iostream>
#include <string>
#include <vector>
#include "Classes.h"



std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

int main()
{
    while(true){
        std::string input;
        std::cout << "Insert of print exit: ";
        std::cin >> input;
        if (input == "exit"){
            break;
        }
        char delimiter = ';';
        std::vector<std::string> expressions = splitString(input, delimiter);

        for (const auto& item : expressions){

            if (item.empty()){
                continue;
            }
            std::string res = item;
            res += ')';
            res.insert(0, "(");
            Parser parser(res);
            std::unique_ptr<Node> root = parser.parse();
            std::cout << "Result: " << root->evaluate() << std::endl;
        }
    }
    return 0;
}


//y = sin(M)*M*(M+(M-M))/M
// expression -> term + term
// term -> factor * factor
// factor -> number \ expression

