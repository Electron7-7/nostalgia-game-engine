#include "ansi_codes.hpp"

ANSI_Sequence::ANSI_Sequence() noexcept = default;

ANSI_Sequence::ANSI_Sequence(ansi_t inColor, bool isForeground, bool isBold) noexcept
{
    codes_.emplace_back(ansi_code_t{ANSI::begin,
        (isForeground)
            ? (isBold)
                ? ANSI::bold_foreground
                : ANSI::foreground
            : (isBold)
                ? ANSI::bold_background
                : ANSI::background,
        inColor,
        ANSI::end});
}

// My cheap trick to keeping the template constructor's definition out of the header file
void ANSI_Sequence::setup_codes(std::initializer_list<ansi_t> inCodes) noexcept
{
    ansi_code_t temp{};
    for(ansi_t code : inCodes)
    {
        temp.emplace_back(code);
        if(code == ANSI::end)
        {
            codes_.emplace_back(temp);
            temp.clear();
        }
    }
}

std::string ANSI_Sequence::convert_code(ansi_t inCode) noexcept
{
    switch(inCode)
    {
    case static_cast<int>(ANSI::begin):
        return "\x1b[";
    case static_cast<int>(ANSI::next):
        return ";";
    case static_cast<int>(ANSI::end):
        return "m";
    case static_cast<int>(ANSI::invalid):
        return "";
    default:
        return std::to_string(inCode);
    }
}

void ANSI_Sequence::operator()(std::string& outString, bool doClearString) const noexcept
{
    if(doClearString)
        { outString.clear(); }
    for(auto& sequence : codes_)
    {
        for(int code : sequence)
            { outString.append(convert_code(code)); }
    }
}

std::string ANSI_Sequence::operator()() const noexcept
{
    std::string output{};
    operator()(output, false);
    return output;
}

void ANSI_Sequence::get(std::string& outString, bool doClearString) const noexcept
{ operator()(outString, doClearString); }

std::string ANSI_Sequence::get() const noexcept
{ return operator()(); }

void ANSI_Sequence::append_to(std::string& outString) const noexcept
{ get(outString, false); }

std::string ANSI_Sequence::append(const std::string& inString) const noexcept
{ return inString + get(); }

void ANSI_Sequence::prepend_to(std::string& outString) const noexcept
{ outString = get() + outString; }

std::string ANSI_Sequence::prepend(const std::string& inString) const noexcept
{ return get() + inString; }

void ANSI_Sequence::insert_in(std::string& outString, size_t inInsertIndex) const noexcept
{ outString.insert(inInsertIndex, get()); }

std::string ANSI_Sequence::insert(const std::string& inString, size_t inInsertIndex) const noexcept
{ return std::string{inString}.insert(inInsertIndex, get()); }

const ansi_sequence_t& ANSI_Sequence::data() const noexcept
{ return codes_; }
