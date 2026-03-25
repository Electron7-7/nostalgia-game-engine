#ifndef RESOURCE_DATABASE_H
#define RESOURCE_DATABASE_H

#define USE_NAME_OR_UID(FUNCTION) \
    FUNCTION(ID inUID); \
    FUNCTION(Sarg inName);

namespace ResourceDatabase
{
    ID Register(TheatreFile::ThingData inData);
    ID Register(Shared<Resource> inResource);

    void DestroyAll(); // The nuclear option (should only really be used by the render manager when it shuts down)

    IdVec_t GetUIDs();
    Sarg GetName(ID inUID);
    ID GetUID(Sarg inName);
    bool DerivedFrom(ID inUID, FPID inType);
    bool DerivedFrom(Sarg inName, FPID inType);

    USE_NAME_OR_UID(FPID TypeOf)
    USE_NAME_OR_UID(Error Destroy)
    USE_NAME_OR_UID(bool Contains)
    USE_NAME_OR_UID(Shared<Resource> GetResource)

    template<Resource_t T, NameOrUID_t U>
        Shared<T> GetResource(Farg<U> inNameOrUID)
        {
            if(auto resource{DCast<T>(GetResource(inNameOrUID))})
                { return resource; }
            return MakeShared<T>();
        }
}

#undef USE_NAME_OR_UID

#endif // RESOURCE_DATABASE_H
