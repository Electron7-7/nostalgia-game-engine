#ifndef STATUS_MAP_H
#define STATUS_MAP_H


template<typename T, ushort Amount, typename Hasher = std::hash<T>>
struct StatusMap
{
public:
    constexpr StatusMap() = default;

    constexpr operator bool() const
    { return all_successful(); }

    const ushort amount{Amount};

    constexpr ushort amount_failed() const
    { return failed_; }

    constexpr ushort amount_succeeded() const
    { return (amount - failed_); }

    constexpr bool has(const T& Item) const
    {
        size_t hash = hasher_(Item);
        for(ushort i = 0 ; i < Amount ; ++i)
            { if(hashes_[i] == hash) { return true; } }
        return false;
    }

    constexpr bool any_failed() const
    { return failed_; }

    constexpr bool any_successful() const
    { return !(Amount - failed_); }

    constexpr bool all_failed() const
    { return !any_successful(); }

    constexpr bool all_successful() const
    { return !any_failed(); }

    constexpr bool successful(const T& Item) const
    {
        size_t hash = hasher_(Item);
        for(ushort i = 0 ; i < Amount ; ++i)
        {
            if(hashes_[i] == hash)
                { return status_[i]; }
        }
        return false;
    }

    constexpr bool failed(const T& Item) const
    { return !successful(Item); }

    constexpr bool add(const T& Item, bool Successful)
    {
        if(items_amount >= Amount)
            { return false; }
        else if(!Successful && failed_ < Amount) // the `failed < Amount` check is unnecessary, but better safe than sorry!
            { ++failed_; }
        items_ [items_amount] = Item;
        status_[items_amount] = Successful;
        hashes_[items_amount] = hasher_(Item);
        log_padding_set       = false;
        return ++items_amount; // Cheeky way of using the prefix `++` while saving room
    }

    constexpr bool set(const T& Item, bool Successful)
    {
        size_t hash = hasher_(Item);
        for(ushort i = 0 ; i < Amount ; ++i)
        {
            if(hashes_[i] == hash)
            {
                if(failed_ > 0 && !Successful && !status_[i])
                    { --failed_; }
                status_[i] = Successful;
                return !(log_padding_set = false);
            }
        }
        return false;
    }

    // Easily the most expensive method in `StatusMap` due to my insistence on text padding...
    std::string get_log(const std::string& Header, bool IncludeHashes = false)
    {
        check_and_set_log_padding();
        std::string output{std::format("StatusMap {}:\n", Header)};
        for(ushort i = 0 ; i < items_amount ; ++i)
        {
            output += std::format("\tValue: {:{}}{} Failed: {:5}\n",
                std::format("'{}',", items_[i]),
                log_item_padding,
                (IncludeHashes) ? std::format(" Value Hash: {:020},", hashes_[i]) : "",
                !status_[i]);
        }
        return output;
    }

    // Uses `get_log`, easily the most expensive method in `StatusMap` due to my insistence on text padding...
    void print_log(const std::string& Header, bool IncludeHashes = false)
    { std::println("{}", get_log(Header, IncludeHashes)); }

private:
    T items_[Amount]{};
    size_t hashes_[Amount]{};
    bool status_[Amount]{};
    ushort failed_{0};

    std::hash<T> hasher_{};
    ushort items_amount{0};
    bool   log_padding_set{false};
    ushort log_item_padding{0};

    constexpr void check_and_set_log_padding()
    {
        if(log_padding_set)
            { return; }
        for(ushort i = 0 ; i < items_amount ; ++i)
        {
            size_t item_size{std::string{std::format("{}", items_[i])}.size()};
            if(item_size > log_item_padding)
                { log_item_padding = item_size + 3; }
        }
        log_padding_set = true;
    }
};

#endif // STATUS_MAP_H
