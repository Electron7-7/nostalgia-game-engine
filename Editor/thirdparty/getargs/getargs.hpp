// Shout out to Sean Barrett aka: nothings on GitHub (creator of STB, the image handling library). I tried to
// follow his advice on header-file libraries: https://github.com/nothings/stb/blob/master/docs/stb_howto.txt

#ifndef GETARGS_HEADER_GUARD
#define GETARGS_HEADER_GUARD

#ifdef GETARGS_IMPLEMENTATION

#ifdef GETARGS_HANDLE_HELP_FLAG
#   ifndef GET_ARGS_RET_VALUE
#       define GET_ARGS_RET_VALUE getargs::Status
#   endif

#   ifdef GETARGS_EXIT_ON_HELP
#       define GETARGS_RETURN_FROM_HELP exit(0)
#   else
#       define GETARGS_RETURN_FROM_HELP return getargs::HELP_CALLED
#   endif // GETARGS_EXIT_ON_HELP

#   ifndef GETARGS_LONG_HELP_FLAG
#      define GETARGS_LONG_HELP_FLAG "--help"
#   endif

#   if !defined GETARGS_SHORT_HELP_FLAG && !defined GETARGS_NO_SHORT_HELP_FLAG
#      define GETARGS_SHORT_HELP_FLAG "-h"
#   endif

#   ifndef GETARGS_HELP_PRINTOUT
#pragma message("Hey! You forgot to define GETARGS_HELP_PRINTOUT! You need to do that, dummy!")
#      define GETARGS_HELP_PRINTOUT R"(This user didn't make a help printout. How rude!)"
#   endif
#endif // GETARGS_HANDLE_HELP_FLAG

#ifdef GETARGS_HANDLE_VERSION_FLAG
#   ifndef GET_ARGS_RET_VALUE
#       define GET_ARGS_RET_VALUE getargs::Status
#   endif

#   ifdef GETARGS_EXIT_ON_VERSION
#       define GETARGS_RETURN_FROM_VERSION exit(0)
#   else
#       define GETARGS_RETURN_FROM_VERSION return getargs::VERSION_CALLED
#   endif

#   ifndef GETARGS_LONG_VERSION_FLAG
#      define GETARGS_LONG_VERSION_FLAG "--version"
#   endif

#   if !defined GETARGS_SHORT_VERSION_FLAG && !defined GETARGS_NO_SHORT_VERSION_FLAG
#      define GETARGS_SHORT_VERSION_FLAG "-v"
#   endif

#   ifndef GETARGS_VERSION_PRINTOUT
#pragma message("Hey! You forgot to define GETARGS_VERSION_PRINTOUT! \
    It's not, like, a big deal but you should think about doing that")
#      define GETARGS_VERSION_PRINTOUT R"(i-forgor v0.0.0)"
#   endif
#endif // GETARGS_HANDLE_VERSION_FLAG

#ifdef GETARGS_HANDLE_INVALID_ARGS
#   ifndef GET_ARGS_RET_VALUE
#       define GET_ARGS_RET_VALUE getargs::Status
#   endif

#   ifdef GETARGS_EXIT_ON_INVALID_ARGS
#       define GETARGS_RETURN_FROM_INVALID_ARGUMENT exit(-1)
#   else
#       define GETARGS_RETURN_FROM_INVALID_ARGUMENT return getargs::INVALID_ARGUMENT
#   endif
#endif // GETARGS_HANDLE_INVALID_ARGS

#ifdef GETARGS_NAMESPACE_NAME_OVERRIDE
#   define GETARGS_NAMESPACE GETARGS_NAMESPACE_NAME_OVERRIDE
#else
#   define GETARGS_NAMESPACE getargs
#endif // GETARGS_NAMESPACE_NAME_OVERRIDE

#ifndef GETARGS_STRING_t
#   include <string>
#   define GETARGS_STRING_t std::string
#endif // GETARGS_STRING_t

#ifndef GETARGS_MAP_t
#   include <map>
#   define GETARGS_MAP_t(KEY, VALUE) std::map<KEY, VALUE>
#endif // GETARGS_MAP_t

#ifndef GETARGS_SET_t
#   include <set>
#   define GETARGS_SET_t(KEY) std::set<KEY>
#endif // GETARGS_SET_t

#ifndef GETARGS_VECTOR_t
#   include <vector>
#   define GETARGS_VECTOR_t(KEY) std::vector<KEY>
#endif // GETARGS_VECTOR_t

