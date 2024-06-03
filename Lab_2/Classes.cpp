#include "Classes.h"


Tokenizer::Tokenizer(const std::string& str) {
    size_t i = 0;
    while (i < str.size()) {
        Token tok;
        if (std::isspace(str[i])){
            i++;
            continue;
        } else if (std::isdigit(str[i])) {
            std::string numStr;
            while (i < str.size() && (std::isdigit(str[i]) || str[i] == '.')) {
                numStr += str[i];
                i++;
            }
            tok.op = OPERANDS::number;
            tok.value = std::stod(numStr);
        } else if (std::isalpha(str[i])) {
            std::string word;
            while (i < str.size() && std::isalpha(str[i])) {
                word += str[i];
                i++;
            }
            if (word == "pow") {
                tok.op = OPERANDS::pow;
            } else if (word == "log") {
                tok.op = OPERANDS::log;
            } else if (word == "sin") {
                tok.op = OPERANDS::sin;
            } else if (word == "sqrt") {
                tok.op = OPERANDS::sqrt;
            } else if (word == "abs") {
                tok.op = OPERANDS::abs;
            } else {
                tok.op = OPERANDS::param;
                tok.param_name = word;
            }
        } else {
            switch (str[i]) {
            case '+':
                tok.op = OPERANDS::plus;
                break;
            case '-':
                tok.op = OPERANDS::minus;
                break;
            case '*':
                tok.op = OPERANDS::mul;
                break;
            case '/':
                tok.op = OPERANDS::del;
                break;
            case '(':
                tok.op = OPERANDS::o_bra;
                break;
            case ')':
                tok.op = OPERANDS::c_bra;
                break;
            case ',':
                tok.op = OPERANDS::comma;
                break;
            case '=':
                tok.op = OPERANDS::assign;
                break;
            default:
                break;
            }
            i++;
        }
        tokens.push_back(tok);
    }
}

Token Tokenizer::getToken(size_t i) const {
    return tokens[i];
}

size_t  Tokenizer::sizeTokens() const {
    return tokens.size();
}

NumberNode::NumberNode(double val) : value(val) {}
double NumberNode::evaluate(const std::map<std::string, double>&) const{ return value; }


ParamNode::ParamNode(const std::string& name):varName(name) {}

double ParamNode::evaluate(const std::map<std::string, double>& variables) const {
    auto it = variables.find(varName);
    if (it != variables.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Variable not found: " + varName);
    }
}

AssignNode::AssignNode(const std::string& name, std::unique_ptr<Node> expr, std::map<std::string, double>& vars)
    : varName(name), expression(std::move(expr)), variables(vars) {}

double AssignNode::evaluate(const std::map<std::string, double>&) const {
    double value = expression->evaluate(variables);
    variables[varName] = value;
    return value;
}



BinaryOperationNode::BinaryOperationNode(OPERANDS op, std::unique_ptr<Node> l, std::unique_ptr<Node> r)
        : operation(op), left(std::move(l)), right(std::move(r)) {}

double BinaryOperationNode::evaluate(const std::map<std::string, double>& variables) const {
    switch (operation) {
    case OPERANDS::plus:
        return left->evaluate(variables) + right->evaluate(variables);
    case OPERANDS::minus:
        return left->evaluate(variables) - right->evaluate(variables);
    case OPERANDS::mul:
        return left->evaluate(variables) * right->evaluate(variables);
    case OPERANDS::del:
        return left->evaluate(variables) / right->evaluate(variables);
    default:
        throw std::runtime_error("Invalid operator in expression tree.");
    }
}


FunctionNode::FunctionNode(OPERANDS func, std::unique_ptr<Node> arg)
    : function(func), argument(std::move(arg)) {}

double FunctionNode::evaluate(const std::map<std::string, double>& variables) const {
    switch (function) {
    case OPERANDS::log:
        return std::log(argument->evaluate(variables));
    case OPERANDS::sin:
        return std::sin(argument->evaluate(variables));
    case OPERANDS::sqrt:
        return std::sqrt(argument->evaluate(variables));
    case OPERANDS::abs:
        return std::abs(argument->evaluate(variables));
    default:
        throw std::runtime_error("Invalid function in expression tree.");
    }
}


