#ifndef TREE_H
#define TREE_H

struct Node
{
    bool invalid() const noexcept;

    ID id{};
    ID parent{};
    IdSet_t children{};
};

struct Tree
{
    std::unordered_map<ID, Node> nodes{};

    Error add_node(ID inNodeID, ID inParentID = ID::Invalid) noexcept;
    Error remove_node(ID inNodeID) noexcept;
    Error set_parent(ID inNodeID, ID inParentID) noexcept;
    IdSet_t get_descendants(ID inNodeID) noexcept;
    IdSet_t get_ancestors(ID inNodeID) noexcept;
    Farg<Node> get_node(ID inNodeID) const noexcept;
    bool has_node(ID inNodeID) const noexcept;
    void clear() noexcept;

    std::string debug_log() const noexcept;
};

#endif // TREE_H