#ifdef GETARGS_HANDLE_INVALID_ARGS
#   ifndef GETARGS_SAMEAS_c
#      include <concepts>
#      define GETARGS_SAMEAS_c(T, U) std::same_as<T, U>
#   endif

#   ifndef GETARGS_CONVERTIBLE_TO_c
#      include <concepts>
#      define GETARGS_CONVERTIBLE_TO_c(FROM, TO) std::convertible_to<FROM, TO>
#   endif
#endif // GETARGS_HANDLE_INVALID_ARGS

#define GETARGS_STRINGIFY(TO_STRING) __GETARGS_STRINGIFY(TO_STRING)
#define __GETARGS_STRINGIFY(TO_STRING) #TO_STRING

namespace GETARGS_NAMESPACE
{
    enum Status : int { INVALID_ARGUMENT=1, HELP_CALLED=2, VERSION_CALLED=3 };

    namespace __hidden
    {
        enum ArgType : int
        {
            ARG_TYPE_LONG_FLAG,
            ARG_TYPE_SHORT_FLAG,
            ARG_TYPE_OPTION,
            ARG_TYPE_VALUE
        };

        static GETARGS_SET_t(GETARGS_STRING_t)                          flags{};
        static GETARGS_MAP_t(GETARGS_STRING_t, GETARGS_STRING_t)      options{};
        static const GETARGS_STRING_t                            empty_string{};
#ifdef GETARGS_HANDLE_INVALID_ARGS
        static GETARGS_SET_t(GETARGS_STRING_t)                     valid_args{};
        static GETARGS_STRING_t                                       bad_arg{};
#endif // GETARGS_HANDLE_INVALID_ARGS

        static GETARGS_VECTOR_t(GETARGS_STRING_t) get_parsed_args(int const& argc, char** const& argv)
        {
            GETARGS_VECTOR_t(GETARGS_STRING_t) parsed_args{};
            for(int i{1}; i < argc; ++i)
            {
                GETARGS_STRING_t arg{argv[i]};
                if(arg.contains('='))
                {
                    GETARGS_STRING_t buffer{};
                    for(const auto& character : arg)
                    {
                        if(character == '=')
                        {
                            parsed_args.push_back(buffer);
                            buffer.clear();
                            continue;
                        }
                        buffer.push_back(character);
                    }
                    parsed_args.push_back(buffer);
                }
                else
                    { parsed_args.push_back(arg); }
            }
            return parsed_args;
        }

#ifdef GETARGS_HANDLE_INVALID_ARGS
        static bool is_valid(const std::string& inArg)
        { return __hidden::valid_args.contains(inArg); }
#endif // GETARGS_HANDLE_INVALID_ARGS

        static bool is_flag_or_option(const std::string& inArg)
        { return not inArg.empty() and inArg.starts_with('-'); }

        static bool is_short(const GETARGS_STRING_t& inArg)
        { return (inArg.size() > 0 and inArg[0] == '-') and (inArg.size() < 2 or inArg[1] != '-'); }

        static ArgType get_type(int inIndex, const GETARGS_VECTOR_t(GETARGS_STRING_t)& inVector)
        {
            GETARGS_STRING_t arg{inVector[inIndex]};
            int next_index{inIndex + 1};

            if(not is_flag_or_option(arg))
                { return ARG_TYPE_VALUE; }
            else if(next_index < inVector.size() and not is_flag_or_option(inVector[next_index]))
                { return ARG_TYPE_OPTION; }
            else if(is_short(arg))
                { return ARG_TYPE_SHORT_FLAG; }
            return ARG_TYPE_LONG_FLAG;
        }

        static GETARGS_STRING_t get_debug_logs()
        {
            GETARGS_STRING_t output{""};
#ifdef GETARGS_HANDLE_INVALID_ARGS
            output += "getargs::__hidden::bad_arg: '" + getargs::__hidden::bad_arg + "'\n";
#endif // GETARGS_HANDLE_INVALID_ARGS
            output += "getargs::__hidden::flags: [";
            for(auto flag{flags.cbegin()}; flag != flags.cend(); ++flag)
            {
                output += "'" + *flag + ((flag == --flags.end())
                    ? "'"
                    : "', ");
            }
            output += "]\ngetargs::__hidden::options: {";
            for(auto option{options.cbegin()}; option != options.cend(); ++option)
            {
                output += "['" + option->first + "', '" + option->second + ((option == --options.cend())
                    ? "']"
                    : "'], ");
            }
            return output += '}';
        }
    }

#ifdef GETARGS_HANDLE_INVALID_ARGS
    template<typename T>
        concept String_t = GETARGS_SAMEAS_c(T, GETARGS_STRING_t)
            or GETARGS_CONVERTIBLE_TO_c(T, GETARGS_STRING_t);

