#ifndef DEBUGGING_H
#define DEBUGGING_H

#include <iostream> // IWYU pragma: keep // Used in macro

// PRINTOUT MACROS
#ifdef DEBUGGING // See Makefile

#define PRINT(content) std::cout << std::string(content) << "\n"
#define PRINTLABEL(label, content) std::cout << std::string(std::string(label) + "\t") << std::string(content) << "\n"
#define PRINTERR(content) PRINTLABEL("[ERROR]", content)
#define PRINTWARN(content) PRINTLABEL("[WARNING]", content)
#define PRINTDEBUG(content) PRINTLABEL("[DEBUG]", content)

#else

#define PRINT(content)
#define PRINTERR(content)
#define PRINTLABEL(label, content)
#define PRINTWARN(content)
#define PRINTDEBUG(content)

#endif // DEBUGGING
#endif // DEBUGGING_H
