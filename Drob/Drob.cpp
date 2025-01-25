// Drob.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>

using namespace std;

class HistoryManager {
private:
    static vector<string> history; //static vector to save history

public:
    //add to history
    static void addToHistory(const string& entry) {
        history.push_back(entry);
    }

    //output in console
    static void displayHistory() {
        cout << "Operations history:\n";
        for (size_t i = 0; i < history.size(); ++i) {
            cout << "[" << i + 1 << "]" << history[i] << endl; 
        }
    }

    //save history to file
    static void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error while opening the file!" << endl;
            return; 
        }
        for (const auto& entry : history) {
            file << entry << endl;
        }
        file.close();
        cout << "History is saved in " << filename << endl;
    }
};

vector<string> HistoryManager::history;

class Fraction {
private:
    int numerator;
    int denominator;

    void simplify() {
        int common = gcd(abs(numerator), abs(denominator));
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
            throw invalid_argument("Denominator cannot be zero");
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

    Fraction substract(const Fraction& other) const {
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
            throw invalid_argument("Cannot divide by zero");
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

    string toString() const {
        ostringstream oss;
        oss << *this;
        return oss.str();
    }

    Fraction operator+(const Fraction& other) const { return add(other); }
    Fraction operator-(const Fraction& other) const { return substract(other); }
    Fraction operator*(const Fraction& other) const { return multiply(other); }
    Fraction operator/(const Fraction& other) const { return divide(other); }

    friend ostream& operator<<(ostream& os, const Fraction& f) {
        os << f.numerator;
        if (f.denominator != 1) os << "/" << f.denominator;
        return os;
    }

    friend istream& operator>>(istream& is, Fraction& f) {
        string input;
        is >> input;

        size_t slashPos = input.find('/');
        if (slashPos != string::npos) {
            int num = stoi(input.substr(0, slashPos));
            int den = stoi(input.substr(slashPos + 1));
            f = Fraction(num, den);
        }
        else {
            try {
                int num = stoi(input);
                f = Fraction(num, 1);
            }
            catch (...) {
                f = Fraction(0, 1);
                is.setstate(ios::failbit);
            }
        }
        return is;
    }
};

//helping functions for parsing
vector<string> tokenize(const string& expression) {
    vector<string> tokens;
    string token;
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
                tokens.push_back(string(1, c));
            }
        }
    }

    if (readingNumber) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void applyOperation(vector <Fraction>& values, vector<string>& ops) {
    if (ops.empty() || values.size() < 2) {
        throw runtime_error("Not enough operands for operator " + (ops.empty() ? "" : ops.back()));
    }

    string op = ops.back();
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


void parseNumber(const string& token, vector<Fraction>& values) {
    if (token.find('.') != string::npos) {
        // decimal handling
        double num = stod(token);
        values.push_back(decimalToFraction(num));
    }
    else if (token.find('/') != string::npos) {
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
Fraction evaluateExpression(const string& expr) {
    vector<string> tokens = tokenize(expr);
    vector<Fraction> values;
    vector<string> ops;
    
    try {
        for (size_t i = 0; i < tokens.size(); ++i) {
            const string& token = tokens[i];

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
                if (ops.empty()) throw runtime_error("Mismatched parentheses");
                ops.pop_back(); //deleting "("
            }
            else {
                //parsing values and fractions
                parseNumber(token, values);
            }
        }

        while (!ops.empty()) {
            if (ops.back() == "(") throw runtime_error("Missmatched parentheses");
            applyOperation(values, ops);
        }

        if (values.size() != 1) {
            throw runtime_error("Invalid expression - missing operations or operands");
        }

        return values.back();
    }
    catch (const exception& e) {
        throw runtime_error(string("Expression error: ") + e.what());
    }
}

//main func to work with console
void consoleInterface() {
    string input;
    cout << "Fraction calculator (type 'exit' to quit)\n";

    while (true) {
        cout << ">";
        getline(cin, input);
        if (input == "exit") break;
        if (input == "history") {
            HistoryManager::displayHistory();
            continue;
        }

        try {
            Fraction result = evaluateExpression(input);
            cout << "Result: " << result << " = " << result.toDecimal() << "\n";
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }
}

int main()
{
    consoleInterface();
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
