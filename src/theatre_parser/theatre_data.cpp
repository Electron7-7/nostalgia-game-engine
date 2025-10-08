#include "theatre_data.hpp"
#include "embedded/names.hpp"
#include "things/thing_factory.hpp"
#include "colors.hpp"

const TheatreData TheatreData::Missing;

ID TheatreData::id() const
{ return id_; }

const std::vector<ThingData>& TheatreData::GetData() const
{ return data_; }

static std::map<std::string, ID>
s_EmbeddedDataNameIDLookup =
{
    { Images::Name::Missing,    UniqueIDs::Reserved::i_Missing    },
    { Images::Name::LightDebug, UniqueIDs::Reserved::i_LightDebug },
    { Images::Name::COMP04_5,   UniqueIDs::Reserved::i_COMP04_5   },
    { Images::Name::LolBit,     UniqueIDs::Reserved::i_LolBit     },
    { Models::Name::Error,      UniqueIDs::Reserved::m_Error      },
    { Models::Name::Cube,       UniqueIDs::Reserved::m_Cube       },
    { Models::Name::Ramiel,     UniqueIDs::Reserved::m_Ramiel     },
    { Fonts::Name::Audiowide,   UniqueIDs::Reserved::f_Audiowide  },
    { Fonts::Name::DejaVuSans,  UniqueIDs::Reserved::f_DejaVuSans },
    { Fonts::Name::Verdana,     UniqueIDs::Reserved::f_Verdana    },
};

void TheatreData::UpdateReferences(const std::map<std::string, std::string>& ids)
{
    for(ThingData& data : data_)
    {
        for(ThingVar& variable : data.variables)
        {
            if(variable.type != ThingVar::eReferenceT || variable.type != ThingVar::eReferenceE)
                { continue; }
            if(ids.contains(variable.value))
                { variable.value = ids.at(variable.value); }
            else if(s_EmbeddedDataNameIDLookup.contains(variable.value))
                { variable.value = std::to_string(s_EmbeddedDataNameIDLookup.at(variable.value)); }
        }
    }
}

// QuickSort functions heavily plagiarized from: https://www.w3schools.com/dsa/dsa_algo_quicksort.php
static int s_DataPartition(std::vector<ThingData>& array, int low, int high)
{
    int pivot = ThingFactory::GetPriority(array.at(high).type());
    int i = (low - 1);

    for(int j = low; j < high; ++j)
    {
        if(ThingFactory::GetPriority(array.at(j).type()) <= pivot)
        {
            ++i;
            ThingData temp = array.at(i);
            array.at(i) = array.at(j);
            array.at(j) = temp;
        }
    }

    ThingData temp = array.at(i + 1);
    array.at(i + 1) = array.at(high);
    array.at(high) = temp;
    return (i + 1);
}

static void s_DataQuickSort(std::vector<ThingData>& array, int low, int high)
{
    if(low < high)
    {
        int pivot_index = s_DataPartition(array, low, high);
        s_DataQuickSort(array, low, pivot_index - 1);
        s_DataQuickSort(array, pivot_index + 1, high);
    }
}

void TheatreData::OrderByPriority()
{ s_DataQuickSort(data_, 0, data_.size() - 1); }

SafeStatus TheatreData::AddData(const ThingData& data)
{
    if(!ThingFactory::IsThing(data.type()))
        { return Status::TheatreDataINVALID_TYPE; }
    data_.push_back(data);
    return Status::NO_ERR;
}

std::string TheatreData::formatted() const
{
    std::string resources("");
    std::string things("");

    for(const auto& thing_data : data_)
    {
        if(ThingFactory::IsResource(thing_data.uid) && !thing_data.variables.empty())
            { resources += std::format("\t{} {} = {}", ThingFactory::GetTypeName(thing_data.uid), name, thing_data.variables.at(0).formatted_value()); }
        else
        {
            things += std::format("\t{} {}\n\t{{\n", ThingFactory::GetTypeName(thing_data.uid), name);
            for(const auto& variable : thing_data.variables)
                { things += std::format("\t\t{}\n", variable.formatted_value()); }
            things += std::format("\t}}\n");
        }
    }

    return std::format("@{}#{}\n\nResources\n{{\n{}}}\n\nThings\n{{\n{}}}\n", name, index, resources, things);
}

void TheatreData::clear()
{ data_.clear(); }

void TheatreData::debug_PrintData()
{
    std::println("{}Theatre Data Printout:{}", sty::Bold + fg::Cyan, sty::Reset);
    for(const auto& data : data_)
        { std::println("{}", data.log(true, true)); }
}
