#include "math.h"
#include <cmath>
namespace math_function {
    // Calculate square of a number (x^2)
    double square(double x) {
        return x * x;
    }
    
    // Calculate cube of a number (x^3)
    double cube(double x) {
        return x * x * x;
    }
    #ifdef ADVANCED_API
        // Calculate power of a number (base^exponent)
        double power(double base, double exponent) {
            return std::pow(base, exponent);
        }
    #endif
}

