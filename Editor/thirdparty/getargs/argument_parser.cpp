#include "argument_parser.hpp"

#include <string>

ArgumentParser static_ArgumentParser;
ArgumentParser* global_ArgumentParser = &static_ArgumentParser;

std::vector<Option*> ArgumentParser::_options = {};
std::vector<Flag*> ArgumentParser::_flags = {};

void ArgumentParser::AddOption(Option* new_option)
{
    for(Option* added_option : _options)
    { if(added_option == new_option) { return; } }

    _options.insert(_options.end(), new_option);
}

void ArgumentParser::AddFlag(Flag* new_flag)
{
    for(Flag* added_flag : _flags)
    { if(added_flag == new_flag) { return; } }

    _flags.insert(_flags.end(), new_flag);
}

int ArgumentParser::ParseArguments(int argc, char** argv)
{
    unsigned int index_of_positional_arguments = 1;

    for(unsigned int i = 1 ; i < argc ; i++)
    {
        for(unsigned int i_f = 0 ; i_f < _flags.size() ; i_f++)
        {
            if(*_flags.at(i_f) == argv[i])
            {
                index_of_positional_arguments++;

                _flags.at(i_f)->Activate();
                break;
            }
        }

        for(unsigned int i_o = 0 ; i_o < _options.size() ; i_o++)
        {
            if(*_options.at(i_o) == argv[i])
            {
                index_of_positional_arguments++;

                if((i+1) >= argc || std::string(argv[i+1])[0] == '-') // No argument was given to this option (end of array OR next item is most likely another flag/option)
                {
                    if(_options.at(i_o)->IsOptionMandatory())
                    {
                        printf("\x1b[1;31m[ERROR] getargs - Option '%s' requires a value and was given none!\n", _options.at(i_o)->PrettyName());
                        return ARG_STATUS_FAILED;
                    }
                    _options.at(i_o)->SetValue("");
                    _options.at(i_o)->Activate();
                    break;
                }

                _options.at(i_o)->SetValue(argv[++i]);
                _options.at(i_o)->Activate();
                break;
            }
        }
    }

    _options.clear();
    _flags.clear();

    return index_of_positional_arguments;
}
