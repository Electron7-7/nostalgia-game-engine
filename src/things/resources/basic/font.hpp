#ifndef FONT_H
#define FONT_H

#include "../resource.hpp"
#include "safe_return.hpp"
#include "filesystem/binary_file_data.hpp"

struct Font : public Resource
{
public:
    static Font Missing;

    virtual void UpdateVariables(const data_t&);

    Font();
    Font(const BinaryFileData& Data);

    void LoadFontFile(const std::string& File);
    void LoadFontFile(const BinaryFileData& Data);

    SafeStatus try_CreateFont();

    const BinaryFileData& GetData() const;

private:
    friend class TheatreManager;
    ResourceStatus m_FontDataStatus = ResourceStatus::NOT_PROCESSED;

    BinaryFileData m_Data;
    std::string m_File = "";
};

#endif // FONT_H
