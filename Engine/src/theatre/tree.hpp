#ifndef TREE_H
#define TREE_H

#include "core/id.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"
#include <map>

struct Tree; // Forward Declaration

struct Node
{
    Node() noexcept;

    bool invalid() const noexcept;

    ID id{},
        parent{};
    Shared<Tree> children{nullptr};
};

struct Tree
{
    std::map<ID, Node> all_nodes{};

    Error add_node(ID inNodeID, ID inParentID = ID::Invalid) noexcept;
    Error remove_node(ID inNodeID) noexcept;
    Error set_parent(ID inNodeID, ID inParentID) noexcept;
};

#endif // TREE_H
