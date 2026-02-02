#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

namespace TheatreFile
{
    struct TheatreData
    {
        std::string name{"Untitled Theatre"};
        uint index{ID::Invalid};
        std::vector<ThingData> data{};

        template<typename... _Args>
            auto emplace_back(_Args&&... __args) noexcept
            { return data.emplace_back(std::forward<_Args>(__args)...); }

        void push_back(Farg<ThingData> inData) noexcept { data.push_back(inData); }
        void clear() noexcept { data.clear(); }

        auto begin()        noexcept { return data.begin();  }
        auto end()          noexcept { return data.end();    }
        auto cbegin() const noexcept { return data.cbegin(); }
        auto cend()   const noexcept { return data.cend();   }
    };
}

#endif // THEATRE_DATA_H
