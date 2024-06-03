#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <map>
enum class OPERANDS{
    plus,
    minus,
    mul,
    del,
    o_bra,
    c_bra,
    pow,
    log,
    sin,
    sqrt,
    abs,
    number,
    param,
    assign,
    comma,
    end
};

struct Token{
    OPERANDS op =OPERANDS::plus;
    double value = 0;
    std::string param_name = "";
};

class Tokenizer{
public:
    Tokenizer(const std::string& str);
    Token getToken(size_t i) const;
    size_t  sizeTokens() const;
private:
    std::vector<Token> tokens;
};




class Node {
public:
    virtual ~Node() {}
    virtual double evaluate(const std::map<std::string, double>& variables) const = 0;
};

class NumberNode : public Node {
public:
    NumberNode(double val);
    virtual double evaluate(const std::map<std::string, double>&) const override;

private:
    double value;
};

class ParamNode : public Node {
public:
    ParamNode(const std::string& name);

    virtual double evaluate(const std::map<std::string, double>&) const override;

private:
    std::string varName;
};

class AssignNode : public Node {
public:
    AssignNode(const std::string& name, std::unique_ptr<Node> expr, std::map<std::string, double>& vars);

    virtual double evaluate(const std::map<std::string, double>&) const override;

private:
    std::string varName;
    std::unique_ptr<Node> expression;
    std::map<std::string, double>& variables;
};

class BinaryOperationNode : public Node {
public:
    BinaryOperationNode(OPERANDS op, std::unique_ptr<Node> l, std::unique_ptr<Node> r);

    virtual double evaluate(const std::map<std::string, double>&) const override;

private:
    OPERANDS operation;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

class FunctionNode : public Node {
public:
    FunctionNode(OPERANDS func, std::unique_ptr<Node> arg);

    virtual double evaluate(const std::map<std::string, double>&) const override;

private:
    OPERANDS function;
    std::unique_ptr<Node> argument;
};

class PowFunctionNode : public Node {
public:
    PowFunctionNode(std::unique_ptr<Node> base, std::unique_ptr<Node> exponent);

    virtual double evaluate(const std::map<std::string, double>&) const override;

private:
    std::unique_ptr<Node> base;
    std::unique_ptr<Node> exponent;
};

class Parser {
public:
    Parser(const std::string& str);

    void addNewExpr(const std::string& str);

    std::unique_ptr<Node> parse();

    const std::map<std::string, double>& getVariables() const;

private:
    Token getNextToken();
    std::unique_ptr<Node> assignment();
    std::unique_ptr<Node> expression();
    std::unique_ptr<Node> term();
    std::unique_ptr<Node> factor();
    void eat(OPERANDS type);

    std::map<std::string, double> variables;

    Tokenizer tokenizer;
    Token currentToken;
    size_t current_index;


};

