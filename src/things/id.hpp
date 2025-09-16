#ifndef ID_H
#define ID_H

typedef unsigned int id_t;

struct ID
{
    static constexpr id_t None = 0;

    static id_t GetNewID();
    static void ClearIDs();
    static bool RemoveID(id_t ID);
};

#endif // ID_H
