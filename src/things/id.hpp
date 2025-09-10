#ifndef ID_H
#define ID_H

typedef unsigned int id_t;
constexpr id_t NoID = 0;

struct ID
{
    static id_t GetNewID();
    static void ClearIDs();
    static bool RemoveID(id_t ID);
};

#endif // ID_H
