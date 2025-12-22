#ifndef MATH_H
#define MATH_H

namespace math_function {
    // Calculate square of a number (x^2)
    double square(double x);
    
    // Calculate cube of a number (x^3)
    double cube(double x);
    
    #ifdef ADVANCED_API
        // Calculate power of a number (base^exponent)
        double power(double base, double exponent);
    #endif
}

#endif // MATH_H

