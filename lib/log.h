//
// Created by Andres Fuentes Hernandez on 8/31/24.
//

#ifndef RETRO_EMULATOR_LIB_LOG_H_
#define RETRO_EMULATOR_LIB_LOG_H_

#include <stdio.h>
#include <stdlib.h>

// ANSI color codes for improved readability
#define RESET_COLOR  "\033[0m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED    "\033[31m"
#define BOLD_TEXT    "\033[1m"


#ifdef DEBUG
#define LOG_SEPARATOR printf(BOLD_TEXT"%s"RESET_COLOR,"------------------------------------------------------------------------------------------------------------------------------------\n")
#define LOG(...) printf(__VA_ARGS__)
#define ERROR_LOG(...) fprintf(stderr, COLOR_RED "[ERROR] " __VA_ARGS__ RESET_COLOR)
#else
#define LOG_SEPARATOR
#define LOG(...)
#define ERROR_LOG(...)
#endif

#endif //RETRO_EMULATOR_LIB_LOG_H_
