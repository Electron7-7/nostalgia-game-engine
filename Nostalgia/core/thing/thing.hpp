#ifndef NOSTALGIA_THING_H
#define NOSTALGIA_THING_H

// Based on Godot's 'Object' class
// https://github.com/godotengine/godot/tree/4cd2d92ae0fe7b2bd9ccab97178daa82911b1d70/core/object/object.h

#define THING_CLASS_HIDDEN(inClass, inInherits) \
public: \
    using type  = inClass; \
    using super = inInherits; \
    static _FORCE_INLINE_ void* GetClassPointer() \
    { \
        static int _pointer{}; \
        return &_pointer; \
    } \
    virtual bool IsClassPointer(void* inPointer) const override \
    { return (inPointer == GetClassPointer()) || inInherits::GetClassPointer(inPointer); } \

#define THING_CLASS

class Thing
{
public:
    using type = Thing;
};

#endif // NOSTALGIA_THING_H
