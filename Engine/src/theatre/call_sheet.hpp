#ifndef CALL_SHEET_H
#define CALL_SHEET_H

#include "theatre/tree.hpp"

class CallSheet
{
public:
    Error Add(ID inNodeID, ID inParentID = ID::Invalid) noexcept;
    Error Remove(ID inNodeID) noexcept;
    Error Reparent(ID inNodeID, ID inParentID) noexcept;
    Farg<Node> Get(ID inNodeID) noexcept;
    Error Get(ID inNodeID, Node& outNode) noexcept;

private:
    Tree mTree{};
};

#endif // CCALL_SHEET_H
