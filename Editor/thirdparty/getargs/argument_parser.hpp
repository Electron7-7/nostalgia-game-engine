#ifndef ARGUMENTS_PARSER_H
#define ARGUMENTS_PARSER_H

#include "argument.hpp"

#include <vector>

#define ARG_STATUS_FAILED -1

class ArgumentParser
{
public:
    static void AddOption(Option* Option);
    static void AddFlag(Flag* Flag);

    // Parses and updates all arguments added via 'AddOption' and 'AddFlag'.
    // Iterates through 'Arguments' instead of 'argv' directly.
    // Returns either 'ARG_STATUS_FAILED' or the index, in 'Arguments', of the last argument parsed.
    static int ParseArguments(int NumberOfArguments, char* Arguments[]);

private:
    static std::vector<Option*> _options;
    static std::vector<Flag*> _flags;
};

extern ArgumentParser* global_ArgumentParser;

#endif // ARGUMENTS_PARSER_H
