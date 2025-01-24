// Drob.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>

using namespace std;

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
        return Fraction(new_num, new_den);
    }

    Fraction substract(const Fraction& other) const {
        int new_num = numerator * other.denominator - other.numerator * denominator;
        int new_den = denominator * other.denominator;
        return Fraction(new_num, new_den);
    }

    Fraction multiply(const Fraction& other) const {
        int new_num = numerator * other.numerator;
        int new_den = denominator * other.denominator;
        return Fraction(new_num, new_den);
    }

    Fraction divide(const Fraction& other) const {
        if (other.numerator == 0) {
            throw invalid_argument("Cannot divide by zero");
        }
        int new_num = numerator * other.denominator;
        int new_den = denominator * other.numerator;
        return Fraction(new_num, new_den);
    }

    double toDecimal() const {
        return static_cast<double>(numerator) / denominator;
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

int main()
{
    try {
        Fraction a(3, 4);
        Fraction b(2, 5);

        cout << "a + b = " << a + b << endl;
        cout << "a - b = " << a - b << endl;
        cout << "a * b = " << a * b << endl;
        cout << "a / b = " << a / b << endl;

        Fraction c;
        cout << "Enter fraction (format a/b): ";
        cin >> c;
        cout << "You entered: " << c << endl;
        cout << "Decimal: " << c.toDecimal() << endl;

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
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
