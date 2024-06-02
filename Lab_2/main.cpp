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

        for (const auto& item : expressions){

            if (item.empty()){
                continue;
            }
            std::string res = item;
            Parser parser(res, params);
            std::unique_ptr<Node> root = parser.parse();
            std::cout << "Result: " << root->evaluate(parser.getVariables()) << std::endl;
        }
    }
    return 0;
}

//param[0] (!=!)[1] (***********);
// if [0] == param;
// if [1] == "="'
//     eval
