#ifndef TREE_H
#define TREE_H

template<typename T> requires std::same_as<T, ID> or std::same_as<T, PID>
    struct Node
    {
        bool invalid() const noexcept
        { return id.invalid(); }

        T id{};
        T parent{};
        std::set<T> children{};
    };

template<typename T> requires std::same_as<T, ID> or std::same_as<T, PID>
    struct Tree
    {
        std::unordered_map<T, Node<T>> nodes{};

        Error add_node(Farg<T> inNodeID, Farg<T> inParentID = T{}) noexcept
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

        Error remove_node(Farg<T> inNodeID) noexcept
        {
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
                return OK;
            }
            return ERR_NOT_FOUND;
        }

        Error set_parent(Farg<T> inNodeID, Farg<T> inParentID) noexcept
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

        std::set<T> get_descendants(Farg<T> inNodeID) const noexcept
        {
// TODO: I should implement an actual root node instead of doing this
            if(inNodeID.invalid())
            {
                auto keys{std::views::keys(nodes)};
                return {keys.begin(), keys.end()};
            }
            else if(!nodes.contains(inNodeID))
                { return {}; }
            std::set<T> output{};
            FAUTO children{nodes.at(inNodeID).children};
            for(FAUTO child : children)
            {
                output.insert(child);
                auto sub_children{get_descendants(child)};
                output.insert(sub_children.cbegin(), sub_children.cend());
            }
            return output;
        }

        std::set<T> get_ancestors(Farg<T> inNodeID) const noexcept
        {
            if(!nodes.contains(inNodeID))
                { return {}; }
            std::set<T> output{};
            T parent{nodes.at(inNodeID).parent};
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

        std::vector<T> get_all_nodes() const noexcept
        {
            auto keys{std::views::keys(nodes)};
            return {keys.begin(), keys.end()};
        }

        Farg<Node<T>> get_node(Farg<T> inNodeID) const noexcept
        {
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
        { return nodes.contains(inNodeID); }

        void clear() noexcept
        { nodes.clear(); }
    };

#endif // TREE_H
