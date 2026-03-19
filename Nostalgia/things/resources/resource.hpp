#ifndef RESOURCE_H
#define RESOURCE_H

// Similar to either Godot's `Resource` or `RefCounted` (I haven't decided yet)
class Resource : public Thing
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData> inData) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    Error Status() const;
    Shared<FileData> Data();

protected:
    Shared<FileData> m_pFileData{MakeShared<FileData>()};
    Error mStatus;

    Error try_LoadFileDataFromVariable(Sarg inPath, Shared<FileData>& outData);
};

template<typename T>
    concept Resource_t = std::derived_from<T, Resource>;

#endif // RESOURCE_H
