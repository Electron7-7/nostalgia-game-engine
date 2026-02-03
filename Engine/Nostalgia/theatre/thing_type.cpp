ThingType::ThingType(FPID inTypeID) noexcept:
    _type_id{inTypeID} {}

ThingType::ThingType() noexcept:
    _type_id{ThingType::Thing},
    _base_type_id{ThingType::Thing},
    _all_base_types{} {}

Farg<ThingType::typeids_t> ThingType::base_types() const noexcept
{ return _all_base_types; }

PID ThingType::base_type() const noexcept
{ return _base_type_id; }

PID ThingType::type() const noexcept
{ return _type_id; }

PID ThingType::operator()() const noexcept
{ return _type_id; }

bool ThingType::is_derived_from(FPID inTypeID) const noexcept
{ return _all_base_types.contains(inTypeID); }

std::string ThingType::log() const noexcept
{
    std::string out{std::format("{} > [", _type_id.name())};
    for(FAUTO id : _all_base_types)
        { out += id.name() + ","; }
    out.pop_back();
    out.push_back(']');
    return out;
}
