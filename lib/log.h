//
// Created by Andres Fuentes Hernandez on 8/31/24.
//

#ifndef RETRO_EMULATOR_LIB_LOG_H_
#define RETRO_EMULATOR_LIB_LOG_H_

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
#define LOG_MEMORY(start, end, arr)  for(uint16_t i=start; i< end; i++) printf("[MEM] address:0x%04X\tvalue:0x%02X\n", i, arr[i])
#else
#define LOG_SEPARATOR
#define LOG(...)
#define ERROR_LOG(...)
#define LOG_MEMORY(start, end, arr)
#endif

#endif //RETRO_EMULATOR_LIB_LOG_H_
