#include "tree.hpp"

bool Node::invalid() const noexcept
{ return id.invalid(); }

Error Tree::add_node(ID inNodeID, ID inParentID) noexcept
{
    if(all_nodes.contains(inNodeID))
        { return ERR_ALREADY_EXISTS; }
    else if(!inParentID.invalid() and !all_nodes.contains(inParentID))
        { return ERR_NOT_FOUND; }

    all_nodes.emplace(inNodeID, Node{inNodeID, inParentID});
    if(!inParentID.invalid())
        { all_nodes.at(inParentID).children.insert(inNodeID); }
    return OK;
}

Error Tree::remove_node(ID inNodeID) noexcept
{
    if(auto found_it{all_nodes.find(inNodeID)}; found_it != all_nodes.end())
    {
        if(auto found_parent{all_nodes.find(found_it->second.parent)};
            found_parent != all_nodes.end())
                { found_parent->second.children.erase(inNodeID); }
        for(ID child : found_it->second.children)
        {
            if(auto found_child{all_nodes.find(child)}; found_child != all_nodes.end())
                { found_child->second.parent = ID::Invalid; }
        }
        all_nodes.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Tree::set_parent(ID inNodeID, ID inParentID) noexcept
{
    if(!all_nodes.contains(inNodeID)
        or (!inParentID.invalid() and !all_nodes.contains(inParentID)))
            { return ERR_NOT_FOUND; }

    auto& child{all_nodes.at(inNodeID)};
    if(auto found_it{all_nodes.find(child.parent)}; found_it != all_nodes.end())
        { found_it->second.children.erase(inNodeID); }
    if(!inParentID.invalid())
        { all_nodes.at(inParentID).children.insert(inNodeID); }
    child.parent = inParentID;
    return OK;
}
