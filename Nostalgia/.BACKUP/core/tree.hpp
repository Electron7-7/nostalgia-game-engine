#ifndef TREE_H
#define TREE_H

#define LOCK LockGuard<RMutex> _lock{_mutex};

template<ID_or_PID T>
    struct Node
    {
        bool invalid() const noexcept
        { return id.invalid(); }

        T id{};
        T parent{};
        IdSet<T> children{};
    };

template<ID_or_PID T>
    struct Tree
    {
    private:
        mutable RMutex _mutex{};
    public:
        IdMap<T, Node<T>> nodes{};
        IdSet<T>     root_nodes{};

        Error add_node(Farg<T> inNodeID, Farg<T> inParentID = T{}) noexcept
        {
            LOCK
            if(nodes.contains(inNodeID))
                { return ERR_ALREADY_EXISTS; }
            else if(!inParentID.invalid())
            {
                if(!nodes.contains(inParentID))
                    { return ERR_NOT_FOUND; }
                nodes.at(inParentID).children.insert(inNodeID);
            }
            else
                { root_nodes.insert(inNodeID); }
            nodes.emplace(inNodeID, Node{inNodeID, inParentID});
            return OK;
        }

        Error remove_node(Farg<T> inNodeID) noexcept
        {
            LOCK
            if(auto found_it{nodes.find(inNodeID)}; found_it != nodes.end())
            {
                if(auto found_parent{nodes.find(found_it->second.parent)};
                    found_parent != nodes.end())
                        { found_parent->second.children.erase(inNodeID); }
                for(FAUTO child : found_it->second.children)
                {
                    if(auto found_child{nodes.find(child)}; found_child != nodes.end())
                        { found_child->second.parent = T{}; }
                }
                nodes.erase(found_it);
                root_nodes.erase(inNodeID);
                return OK;
            }
            return ERR_NOT_FOUND;
        }

        Error set_parent(Farg<T> inNodeID, Farg<T> inParentID) noexcept
        {
            LOCK
            if(!nodes.contains(inNodeID)
                or (!inParentID.invalid() and !nodes.contains(inParentID)))
                    { return ERR_NOT_FOUND; }
            auto& child{nodes.at(inNodeID)};
            if(auto found_it{nodes.find(child.parent)}; found_it != nodes.end())
                { found_it->second.children.erase(inNodeID); }
            if(!inParentID.invalid())
            {
                nodes.at(inParentID).children.insert(inNodeID);
                root_nodes.erase(inNodeID);
            }
            else
                { root_nodes.insert(inNodeID); }
            child.parent = inParentID;
            return OK;
        }

        IdSet<T> get_descendants(Farg<T> inNodeID) const noexcept
        {
            LOCK
// TODO: I should implement an actual root node instead of doing this
            if(inNodeID.invalid())
            {
                auto keys{std::views::keys(nodes)};
                return {keys.begin(), keys.end()};
            }
            else if(!nodes.contains(inNodeID))
                { return {}; }
            IdSet<T> output{};
            auto iter{output.begin()};
            FAUTO children{nodes.at(inNodeID).children};
            for(FAUTO child : children)
            {
                iter = output.emplace_hint(iter, child);
                IdSet<T> sub_children{get_descendants(child)};
                for(auto second_iter{sub_children.begin()}; second_iter != sub_children.end(); ++second_iter)
                    { iter = output.emplace_hint(iter, *second_iter); }
            }
            return output;
        }

        IdSet<T> get_ancestors(Farg<T> inNodeID) const noexcept
        {
            LOCK
            if(!nodes.contains(inNodeID))
                { return {}; }
            IdSet<T> output{};
            auto iter{output.begin()};
            T parent{nodes.at(inNodeID).parent};
            while(not parent.invalid())
            {
                if(nodes.contains(parent))
                {
                    iter = output.emplace_hint(iter, parent);
                    parent = nodes.at(parent).parent;
                    continue;
                }
                parent = {};
            }
            return output;
        }

        std::vector<T> get_all_nodes() const noexcept
        {
            LOCK
            auto keys{std::views::keys(nodes)};
            return {keys.begin(), keys.end()};
        }

        Farg<Node<T>> get_node(Farg<T> inNodeID) const noexcept
        {
            LOCK
            static Node<T> error_node{};
// TODO: I should implement an actual root node instead of doing this
            static Node<T> root_node{};
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

        bool has_node(Farg<T> inNodeID) const noexcept
        {
            LOCK
            return nodes.contains(inNodeID);
        }

        void clear() noexcept
        {
            LOCK
            nodes.clear();
        }
    };

#undef LOCK
#endif // TREE_H
