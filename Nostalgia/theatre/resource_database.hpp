#ifndef RESOURCE_DATABASE_H
#define RESOURCE_DATABASE_H

#define USE_NAME_OR_UID(FUNCTION) \
    static FUNCTION(ID inUID); \
    static FUNCTION(Sarg inName);

class ResourceDatabase
{
public:
    static ID Create(FPID inType, Sarg inName);
    static ID Register(Shared<Resource> inResource, Sarg inNameOverride = "");

    static void DestroyAll(); // The nuclear option (should only really be used when shutting down the entire app)

    static IdVec_t GetUIDs();
    static Sarg GetName(ID inUID);
    static ID GetUID(Sarg inName);
    static bool DerivedFrom(ID inUID, FPID inType);
    static bool DerivedFrom(Sarg inName, FPID inType);
    static Error SetName(ID inUID, Sarg inNewName);
    static Error SetName(Sarg inOldName, Sarg inNewName);

    USE_NAME_OR_UID(FPID TypeOf)
    USE_NAME_OR_UID(Error Destroy)
    USE_NAME_OR_UID(bool Contains)
    USE_NAME_OR_UID(Shared<Resource> GetResource)

    template<Resource_t T, NameOrUID_t U>
        static Shared<T> GetResource(Farg<U> inNameOrUID)
        {
            if(auto resource{DCast<T>(GetResource(inNameOrUID))})
                { return resource; }
            return MakeShared<T>();
        }
};

#undef USE_NAME_OR_UID

#endif // RESOURCE_DATABASE_H
