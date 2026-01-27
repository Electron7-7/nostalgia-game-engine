#ifndef CALL_SHEET_H
#define CALL_SHEET_H

#include "theatre/tree.hpp"

using Things_t = std::unordered_map<ID, Shared<Thing>>;

class CallSheet
{
public:
    Error Add(ID inNodeID, ID inParentID = ID::Invalid) noexcept;
    Error Remove(ID inNodeID) noexcept;
    Error Reparent(ID inNodeID, ID inParentID) noexcept;
    IdSet_t Descendants(ID inNodeID) noexcept;
    IdSet_t Ancestors(ID inNodeID) noexcept;
    Farg<Node> Get(ID inNodeID) const noexcept;
    bool Has(ID inNodeID) const noexcept;
    void Clear() noexcept;

    Farg<Tree> get_tree() const noexcept;
    std::string debug_log(Farg<Things_t>) const noexcept;

private:
    Tree mTree{};
};

#endif // CCALL_SHEET_H
