#ifndef ANSI_SEQUENCE_H
#define ANSI_SEQUENCE_H

using ansi_code_t = std::vector<ansi_t>;
using ansi_sequence_t = std::vector<ansi_code_t>;

template<typename T>
    concept ansi_t_strict = std::same_as<T, ansi_t> and not std::same_as<T, bool>;

struct ANSI_Sequence
{
public:
    ANSI_Sequence() noexcept;
    ANSI_Sequence(ansi_t inColorCode, bool isForeground, bool isBold = false) noexcept;
    ANSI_Sequence(const ansi_sequence_t& inSequence) noexcept;
    ANSI_Sequence(const std::string& inSequence) noexcept;

    template<ansi_t_strict... Codes>
        ANSI_Sequence(Codes... inCodes) noexcept
        { setup_codes({inCodes...}); }

    static std::string convert_code(ansi_t inCode) noexcept;

    void operator()(std::string& outString, bool doClearStringFirst = true) const noexcept;
    std::string operator()() const noexcept;

    void get(std::string& outString, bool doClearStringFirst = true) const noexcept;
    std::string get() const noexcept;

    void append_to(std::string& outString) const noexcept;
    std::string append(const std::string& inString) const noexcept;

    void prepend_to(std::string& outString) const noexcept;
    std::string prepend(const std::string& inString) const noexcept;

    void insert_in(std::string& outString, size_t inInsertIndex = 0) const noexcept;
    std::string insert(const std::string& inString, size_t inInsertIndex = 0) const noexcept;

    const ansi_sequence_t& data() const noexcept;

private:
    ansi_sequence_t codes_{};

    void setup_codes(std::initializer_list<ansi_t> inCodes) noexcept;
};

#endif // ANSI_SEQUENCE_H
