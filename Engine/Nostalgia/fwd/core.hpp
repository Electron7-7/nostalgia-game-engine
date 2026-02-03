#ifndef CORE_FWD_H
#define CORE_FWD_H

enum Error : int;

struct ID;
struct PID;
struct ANSI_Sequence;
struct MessageLabel;

extern MessageLabel ErrorLabel,
    WarningLabel,
    DebugLabel,
    JoltLabel,
    FunctionLabel,
    ConstructorLabel,
    DestructorLabel;
extern MessageLabel* __all_labels_for_debugging[7];

class EnumPrettifier;
namespace EnumSet
{
    extern constinit const char* Global;
    extern constinit const char* PhysicsBodyShape;
    extern constinit const char* PhysicsBodyMotion;
}

extern constinit const unsigned int MaxBitMaskPlaces;
struct BitMask;

#endif // CORE_FWD_H
