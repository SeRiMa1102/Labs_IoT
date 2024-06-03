#include <QCoreApplication>
#include <iostream>
#include <string>
#include <vector>
#include "Classes.h"



std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> for_tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        for_tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    for_tokens.push_back(str.substr(start));
    return for_tokens;
}

int main()
{
    std::map<std::string, double> params;
    while(true){
        std::string input;
        std::cout << "Insert or print \"exit\": ";
        std::getline(std::cin, input); // Считывание строки до символа новой строки
        if (input == "exit"){
            break;
        }
        char delimiter = ';';
        std::vector<std::string> expressions = splitString(input, delimiter);
        //std::cout << expressions.size() << std::endl;

        Parser parser("");

        for (size_t i = 0; i < expressions.size(); i++){
            const auto& item = expressions[i];
            if (item.empty()){
                continue;
            }
            parser.addNewExpr(item);
            std::unique_ptr<Node> root = parser.parse();
            std::cout << "Result: " << root->evaluate(parser.getVariables()) << std::endl;

            if (i == expressions.size() - 2){
                // Печать значений всех переменных
                const std::map<std::string, double>& variables = parser.getVariables();
                for (const auto& var : variables) {
                    std::cout << var.first << " = " << var.second << std::endl;
                }
            }

        }

    }
    return 0;
}

//param[0] (!=!)[1] (***********);
// if [0] == param;
// if [1] == "="'
//     eval
