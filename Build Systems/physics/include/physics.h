#ifndef PHYSICS_H
#define PHYSICS_H



namespace physics_function {

    // Windows DLL export/import declarations
    // When building the physics DLL, PHYSICS_EXPORTS is defined and we export symbols
    // When using the physics DLL, PHYSICS_EXPORTS is not defined and we import symbols
    #if defined(_WIN32) || defined(_WIN64)
        #ifdef PHYSICS_EXPORTS // This is for the physics DLL, defined in CMakeLists.txt of physics project
            #define PHYSICS_API __declspec(dllexport)
        #else
            #define PHYSICS_API __declspec(dllimport) // This is required for calling functions of the physics DLL to be used in other projects, it is not defined in the main project CMakeLists.txt.
        #endif
    #elif defined(__LINUX__) || defined(__APPLE__)
        #define PHYSICS_API __attribute__((visibility("default")))
    #endif

    // Calculate force using Newton's second law: F = m * a (Force = mass * acceleration)
    PHYSICS_API double force(double mass, double acceleration);
    
    // Calculate kinetic energy: KE = 0.5 * m * v^2 (Kinetic Energy = 0.5 * mass * velocity^2)
    PHYSICS_API double kinetic_energy(double mass, double velocity);

    // Calculate potential energy: PE = m * g * h (Potential Energy = mass * gravity * height)
    PHYSICS_API double potential_energy(double mass, double gravity, double height);
}

#endif // PHYSICS_H

