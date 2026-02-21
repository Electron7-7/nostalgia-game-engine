BitMask::BitMask(bool inState) noexcept:
    layers_{(inState) ? all_enabled : all_disabled} {}

BitMask::BitMask(const StatusArray& inStatusArray) noexcept:
    BitMask()
{ set(inStatusArray); }

int BitMask::get() const noexcept
{ return layers_; }

void BitMask::set(int inLayers) noexcept
{ layers_ = inLayers; }

void BitMask::set(const StatusArray& inArray) noexcept
{
    for(int i{0}; i < inArray.size(); ++i)
        { set_index(i, inArray[i]); }
}

bool BitMask::contains(int inLayers) const noexcept
{ return layers_ & inLayers; }

bool BitMask::contains(BitMask inLayers) const noexcept
{ return layers_ & inLayers.get(); }

bool BitMask::status(ushort index) const noexcept
{
    int layer{1 << index};
    return (layers_ & layer) == layer;
}

BitMask::StatusArray BitMask::status() const noexcept
{
    StatusArray output{};
    for(int i{0}; i < max; ++i)
        { output[i] = status(i); }
    return output;
}

void BitMask::set_index(ushort index, bool state) noexcept
{
    if(state) { enable_index(index); }
    else { disable_index(index); }
}

void BitMask::enable_index(ushort index) noexcept
{ enable(1 << index); }

void BitMask::disable_index(ushort index) noexcept
{ disable(1 << index); }

bool BitMask::toggle_index(ushort index) noexcept
{
    toggle(1 << index);
    return status(index);
}

void BitMask::set(int inBits, bool inStatus) noexcept
{
    if(inStatus) { enable(inBits); }
    else { disable(inBits); }
}

void BitMask::enable(int inBits) noexcept
{ layers_ |= inBits; }

void BitMask::disable(int inBits) noexcept
{ layers_ &= ~inBits; }

void BitMask::toggle(int inBits) noexcept
{ layers_ ^= inBits; }

std::string BitMask::log(bool noLayersStatus) const noexcept
{
    std::string output{std::format("(count: {})", max)};
    if(!noLayersStatus)
    {
        output += " [";
        for(int i{0}; i < max; ++i)
        {
            output += std::format("{}{}",
                status(i),
                (i < max - 1)
                    ? ", "
                    : "]");
        }
    }
    return output;
}
