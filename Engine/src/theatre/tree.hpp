#ifndef TREE_H
#define TREE_H


struct Tree; // Forward Declaration

struct Node
{
    bool invalid() const noexcept;

    ID id{};
    ID parent{};
    IdSet_t children{};
};

struct Tree
{
    std::unordered_map<ID, Node> all_nodes{};

    Error add_node(ID inNodeID, ID inParentID = ID::Invalid) noexcept;
    Error remove_node(ID inNodeID) noexcept;
    Error set_parent(ID inNodeID, ID inParentID) noexcept;
};

#endif // TREE_H
