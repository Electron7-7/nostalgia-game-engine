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
};

#endif // RESOURCE_H