PowFunctionNode::PowFunctionNode(std::unique_ptr<Node> base, std::unique_ptr<Node> exponent)
    : base(std::move(base)), exponent(std::move(exponent)) {}

double PowFunctionNode::evaluate(const std::map<std::string, double>& variables) const{
    return std::pow(base->evaluate(variables), exponent->evaluate(variables));
}




Parser::Parser(const std::string& str) : tokenizer(Tokenizer(str)), current_index(0) {
    currentToken = getNextToken();
}

void Parser::addNewExpr(const std::string& str){
    tokenizer = Tokenizer(str);
    current_index = 0;
    currentToken = getNextToken();
}

std::unique_ptr<Node> Parser::parse() {
    return assignment();
}

const std::map<std::string, double>& Parser::getVariables() const {
    return variables;
}

Token Parser::getNextToken(){
    if (current_index>=tokenizer.sizeTokens()){
        return {OPERANDS::end, 0, ""};
    }
    return tokenizer.getToken(current_index++);
}

std::unique_ptr<Node> Parser::assignment() {
    if (currentToken.op == OPERANDS::param) {
        if (tokenizer.getToken(current_index).op != OPERANDS::assign){
            return expression();
        }

        std::string varName = currentToken.param_name;
        eat(OPERANDS::param);
        if (currentToken.op == OPERANDS::assign) {
            eat(OPERANDS::assign);
            return std::make_unique<AssignNode>(varName, expression(), variables);
        } else {
            return std::make_unique<ParamNode>(varName);
        }
    }
    return expression();
}

std::unique_ptr<Node> Parser::expression() {
    auto result = term();

    while (currentToken.op == OPERANDS::plus || currentToken.op == OPERANDS::minus) {
        Token token = currentToken;
        if (token.op == OPERANDS::plus || token.op == OPERANDS::minus) {
            eat(token.op);
            result = std::make_unique<BinaryOperationNode>(token.op == OPERANDS::plus ? OPERANDS::plus : OPERANDS::minus, std::move(result), term());
        }
    }

    return result;
}

std::unique_ptr<Node> Parser::term() {
    auto result = factor();

    while (currentToken.op == OPERANDS::mul || currentToken.op == OPERANDS::del) {
        Token token = currentToken;
        if (token.op == OPERANDS::mul || token.op == OPERANDS::del) {
            eat(token.op);
            result = std::make_unique<BinaryOperationNode>(token.op == OPERANDS::mul ? OPERANDS::mul : OPERANDS::del, std::move(result), factor());
        }
    }

    return result;
}

std::unique_ptr<Node> Parser::factor() {
    Token token = currentToken;
    if (token.op == OPERANDS::number) {
        eat(OPERANDS::number);
        return std::make_unique<NumberNode>(token.value);
    } else if (token.op == OPERANDS::o_bra) {
        eat(OPERANDS::o_bra);
        auto result = expression();
        eat(OPERANDS::c_bra);
        return result;



    } else if (token.op == OPERANDS::log || token.op == OPERANDS::sin || token.op == OPERANDS::abs || token.op == OPERANDS::sqrt) {
        eat(token.op);
        eat(OPERANDS::o_bra);
        auto arg = expression();
        eat(OPERANDS::c_bra);
        return std::make_unique<FunctionNode>(token.op, std::move(arg));
    } else if (token.op == OPERANDS::pow) {
        eat(token.op);
        eat(OPERANDS::o_bra);
        auto base = expression();
        eat(OPERANDS::comma);
        auto exponent = expression();
        eat(OPERANDS::c_bra);
        return std::make_unique<PowFunctionNode>(std::move(base), std::move(exponent));


    } else if (token.op == OPERANDS::param) {
        eat(OPERANDS::param);
        return std::make_unique<ParamNode>(token.param_name);

    } else {
        throw std::runtime_error("Invalid expression.");
    }
}

void Parser::eat(OPERANDS type) {
    if (currentToken.op == type) {
        currentToken = getNextToken();
    } else {
        throw std::runtime_error("Invalid token type.");
    }
}

