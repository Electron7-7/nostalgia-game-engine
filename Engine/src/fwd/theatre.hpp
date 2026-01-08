#ifndef THEATRE_FWD_H
#define THEATRE_FWD_H

enum class ThingVarType : unsigned short;

class VariableRegistry;

struct ThingVar;
struct ThingData;
struct TheatreParser;
struct TheatreData;

extern unsigned int gColumn,
    gLine,
    gBreakOnColumn,
    gBreakOnLine;

extern VariableRegistry* g_pVariableRegistry;

#endif // THEATRE_FWD_H
