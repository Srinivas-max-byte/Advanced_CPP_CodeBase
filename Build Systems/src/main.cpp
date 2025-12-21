#include <iostream>
#include "Calculator.h"
#include "maths.h"

int main() {
    Calculator calc;
    double num1, num2;
    char operation;
    
    std::cout << "=== Basic Calculator ===" << std::endl;
    std::cout << "Enter first number: ";
    std::cin >> num1;
    
    std::cout << "Enter operation (+, -, *, /): ";
    std::cin >> operation;
    
    std::cout << "Enter second number: ";
    std::cin >> num2;
    
    double result = 0.0;
    
    try {
        switch (operation) {
            case '+':
                result = calc.add(num1, num2);
                break;
            case '-':
                result = calc.subtract(num1, num2);
                break;
            case '*':
                result = calc.multiply(num1, num2);
                break;
            case '/':
                result = calc.divide(num1, num2);
                break;
            default:
                std::cout << "Invalid operation!" << std::endl;
                return 1;
        }
        
        std::cout << "Result: " << num1 << " " << operation << " " << num2 << " = " << result << std::endl;
        
        // Using math_function namespace to calculate square of the result
        double squaredResult = math_function::square(2);
        std::cout << "Square of result: " << squaredResult << std::endl;
        
        std::cout << "Press c to continue...";
        std::cin>>num1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

