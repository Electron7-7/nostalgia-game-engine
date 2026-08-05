#ifndef NOSTALGIA_THING_ID_H
#define NOSTALGIA_THING_ID_H

// Based on Godot's 'ObjectID'
// https://github.com/godotengine/godot/tree/4cd2d92ae0fe7b2bd9ccab97178daa82911b1d70/core/object/object_id.h

class ThingID
{
private:
    uint64_t _id{0};

public:
    __ALWAYS_INLINE__ bool is_valid() const { return _id != 0; }
    __ALWAYS_INLINE__ bool is_null()  const { return _id == 0; }
    __ALWAYS_INLINE__ operator uint64_t() const { return _id; }
    __ALWAYS_INLINE__ operator int64_t() const { return static_cast<int64_t>(_id); }

    __ALWAYS_INLINE__ bool operator==(Farg<ThingID> inID) const { return _id == inID._id; }
    __ALWAYS_INLINE__ bool operator!=(Farg<ThingID> inID) const { return _id != inID._id; }
    __ALWAYS_INLINE__ bool operator< (Farg<ThingID> inID) const { return _id <  inID._id; }

    __ALWAYS_INLINE__ void operator= (uint64_t inUINT64) const { _id = inUINT64; }
    __ALWAYS_INLINE__ void operator= (int64_t inINT64)   const { _id = inINT64;  }

    __ALWAYS_INLINE__ ThingID() = default;
    __ALWAYS_INLINE__ explicit ThingID(const uint64_t inUINT64) noexcept { _id = inUINT64; }
    __ALWAYS_INLINE__ explicit ThingID(const int64_t inINT64)   noexcept { _id = inINT64;  }
};

#endif // NOSTALGIA_THING_ID_H
