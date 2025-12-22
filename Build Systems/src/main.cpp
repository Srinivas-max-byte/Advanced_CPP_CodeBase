#include <iostream>
#include "Calculator.h"
#include "maths.h"
#include "physics.h"

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
        double squaredResult = math_function::square(result);
        std::cout << "Square of result: " << squaredResult << std::endl;
        
        #ifdef ADVANCED_API // This is for the advanced API
        // Using power function from maths library
        double powerResult = math_function::power(result, 3);
        std::cout << "Result raised to the power of 3: " << powerResult << std::endl;
        #endif
        
        // Using physics_function namespace to demonstrate physics calculations
        std::cout << "\n=== Physics Calculations ===" << std::endl;
        
        // Calculate force: F = m * a (using result as mass and a sample acceleration)
        double sampleMass = result;
        double sampleAcceleration = 9.8; // m/s^2 (Earth's gravity)
        double forceResult = physics_function::force(sampleMass, sampleAcceleration);
        std::cout << "Force (mass=" << sampleMass << " kg, acceleration=" << sampleAcceleration << " m/s^2): " << forceResult << " N" << std::endl;
        
        // Calculate kinetic energy: KE = 0.5 * m * v^2 (using result as mass and a sample velocity)
        double sampleVelocity = 10.0; // m/s
        double kineticEnergyResult = physics_function::kinetic_energy(sampleMass, sampleVelocity);
        std::cout << "Kinetic Energy (mass=" << sampleMass << " kg, velocity=" << sampleVelocity << " m/s): " << kineticEnergyResult << " J" << std::endl;
        
        
        // Calculate potential energy: PE = m * g * h (using result as mass, standard gravity, and sample height)
        double standardGravity = 9.8; // m/s^2 (Earth's gravitational acceleration)
        double sampleHeight = 5.0; // meters
        double potentialEnergyResult = physics_function::potential_energy(sampleMass, standardGravity, sampleHeight);
        std::cout << "Potential Energy (mass=" << sampleMass << " kg, gravity=" << standardGravity << " m/s^2, height=" << sampleHeight << " m): " << potentialEnergyResult << " J" << std::endl;
        
        std::cout << "Press c to continue...";
        std::cin>>num1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

