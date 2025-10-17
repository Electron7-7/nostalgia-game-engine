#include "theatre_data.hpp"
#include "things/thing_factory.hpp"
#include "colors.hpp" // IWYU pragma: keep

#include <map>

const TheatreData TheatreData::Missing;

ID TheatreData::id() const
{ return id_; }

// QuickSort functions heavily plagiarized from: https://www.w3schools.com/dsa/dsa_algo_quicksort.php
static int s_DataPartition(std::vector<ThingData>& array, int low, int high)
{
    int pivot = g_pThingFactory->GetPriority(array.at(high).type());
    int i = (low - 1);

    for(int j = low; j < high; ++j)
    {
        if(g_pThingFactory->GetPriority(array.at(j).type()) <= pivot)
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

void TheatreData::SetupUIDsAndPriorities()
{
    s_DataQuickSort(things_data, 0, things_data.size() - 1);

    std::map<std::string, ID> name_id_map{UniqueIDs::Reserved::ResourceNameToUIDMap};
    UniqueIDs::Clear();

    // I wanted to combine these two `for` loops, but it's safer to separate them, since I can't guarantee that
    // referenced things will have defined UIDs even after sorting by priority. Perhaps in the future I can change
    // this, but for now it's fine to have two for loops (even if I personally hate it).
    for(ThingData& data : things_data)
        { name_id_map[data.name] = data.uid = UniqueIDs::Generate(); }
    for(ThingData& data : things_data)
    {
        for(ThingVar& variable : data.variables)
        {
            if(variable.type == ThingVar::ePrettyEnum && sPrettyEnumLookup.contains(variable.value))
                { variable.pretty_enum = sPrettyEnumLookup.at(variable.value); }
            else if(variable.type == ThingVar::eReference && name_id_map.contains(variable.value))
                { variable.reference_id = name_id_map.at(variable.value); }
        }
    }
}

SafeStatus TheatreData::AddData(const ThingData& data)
{
    if(!g_pThingFactory->IsThing(data.type()))
        { return Status::TheatreDataINVALID_TYPE; }
    things_data.push_back(data);
    return Status::NO_ERR;
}

std::string TheatreData::formatted() const
{
    std::string resources("");
    std::string things("");
    for(const ThingData& thing_data : things_data)
    {
        if(g_pThingFactory->IsResource(thing_data.type()) && !thing_data.variables.empty())
            { resources += std::format("\t{} {} = {}\n", g_pThingFactory->GetTypeName(thing_data.type()), thing_data.name, thing_data.variables.at(0).formatted_value()); }
        else
        {
            things += std::format("\t{} {}\n\t{{\n", g_pThingFactory->GetTypeName(thing_data.type()), thing_data.name);
            for(const ThingVar& variable : thing_data.variables)
            {
                if(auto format_var = variable.formatted(); !format_var.empty())
                    { things += std::format("\t\t{}\n", format_var); }
            }
            things += std::format("\t}}\n");
        }
    }
    return std::format("@{}#{}\n\nResources\n{{\n{}}}\n\nThings\n{{\n{}}}\n", name, index, resources, things);
}

void TheatreData::clear()
{ things_data.clear(); }

void TheatreData::debug_PrintData()
{
    print_debug("Theatre Data Printout:", sty::Bold + fg::Cyan, sty::Reset);
#ifdef DEBUGGING
    for(const ThingData& thing_data : things_data)
        { println("{}", thing_data.log(true, true)); }
#endif
}
