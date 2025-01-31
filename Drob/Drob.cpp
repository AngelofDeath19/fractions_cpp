﻿#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>

class HistoryManager {
private:
    static std::vector<std::string> history; //static vector to save history

public:
    //add to history
    static void addToHistory(const std::string& entry) {
        history.push_back(entry);
    }

    //output in console
    static void displayHistory() {
        std::cout << "Operations history:\n";
        for (size_t i = 0; i < history.size(); ++i) {
            std::cout << "[" << i + 1 << "]" << history.at(i) << std::endl;
        }
    }

    //save history to file
    static void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Error while opening the file: " << filename << std::endl;
            return; 
        }
        for (const auto& entry : history) {
            file << entry << std::endl;
        }
        file.close();
        std::cout << "History is saved in " << filename << std::endl;
    }
};

std::vector<std::string> HistoryManager::history;

class Fraction {
private:
    int numerator;
    int denominator;

    void simplify() {
        int common = gcd(std::abs(numerator), std::abs(denominator));
        numerator /= common;
        denominator /= common;

        if (denominator < 0) {
            numerator *= -1;
            denominator *= -1;
        }
    }

    static int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

public:
    Fraction(int num = 0, int den = 1) : numerator(num), denominator(den) {
        if (denominator == 0) {
            throw std::invalid_argument("Denominator cannot be zero");
        }
        simplify();
    }

    Fraction add(const Fraction& other) const {
        int new_num = numerator * other.denominator + other.numerator * denominator;
        int new_den = denominator * other.denominator;
        Fraction result(new_num, new_den);
        HistoryManager::addToHistory(
            toString() + " + " + other.toString() + " = " + result.toString()
        );
        return result;
    }

    Fraction subtract(const Fraction& other) const {
        int new_num = numerator * other.denominator - other.numerator * denominator;
        int new_den = denominator * other.denominator;
        Fraction result(new_num, new_den);
        HistoryManager::addToHistory(
            toString() + " - " + other.toString() + " = " + result.toString()
        );
        return result;
    }

    Fraction multiply(const Fraction& other) const {
        int new_num = numerator * other.numerator;
        int new_den = denominator * other.denominator;
        Fraction result(new_num, new_den);
        HistoryManager::addToHistory(
            toString() + " * " + other.toString() + " = " + result.toString()
        );
        return result;

    }

