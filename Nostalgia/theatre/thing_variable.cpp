using namespace TheatreFile;

std::string ThingVariable::get_log() const noexcept
{
    return std::format("[name: {}, value: {}, type: {}]",
        name,
        value,
        EnumPrettifier::Prettify(type));
}

std::string ThingVariable::get_parsable_string() const noexcept
{
    std::string variable_name{name},
        variable_value{value},
        delim_in{},
        delim_out{};
    if(type == ThingVarType::Child) { variable_name = "Child"; }
    switch(type)
    {
    case ThingVarType::None:
        [[fallthrough]];
    default:
        print_warning("Invalid `ThingVariable` detected -> {}", get_log());
        break;
    case ThingVarType::Parent:
        variable_name = "Parent";
        [[fallthrough]];
    case ThingVarType::Child:
        [[fallthrough]];
    case ThingVarType::ID:
        delim_in  = "<";
        delim_out = ">";
        break;
    case ThingVarType::String:
        delim_in = delim_out = "'";
        break;
    case ThingVarType::Bool:
        [[fallthrough]];
    case ThingVarType::Number:
        delim_in  = "[";
        delim_out = "]";
        break;
    case ThingVarType::Enum:
        delim_in  = "(";
        delim_out = ")";
        break;
    }
    return std::format("    {} = {}{}{}",
        variable_name,
        delim_in,
        variable_value,
        delim_out);
}
