#pragma once
#include <iostream>
class LoggerStdOut {
public:
    static void log() {
        std::cout << std::endl;
    }

    template <typename FirstT, typename ...TailT>
    static void log(const FirstT & a1,  TailT ... args) {
        std::cout << a1;
        if constexpr(sizeof...(args) > 0) {
            std::cout << " ";
        }
        log(args...);
    }
};

class LoggerSilent {
public:
    static void log(...) {
        
    }
};