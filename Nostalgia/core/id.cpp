// uint ID::Generate() -> Defined in 'uid.cpp' so it can use that file's static random number generator

PID::PID() noexcept:
    ID{Invalid}, name_{"N/A"} {}

PID::PID(uint inID, Sarg inName) noexcept:
    ID{inID}, name_{inName} {}

PID::PID(Sarg inHashable) noexcept:
    ID{inHashable}, name_{inHashable} {}

PID::PID(const char* inHashable) noexcept:
    ID{inHashable}, name_{inHashable} {}

PID::~PID() noexcept = default;

Sarg PID::name() const
{ return name_; }

const char* PID::c_name() const
{ return name_.data(); }

std::string PID::log() const
{
    return std::format("[{}, {}]",
        id_,
        (invalid())
            ? "Invalid"
            : name_);
}
