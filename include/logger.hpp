#pragma once
// Библиотеки C++
#include <iostream>

typedef enum {
    ERROR   = 0,
    INFO    = 100,
    WARNING = 200,
    NOTICE  = 300,
    DEBUG   = 400
} prio_level_t;

extern int log_level;
#define LOG(__level) \
    if(__level <= log_level) \
        std::cout << "[" << #__level << "](" << __FILE__ << ":" << __LINE__ << "): "

#define ASSERT(__cond) \
    if(!(__cond)) { \
        std::cerr << "ASSERTION FAILED\n\tfile: " << __FILE__ << "\n\tline: " << __LINE__ << "\n\tcond: " << #__cond << "\n"; \
        exit(EXIT_FAILURE); \
    }
        

inline void init_logger(int verbose) {
    switch(verbose) {
        case 2: log_level = DEBUG;
        case 1: log_level = NOTICE;
        default: log_level = INFO;
    }
}
