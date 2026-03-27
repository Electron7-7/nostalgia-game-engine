#include "./call_sheet.hpp"
#include "things/thing.hpp" // IWYU pragma: keep

Error CallSheet::Add(ID inNodeID, ID inParentID) noexcept
{ return mTree.add_node(inNodeID, inParentID); }

Error CallSheet::Remove(ID inNodeID) noexcept
{ return mTree.remove_node(inNodeID); }

Error CallSheet::Reparent(ID inNodeID, ID inParentID) noexcept
{ return mTree.set_parent(inNodeID, inParentID); }

IdSet_t CallSheet::Descendants(ID inNodeID) noexcept
{
#pragma message("TODO: I should implement an actual root node instead of doing this")
    if(inNodeID.invalid())
    {
        auto keys{std::views::keys(mTree.all_nodes)};
        return {keys.begin(), keys.end()};
    }
    else if(!mTree.all_nodes.contains(inNodeID))
        { return {}; }
    IdSet_t output{};
    FAUTO children{mTree.all_nodes.at(inNodeID).children};
    for(ID child : children)
    {
        output.insert(child);
        auto sub_children{Descendants(child)};
        output.insert(sub_children.cbegin(), sub_children.cend());
    }
    return output;
}

IdSet_t CallSheet::Ancestors(ID inNodeID) noexcept
{
    if(!mTree.all_nodes.contains(inNodeID))
        { return {}; }
    IdSet_t output{};
    ID parent{mTree.all_nodes.at(inNodeID).parent};
    while(!parent.invalid())
    {
        if(mTree.all_nodes.contains(parent))
        {
            output.insert(parent);
            parent = mTree.all_nodes.at(parent).parent;
        }
        else { parent = {}; }
    }
    return output;
}

Farg<Node> CallSheet::Get(ID inNodeID) const noexcept
{
    static Node error_node{};
#pragma message("TODO: I should implement an actual root node instead of doing this")
    static Node root_node{};
    if(inNodeID.invalid())
    {
        root_node.children.clear();
        for(FAUTO [id, node] : mTree.all_nodes)
        {
            if(node.parent.invalid())
                { root_node.children.insert(id); }
        }
        return root_node;
    }
    else if(auto found_it{mTree.all_nodes.find(inNodeID)}; found_it != mTree.all_nodes.end())
        { return found_it->second; }
    return error_node;
}

bool CallSheet::Has(ID inNodeID) const noexcept
{ return mTree.all_nodes.contains(inNodeID); }

void CallSheet::Clear() noexcept
{ mTree.all_nodes.clear(); }

Farg<Tree> CallSheet::get_tree() const noexcept
{ return mTree; }

static std::string s_GetDebugLog(Farg<Tree> inTree,
    Node inNode,
    Farg<Things_t> inThings = {},
    int indent = 1)
{
    std::string log{};
    std::string indentation{};
    indentation.append(indent, '\t');
    for(ID child : inNode.children)
    {
        std::string name{(inThings.contains(child)) ? inThings.at(child)->name() : ""};
        log += std::format("{}[{}, {}]\n",
            indentation,
            child(),
            name);
        // if(inTree.all_nodes.contains(child))
            // { log += s_GetDebugLog(inTree, inTree.all_nodes.at(child), inThings, ++indent); }
    }
    return log;
}

std::string CallSheet::debug_log(Farg<Things_t> inThings) const noexcept
{
    std::string log{"CallSheet Log:\n"};
    for(FAUTO [id, node] : mTree.all_nodes)
    {
        log += std::format("\tNode [{}, {}]\n", id(),
            (inThings.contains(id)) ? inThings.at(id)->name() : "N/A");
        log += s_GetDebugLog(mTree, node, inThings, 2);
    }
    return log;
}

Error CallSheet::ChangeID(ID inOldNodeID, ID inNewNodeID) noexcept
{
    if(auto found_it{mTree.all_nodes.find(inOldNodeID)}; found_it != mTree.all_nodes.end())
    {
        FAUTO children{found_it->second.children};
        for(ID child : children)
        {
            if(auto found_it_again{mTree.all_nodes.find(child)}; found_it_again != mTree.all_nodes.end())
                { found_it_again->second.parent = inNewNodeID; }
        }
        found_it->second.id = inNewNodeID;
        auto node_handler{mTree.all_nodes.extract(inOldNodeID)};
        node_handler.key() = inNewNodeID;
        mTree.all_nodes.insert(std::move(node_handler));
        return OK;
    }
    return ERR_NOT_FOUND;
}
