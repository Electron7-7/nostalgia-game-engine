#ifndef RESOURCE_H
#define RESOURCE_H

#include <Nostalgia/things/thing.hpp>

// Similar to either Godot's `Resource` or `RefCounted` (I haven't decided yet)
class Resource : public Thing
{
public:
	SET_SUPER(Thing)
	SET_TYPEID(ThingType::Resource)

private:
    void Update() final {}
    void Tick() final {}
    void Input(InputEvent*) final {}
};

template<typename T>
    concept Resource_t = std::derived_from<T, Resource>;

#endif // RESOURCE_H
