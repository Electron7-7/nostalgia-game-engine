#ifndef DEBUGGING_H
#define DEBUGGING_H

#include <iostream> // IWYU pragma: keep // Used in macro

// Yes, this is technically a debugging tool, but it's only used in cases where the entire program has a high likelihood of crashing so
// I've decided to make sure it always prints, even in the release builds.
#define PRINTCATASTROPHIC(content) std::cerr << std::string("[CATASTROPHIC ERROR]\t" + std::string(content)) << std::endl

#ifdef DEBUGGING // See Makefile

// PRINTOUT MACROS
#define PRINT(content) std::cout << std::string(content) << std::endl
#define PRINTLABEL(label, content) std::cout << std::string(std::string(label) + "\t") << std::string(content) << std::endl
#define PRINTERR(content) PRINTLABEL("[ERROR]", content)
#define PRINTWARN(content) PRINTLABEL("[WARNING]", content)
#define PRINTDEBUG(content) PRINTLABEL("[DEBUG]", content)

#else // DEBUGGING

#define PRINT(content)
#define PRINTERR(content)
#define PRINTLABEL(label, content)
#define PRINTWARN(content)
#define PRINTDEBUG(content)

#endif // DEBUGGING
#endif // DEBUGGING_H
