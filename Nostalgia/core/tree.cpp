#include "./tree.hpp"

bool Node::invalid() const noexcept
{ return id.invalid(); }

Error Tree::add_node(ID inNodeID, ID inParentID) noexcept
{
    if(nodes.contains(inNodeID))
        { return ERR_ALREADY_EXISTS; }
    else if(!inParentID.invalid())
    {
        if(!nodes.contains(inParentID))
            { return ERR_NOT_FOUND; }
        nodes.at(inParentID).children.insert(inNodeID);
    }
    nodes.emplace(inNodeID, Node{inNodeID, inParentID});
    return OK;
}

Error Tree::remove_node(ID inNodeID) noexcept
{
    if(auto found_it{nodes.find(inNodeID)}; found_it != nodes.end())
    {
        if(auto found_parent{nodes.find(found_it->second.parent)};
            found_parent != nodes.end())
                { found_parent->second.children.erase(inNodeID); }
        for(ID child : found_it->second.children)
        {
            if(auto found_child{nodes.find(child)}; found_child != nodes.end())
                { found_child->second.parent = ID::Invalid; }
        }
        nodes.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Tree::set_parent(ID inNodeID, ID inParentID) noexcept
{
    if(!nodes.contains(inNodeID)
        or (!inParentID.invalid() and !nodes.contains(inParentID)))
            { return ERR_NOT_FOUND; }

    auto& child{nodes.at(inNodeID)};
    if(auto found_it{nodes.find(child.parent)}; found_it != nodes.end())
        { found_it->second.children.erase(inNodeID); }
    if(!inParentID.invalid())
        { nodes.at(inParentID).children.insert(inNodeID); }
    child.parent = inParentID;
    return OK;
}

IdSet_t Tree::get_descendants(ID inNodeID) noexcept
{
#pragma message("TODO: I should implement an actual root node instead of doing this")
    if(inNodeID.invalid())
    {
        auto keys{std::views::keys(nodes)};
        return {keys.begin(), keys.end()};
    }
    else if(!nodes.contains(inNodeID))
        { return {}; }
    IdSet_t output{};
    FAUTO children{nodes.at(inNodeID).children};
    for(ID child : children)
    {
        output.insert(child);
        auto sub_children{get_descendants(child)};
        output.insert(sub_children.cbegin(), sub_children.cend());
    }
    return output;
}

IdSet_t Tree::get_ancestors(ID inNodeID) noexcept
{
    if(!nodes.contains(inNodeID))
        { return {}; }
    IdSet_t output{};
    ID parent{nodes.at(inNodeID).parent};
    while(not parent.invalid())
    {
        if(nodes.contains(parent))
        {
            output.insert(parent);
            parent = nodes.at(parent).parent;
        }
        else
            { parent = {}; }
    }
    return output;
}

Farg<Node> Tree::get_node(ID inNodeID) const noexcept
{
    static Node error_node{};
#pragma message("TODO: I should implement an actual root node instead of doing this")
    static Node root_node{};
    if(inNodeID.invalid())
    {
        for(FAUTO [id, node] : nodes)
        {
            if(node.parent.invalid())
                { root_node.children.insert(id); }
        }
        return root_node;
    }
    else if(auto found_it{nodes.find(inNodeID)}; found_it != nodes.end())
        { return found_it->second; }
    return error_node;
}

bool Tree::has_node(ID inNodeID) const noexcept
{ return nodes.contains(inNodeID); }

void Tree::clear() noexcept
{ nodes.clear(); }