    Fraction divide(const Fraction& other) const {
        if (other.numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        int new_num = numerator * other.denominator;
        int new_den = denominator * other.numerator;
        Fraction result(new_num, new_den);
        HistoryManager::addToHistory(
            toString() + " / " + other.toString() + " = " + result.toString()
        );
        return result;
    }

    double toDecimal() const {
        return static_cast<double>(numerator) / denominator;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    Fraction operator+(const Fraction& other) const { return add(other); }
    Fraction operator-(const Fraction& other) const { return subtract(other); }
    Fraction operator*(const Fraction& other) const { return multiply(other); }
    Fraction operator/(const Fraction& other) const { return divide(other); }

    friend std::ostream& operator<<(std::ostream& os, const Fraction& f) {
        os << f.numerator;
        if (f.denominator != 1) os << "/" << f.denominator;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Fraction& f) {
        std::string input;
        is >> input;

        size_t slashPos = input.find('/');
        if (slashPos != std::string::npos) {
            int num = std::stoi(input.substr(0, slashPos));
            int den = std::stoi(input.substr(slashPos + 1));
            f = Fraction(num, den);
        }
        else {
            try {
                int num = std:: stoi(input);
                f = Fraction(num, 1);
            }
            catch (...) {
                f = Fraction(0, 1);
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }
};

//helping functions for parsing
std::vector<std::string> tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string token;
    bool readingNumber = false;
    for (size_t i = 0; i < expression.size(); ++i) {
        char c = expression[i];

        if (isdigit(c) || c == '/' || c == '.' || (c == '-' && (i == 0 || expression[i - 1] == '('))) {
            //adding symbols to number/fraction
            token += c;
            readingNumber = true;
        }
        else {
            if (readingNumber) {
                tokens.push_back(token);
                token.clear();
                readingNumber = false;
            }
            if (!isspace(c)) {
                tokens.push_back(std::string(1, c));
            }
        }
    }

    if (readingNumber) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void applyOperation(std::vector <Fraction>& values, std::vector<std::string>& ops) {
    if (ops.empty() || values.size() < 2) {
        throw std::runtime_error("Not enough operands for operator " + (ops.empty() ? "" : ops.back()));
    }

    std::string op = ops.back();
    ops.pop_back();

    Fraction b = values.back(); values.pop_back();
    Fraction a = values.back(); values.pop_back();

    if (op == "+") values.push_back(a + b);
    else if (op == "-") values.push_back(a - b);
    else if (op == "*") values.push_back(a * b);
    else if (op == "/") values.push_back(a / b);
}


//decimal to fraction convert (by Farey alghoritm)
Fraction decimalToFraction(double decimal, int max_denominator = 1000) {
    int sign = decimal < 0 ? -1 : 1; 
    decimal = fabs(decimal);

    int a = 0, b = 1;
    int c = 1, d = 1;

    while (b <= max_denominator && d <= max_denominator) {
        double mediant = (a + c) / static_cast<double>(b + d);
        if (decimal == mediant) {
            if (b + d <= max_denominator)
                return Fraction(sign * (a + c), b + d);
            else if (b > d)
                return Fraction(sign * c, d);
            else
                return Fraction(sign * a, b);
        }
        else if (decimal > mediant) {
            a += c;
            b += d; 
        }
        else {
            c += a; 
            d += b;
        }
    }

    if (b > max_denominator)
        return Fraction(sign * c, d);
    else
        return Fraction(sign * a, b);
}

//Helping functions


void parseNumber(const std::string& token, std::vector<Fraction>& values) {
    if (token.find('.') != std::string::npos) {
        // decimal handling
        double num = stod(token);
        values.push_back(decimalToFraction(num));
    }
    else if (token.find('/') != std::string::npos) {
        // fraction handling
        size_t slash = token.find('/');
        int num = stoi(token.substr(0, slash));
        int den = stoi(token.substr(slash + 1));
        values.push_back(Fraction(num, den));
    }
    else {
        //int value
        values.push_back(Fraction(stoi(token), 1));
    }
}

// Parser with operation prior handling
Fraction evaluateExpression(const std::string& expr) {
    std::vector<std::string> tokens = tokenize(expr);
    std::vector<Fraction> values;
    std::vector<std::string> ops;
    
    try {
        for (size_t i = 0; i < tokens.size(); ++i) {
            const std::string& token = tokens.at(i);

            if (token == "+" || token == "-" || token == "*" || token == "/") {
                //un minus (is "-" is on start or after "(")
                if (token == "-" && (values.empty() || (!ops.empty() && ops.back() == "("))) {
                    values.push_back(Fraction(0, 1)); // addin zero as left operand
                }

                //priority operations handle
                while (!ops.empty() && ops.back() != "(" && ((token == "+" || token == "-") && (ops.back() == "*" || ops.back() == "/"))) {
                    applyOperation(values, ops);
                }
                ops.push_back(token);
            }
            else if (token == "(") {
                ops.push_back(token);
            }
            else if (token == ")") {
                while (!ops.empty() && ops.back() != "(") {
                    applyOperation(values, ops);
                }
                if (ops.empty()) throw std::runtime_error("Mismatched parentheses");
                ops.pop_back(); //deleting "("
            }
            else {
                //parsing values and fractions
                parseNumber(token, values);
            }
        }

        while (!ops.empty()) {
            if (ops.back() == "(") throw std::runtime_error("Missmatched parentheses");
            applyOperation(values, ops);
        }

        if (values.size() != 1) {
            throw std::runtime_error("Invalid expression - missing operations or operands");
        }

        return values.back();
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Expression error: ") + e.what());
    }
}

//main func to work with console
void consoleInterface() {
    std::string input;
    std::cout << "Fraction calculator (type 'exit' to quit)\n";

    while (true) {
        std::cout << ">";
        std::getline(std::cin, input);
        if (input == "exit") break;
        if (input == "history") {
            HistoryManager::displayHistory();
            continue;
        }

        try {
            Fraction result = evaluateExpression(input);
            std::cout << "Result: " << result << " = " << result.toDecimal() << "\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

int main()
{
    consoleInterface();
    return 0;
}