    template<String_t... Args>
        static void set_valid_args(Args... inArgs)
        {
            for(const auto& arg : {inArgs...})
                { __hidden::valid_args.insert(arg); }
        }
#endif // GETARGS_HANDLE_INVALID_ARGS

    static int get_args(int const& argc, char** const& argv)
    {
        __hidden::flags.clear();
        __hidden::options.clear();

        GETARGS_STRING_t option_store{};
        GETARGS_VECTOR_t(GETARGS_STRING_t) args{__hidden::get_parsed_args(argc, argv)};
        size_t args_size{args.size()};

        for(size_t i{0}; i < args_size; ++i)
        {
            const GETARGS_STRING_t& arg{args[i]};
#ifdef GETARGS_HANDLE_INVALID_ARGS
            if(not __hidden::is_valid(arg))
            {
                __hidden::bad_arg = arg;
                fprintf(stderr, "Unkown command line argument: '%s'\n", arg.data());
                GETARGS_RETURN_FROM_INVALID_ARGUMENT;
            }
#endif // GETARGS_HANDLE_INVALID_ARGS
#ifdef GETARGS_HANDLE_HELP_FLAG
            else if(not arg.compare(GETARGS_LONG_HELP_FLAG)
#   ifndef GETARGS_NO_SHORT_HELP_FLAG
                or not arg.compare(GETARGS_SHORT_HELP_FLAG)
#   endif // GETARGS_NO_SHORT_HELP_FLAG
            )
            {
                fprintf(stdout, "%s\n", GETARGS_HELP_PRINTOUT);
                GETARGS_RETURN_FROM_HELP;
            }
#endif // GETARGS_HANDLE_HELP_FLAG
#ifdef GETARGS_HANDLE_VERSION_FLAG
            else if(not arg.compare(GETARGS_LONG_VERSION_FLAG)
#   ifndef GETARGS_NO_SHORT_VERSION_FLAG
                or not arg.compare(GETARGS_SHORT_VERSION_FLAG)
#   endif // GETARGS_NO_SHORT_VERSION_FLAG
            )
            {
                fprintf(stdout, "%s\n", GETARGS_VERSION_PRINTOUT);
                GETARGS_RETURN_FROM_VERSION;
            }
#endif // GETARGS_HANDLE_VERSION_FLAG
            switch(__hidden::get_type(i, args))
            {
            case __hidden::ARG_TYPE_VALUE:
                if(not option_store.empty())
                {
                    __hidden::options[option_store] = arg;
                    option_store.clear();
                }
                break;
            case __hidden::ARG_TYPE_SHORT_FLAG:
            {
                size_t arg_size{arg.size()};
                for(int j{1}; j < arg_size; ++j)
                    { __hidden::flags.insert({arg[j]}); }
                break;
            }
            case __hidden::ARG_TYPE_LONG_FLAG:
                __hidden::flags.insert(arg.substr(2));
                break;
            case __hidden::ARG_TYPE_OPTION:
                option_store = arg.substr((__hidden::is_short(arg)) ? 1 : 2);
                break;
            }
        }
        return 0;
    }

#ifdef GETARGS_HANDLE_INVALID_ARGS
    static const GETARGS_STRING_t get_bad_arg()
    { return __hidden::bad_arg; }
#endif // GETARGS_HANDLE_INVALID_ARGS

    static bool get_flag(const GETARGS_STRING_t& inFlag)
    { return __hidden::flags.contains(inFlag); }

    static bool get_option(const GETARGS_STRING_t& inOption, GETARGS_STRING_t& outValue)
    {
        if(auto found_it{__hidden::options.find(inOption)}; found_it != __hidden::options.end())
        {
            outValue = found_it->second;
            return true;
        }
        return false;
    }

    static const GETARGS_STRING_t& get_option(const GETARGS_STRING_t& inOption)
    {
        if(auto found_it{__hidden::options.find(inOption)}; found_it != __hidden::options.end())
            { return found_it->second; }
        return __hidden::empty_string;
    }

    static bool argument_exists(const GETARGS_STRING_t& inArg)
    { return __hidden::options.contains(inArg) or __hidden::flags.contains(inArg); }

    static bool option_has_value(const GETARGS_STRING_t& inOption)
    {
        auto found_it{__hidden::options.find(inOption)};
        return (found_it != __hidden::options.end() and not found_it->second.empty());
    }
}

#endif // GETARGS_IMPLEMENTATION

#endif // GETARGS_HEADER_GUARD
