#include "call_sheet.hpp"

Error CallSheet::Add(ID inNodeID, ID inParentID) noexcept
{ return mTree.add_node(inNodeID, inParentID); }

Error CallSheet::Remove(ID inNodeID) noexcept
{ return mTree.remove_node(inNodeID); }

Error CallSheet::Reparent(ID inNodeID, ID inParentID) noexcept
{ return mTree.set_parent(inNodeID, inParentID); }

Farg<Node> CallSheet::Get(ID inNodeID) noexcept
{
    static Node error_node{};
    if(auto found_it{mTree.all_nodes.find(inNodeID)}; found_it != mTree.all_nodes.end())
        { return found_it->second; }
    return error_node;
}

Error CallSheet::Get(ID inNodeID, Node& outNode) noexcept
{
    if(auto found_it{Get(inNodeID)}; !found_it.invalid())
        { outNode = found_it; return OK; }
    return ERR_NOT_FOUND;
}
