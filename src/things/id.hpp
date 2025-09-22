#ifndef ID_H
#define ID_H

typedef unsigned int id_t;

struct ID
{
    static constexpr id_t None = 0;
    static constexpr id_t EndOfReservedIDs = 10;

    static id_t GetNewID();
    static void ClearIDs();
    static bool AddID(id_t ID);
    static bool RemoveID(id_t ID);
    static bool Contains(id_t ID);
};

#endif // ID_H
