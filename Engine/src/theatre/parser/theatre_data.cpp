#include "theatre_data.hpp"
#include "theatre/thing_factory.hpp"
#include "core/uid.hpp"
#include "core/printing.hpp"
#include "common/colors.hpp"
#include <map>

const TheatreData TheatreData::Missing;

ID TheatreData::id() const
{ return id_; }

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

void TheatreData::SetupUIDsAndPriorities()
{
    s_DataQuickSort(things_data, 0, things_data.size() - 1);

    std::map<std::string, ID> name_id_map{};
    UID::Clear();
    g_pVariableRegistry->ClearIDs();

    // I wanted to combine these two `for` loops, but it's safer to separate them, since I can't guarantee that
    // referenced things will have defined UIDs even after sorting by priority. Perhaps in the future I can change
    // this, but for now it's fine to have two for loops (even if I personally hate it).
    for(ThingData& data : things_data)
    {
        if(data.uid.invalid())
            { data.uid = UID::Generate(); }
        if(!g_pVariableRegistry->RegisterID(data.name, data.uid[]))
            { print_warning("Duplicate Thing name found: {}", data.name); }
    }
    for(ThingData& data : things_data)
    {
        for(ThingVar& variable : data.variables)
        {
            if(variable.type == ThingVar::Type::Reference)
            {
                if(uint id; g_pVariableRegistry->try_GetID(variable.value, id))
                    { variable.id_or_enum = id; }
            }
            // Note: this uses an inaccurate and bad function in `VariableRegistry` that's only for
            // debugging purposes, as `ThingVar::id_or_enum` isn't used when calling `ThingData::GetVariable`
            // for an enum variable.
            else if(variable.type == ThingVar::Type::Enum)
                { g_pVariableRegistry->try_GetEnum(variable.value, variable.id_or_enum); }
        }
        for(ThingVar& child : data.children)
        {
            if(uint id; g_pVariableRegistry->try_GetID(child.value, id))
                { child.id_or_enum = id; }
        }
    }
}

Error TheatreData::AddData(Farg<ThingData> data)
{
    if(!ThingFactory::IsThing(data.type()))
        { return ERR_INVALID_TYPE; }
    things_data.push_back(data);
    return OK;
}

std::string TheatreData::formatted() const
{
    std::string resources("");
    std::string things("");
    for(const ThingData& thing_data : things_data)
    {
        if(ThingFactory::IsResource(thing_data.type()) && !thing_data.variables.empty())
            { resources += std::format("\t{} {} = {};\n", thing_data.type().name(), thing_data.name, thing_data.variables.at(0).formatted_value()); }
        else
        {
            things += std::format("\t{} {}\n\t{{\n", thing_data.type().name(), thing_data.name);
            for(Farg<ThingVar> variable : thing_data.variables)
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
    print_debug("Theatre Data Printout:", Sty::Bold + Fg::Cyan, Sty::Reset);
    for(const ThingData& thing_data : things_data)
        { println("{}", thing_data.log(true, true)); }
}
