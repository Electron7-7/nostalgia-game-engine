#ifndef THEATRE_FWD_H
#define THEATRE_FWD_H

class Transform3D;

class VariableRegistry;
extern VariableRegistry* g_pVariableRegistry;

enum class ThingVarType : unsigned short;
struct ThingVar;

struct ThingData;

struct TheatreParser;
extern unsigned int gColumn,
    gLine,
    gBreakOnColumn,
    gBreakOnLine;

struct TheatreData;

#endif // THEATRE_FWD_H
