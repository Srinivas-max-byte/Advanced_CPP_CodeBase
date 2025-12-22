#include "physics.h"
#include <cmath>
namespace physics_function {
    // Calculate force using Newton's second law: F = m * a (Force = mass * acceleration)
    double force(double mass, double acceleration) {
        return mass * acceleration;
    }
    
    // Calculate kinetic energy: KE = 0.5 * m * v^2 (Kinetic Energy = 0.5 * mass * velocity^2)
    double kinetic_energy(double mass, double velocity) {
        return 0.5 * mass * velocity * velocity;
    }
    
    // Calculate potential energy: PE = m * g * h (Potential Energy = mass * gravity * height)
    double potential_energy(double mass, double gravity, double height) {
        return mass * gravity * height;
    }
}

