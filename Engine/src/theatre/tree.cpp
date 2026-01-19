#include "tree.hpp"

Node::Node() noexcept:
    id{},
    parent{},
    children{MakeUnique<Tree>()} {}

bool Node::invalid() const noexcept
{ return id.invalid(); }

Error Tree::add_node(ID inNodeID, ID inParentID) noexcept
{
    if(all_nodes.contains(inNodeID))
        { return ERR_ALREADY_EXISTS; }
    else if(!inParentID.invalid() and !all_nodes.contains(inNodeID))
        { return ERR_NOT_FOUND; }

    all_nodes.emplace(inNodeID, Node{});
    all_nodes[inNodeID].id = inNodeID;
    all_nodes[inNodeID].parent = inParentID;
    if(!inParentID.invalid())
        { all_nodes.at(inParentID).children->add_node(inNodeID); }
    return OK;
}

Error Tree::remove_node(ID inNodeID) noexcept
{
    if(auto found_it{all_nodes.find(inNodeID)}; found_it != all_nodes.end())
    {
        if(auto found_parent{all_nodes.find(found_it->second.parent)};
            found_parent != all_nodes.end())
                { found_parent->second.children->remove_node(inNodeID); }
        all_nodes.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Tree::set_parent(ID inNodeID, ID inParentID) noexcept
{
    if(inNodeID.invalid() or inParentID.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{all_nodes.find(inNodeID)}; found_it != all_nodes.end())
    {
        if(auto found_it2{all_nodes.find(inParentID)}; found_it2 != all_nodes.end())
        {
            if(!found_it->second.parent.invalid())
                { all_nodes.at(found_it->second.parent).children->remove_node(inNodeID); }
            found_it->second.parent = inParentID;
            found_it2->second.children->add_node(inNodeID);
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}
