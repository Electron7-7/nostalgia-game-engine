#ifndef RESOURCE_H
#define RESOURCE_H

// Similar to either Godot's `Resource` or `RefCounted` (I haven't decided yet)
class Resource : public Thing
{
public:
    SUPER(Thing)

    virtual void Init() {}
};

template<typename T>
    concept Resource_t = std::derived_from<T, Resource>;

#endif // RESOURCE_H
