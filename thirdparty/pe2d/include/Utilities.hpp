#pragma once

#include <iostream>

#define DEBUG

#ifdef DEBUG
    template<typename ...T>
    void Print(T&&... args) 
    {
	    (std::cout << ... << args);
    }
    #define LogCall(...) Print(__VA_ARGS__)
#else
    #define LogCall(...) 
#endif

#define ASSERT(x) errorMessage(__FILE__, __LINE__, x);\
    std::exit(1)

inline void errorMessage(const char *file, int line, const char *msg)
{
    std::cout << file << " at line " << line << " ERROR: " <<  msg << std::endl;
}

