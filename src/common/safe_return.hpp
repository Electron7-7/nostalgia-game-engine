#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

#include "printing.hpp"

namespace
{
    typedef unsigned int __statusId_t;
    constexpr __statusId_t NO_ERR_STATUS = 0;
}

struct SafeStatus
{
public:
    SafeStatus(): m_Status(NO_ERR_STATUS), m_Printout("NO_ERR") {}
    SafeStatus(const SafeStatus& CopyFrom): m_Status(CopyFrom.m_Status), m_Printout(CopyFrom.m_Printout) {}

    const __statusId_t Status() const { return m_Status; }
    const char* Printout() const { return m_Printout; }

    constexpr bool operator==(const SafeStatus& CompareTo) const { return (m_Status == CompareTo.m_Status); }
    constexpr bool operator!=(const SafeStatus& CompareTo) const { return (m_Status != CompareTo.m_Status); }
    constexpr bool operator< (const SafeStatus& CompareTo) const { return (m_Status <  CompareTo.m_Status); }
    constexpr bool operator> (const SafeStatus& CompareTo) const { return (m_Status >  CompareTo.m_Status); }
    constexpr bool operator<=(const SafeStatus& CompareTo) const { return (m_Status <= CompareTo.m_Status); }
    constexpr bool operator>=(const SafeStatus& CompareTo) const { return (m_Status >= CompareTo.m_Status); }

    constexpr operator __statusId_t() const { return m_Status; }

    static constexpr bool Check(const SafeStatus& Status)
    { return(Status.Status() == NO_ERR_STATUS); }

    static constexpr bool PrintCheck(const SafeStatus& Status)
    {
        if(Status.Status() != NO_ERR_STATUS)
        {
            PRINT_ERROR("{}", Status.Printout());
            return false;
        }

        return true;
    }

private:
    __statusId_t m_Status = NO_ERR_STATUS;
    const char* m_Printout  = "NO_ERR?";

    friend struct Status;
    constexpr SafeStatus(const __statusId_t Status, const char* Printout): m_Status(Status), m_Printout(Printout) {}
};

struct Status
{
    inline static SafeStatus NO_ERR;
    inline static SafeStatus ERROR_GENERIC                          = SafeStatus( 0x1,  "ERROR_GENERIC"                          );
    inline static SafeStatus ERROR_ALREADY_ACTIVE                   = SafeStatus( 0x2,  "ERROR_ALREADY_ACTIVE"                   );
    inline static SafeStatus ERROR_INVALID_KEY_ID                   = SafeStatus( 0x3,  "ERROR_INVALID_KEY_ID"                   );
    inline static SafeStatus ERROR_EXCEPTION_CAUGHT                 = SafeStatus( 0x4,  "ERROR_EXCEPTION_CAUGHT"                 );
    inline static SafeStatus WindowingBackendWINDOW_CREATION_FAILED = SafeStatus( 0x5,  "WindowingBackendWINDOW_CREATION_FAILED" );
    inline static SafeStatus WindowingBackendGRAPHICS_INIT_FAILED   = SafeStatus( 0x6,  "WindowingBackendGRAPHICS_INIT_FAILED"   );
    inline static SafeStatus EventQueueNOT_ENABLED                  = SafeStatus( 0x7,  "EventQueueNOT_ENABLED"                  );
    inline static SafeStatus EventQueueNOT_PROCESSING_EVENTS        = SafeStatus( 0x8,  "EventQueueNOT_PROCESSING_EVENTS"        );
    inline static SafeStatus EventQueueEMPTY                        = SafeStatus( 0x9,  "EventQueueEMPTY"                        );
    inline static SafeStatus InputManagerKEY_NOT_FOUND              = SafeStatus( 0xA,  "InputManagerKEY_NOT_FOUND"              );
    inline static SafeStatus InputManagerKEY_IS_LOCKED              = SafeStatus( 0xB,  "InputManagerKEY_IS_LOCKED"              );
    inline static SafeStatus CommandLineINVALID_COMMAND             = SafeStatus( 0xC,  "CommandLineINVALID_COMMAND"             );
    inline static SafeStatus KeyBindsKEY_HAS_NO_BINDS               = SafeStatus( 0xD,  "KeyBindsKEY_HAS_NO_BINDS"               );
    inline static SafeStatus SettingsINVALID_SETTING_NAME           = SafeStatus( 0xE,  "SettingsINVALID_SETTING_NAME"           );
    inline static SafeStatus DataTypeINVALID_VARIABLE_NAME          = SafeStatus( 0xF,  "DataTypeINVALID_VARIABLE_NAME"          );
    inline static SafeStatus DataTypeEMPTY_VARIABLE                 = SafeStatus( 0x10, "DataTypeEMPTY_VARIABLE"                 );
    inline static SafeStatus TheatreParserFILE_DOES_NOT_EXIST       = SafeStatus( 0x11, "TheatreParserFILE_DOES_NOT_EXIST"       );
    inline static SafeStatus TheatreParserWRONG_FILE_EXTENSION      = SafeStatus( 0x12, "TheatreParserWRONG_FILE_EXTENSION"      );
    inline static SafeStatus TheatreParserNO_THEATRE_LOADED         = SafeStatus( 0x13, "TheatreParserNO_THEATRE_LOADED"         );
    inline static SafeStatus TheatreParserMISSING_THEATRE_NAME      = SafeStatus( 0x14, "TheatreParserMISSING_THEATRE_NAME"      );
    inline static SafeStatus TheatreParserMISSING_THEATRE_INDEX     = SafeStatus( 0x15, "TheatreParserMISSING_THEATRE_INDEX"     );
    inline static SafeStatus TheatreDataINVALID_TYPE                = SafeStatus( 0x16, "TheatreDataINVALID_TYPE"                );
    inline static SafeStatus MeshUNKNOWN_FILETYPE                   = SafeStatus( 0x17, "MeshUNKNOWN_FILETYPE"                   );
    inline static SafeStatus MeshFAILED_TO_LOAD_OBJ                 = SafeStatus( 0x18, "MeshFAILED_TO_LOAD_OBJ"                 );
    inline static SafeStatus FilesystemINVALID_PATH                 = SafeStatus( 0x19, "FilesystemINVALID_PATH"                 );
    inline static SafeStatus FilesystemFILE_READ_ERROR              = SafeStatus( 0x20, "FilesystemFILE_READ_ERROR"              );
    inline static SafeStatus TextureIMAGE_FILE_FAILED_TO_LOAD       = SafeStatus( 0x21, "TextureIMAGE_FILE_FAILED_TO_LOAD"       );
    inline static SafeStatus EngineReferenceINVALID_REFERENCE       = SafeStatus( 0x22, "EngineReferenceINVALID_REFERENCE"       );
};

template<typename T>
struct SafeReturn
{
public:
    SafeReturn(T Data, SafeStatus ReturnStatus = Status::NO_ERR)
    : m_Data(Data), m_Status(ReturnStatus)
    {}

    SafeReturn(const SafeReturn& Copy)
    : m_Data(Copy.m_Data), m_Status(Copy.m_Status)
    {}

    const T& Data() const { return m_Data; }
    SafeStatus Status() const { return m_Status; }

private:
    T m_Data;
    SafeStatus m_Status = Status::NO_ERR;
};

#endif // SAFE_RETURN_H
