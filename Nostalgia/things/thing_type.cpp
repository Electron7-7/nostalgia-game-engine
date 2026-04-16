ThingType_t::ThingType_t(FPID inTypeID) noexcept:
    _type_id{inTypeID} {}

ThingType_t::ThingType_t() noexcept:
    _type_id{ThingType::Thing},
    _base_type_id{ThingType::Thing},
    _all_base_types{} {}

Farg<ThingType_t::typeids_t> ThingType_t::base_types() const noexcept
{ return _all_base_types; }

PID ThingType_t::base_type() const noexcept
{ return _base_type_id; }

PID ThingType_t::type() const noexcept
{ return _type_id; }

PID ThingType_t::operator()() const noexcept
{ return _type_id; }

bool ThingType_t::is_derived_from(FPID inTypeID) const noexcept
{ return _all_base_types.contains(inTypeID); }

std::string ThingType_t::log() const noexcept
{
    std::string out{std::format("{}", _type_id.name())};
    std::vector<PID> _types{};
    for(FAUTO id : _all_base_types)
        { _types.insert(_types.begin(), id); }
    for(FAUTO id : _types)
        { out += " > " + id.name(); }
    return out;
}
