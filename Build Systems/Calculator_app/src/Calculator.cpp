#include "Calculator.h"
#include <stdexcept>

Calculator::Calculator() {
    // Constructor implementation
}

Calculator::~Calculator() {
    // Destructor implementation
}

double Calculator::add(double a, double b) {
    return a + b;
}

double Calculator::subtract(double a, double b) {
    return a - b;
}

double Calculator::multiply(double a, double b) {
    return a * b;
}

double Calculator::divide(double a, double b) {
    if (b == 0.0) {
        // Handle division by zero
        throw std::runtime_error("Division by zero is not allowed");
    }
    return a / b;
}

