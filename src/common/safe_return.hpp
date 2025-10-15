#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

#include "printing.hpp"

namespace
{
    typedef unsigned int __statusId_t;
    constexpr __statusId_t NO_ERR_STATUS = 0x0;
}

struct SafeStatus
{
public:
    SafeStatus()
    : m_Status(NO_ERR_STATUS), m_Printout("NO_ERR")
    {}

    SafeStatus(const SafeStatus& CopyFrom)
    : m_Status(CopyFrom.m_Status), m_Printout(CopyFrom.m_Printout)
    {}

    const __statusId_t Status() const { return m_Status; }
    const char* Printout() const { return m_Printout; }

    constexpr bool operator==(const SafeStatus& CompareTo) const { return (m_Status == CompareTo.m_Status); }
    constexpr bool operator!=(const SafeStatus& CompareTo) const { return (m_Status != CompareTo.m_Status); }
    constexpr bool operator< (const SafeStatus& CompareTo) const { return (m_Status <  CompareTo.m_Status); }
    constexpr bool operator> (const SafeStatus& CompareTo) const { return (m_Status >  CompareTo.m_Status); }
    constexpr bool operator<=(const SafeStatus& CompareTo) const { return (m_Status <= CompareTo.m_Status); }
    constexpr bool operator>=(const SafeStatus& CompareTo) const { return (m_Status >= CompareTo.m_Status); }

    constexpr operator bool() const
    { return m_Status == NO_ERR_STATUS; }

    static constexpr bool Check(const SafeStatus& Status) { return(Status.Status() == NO_ERR_STATUS); }

    static constexpr bool PrintCheck(const SafeStatus& Status, const char* Prefix = "SafeStatus::PrintCheck")
    {
        if(Status.Status() != NO_ERR_STATUS)
        {
            print_error("{} returned '{}'", Prefix, Status.Printout());
            return false;
        }
        return true;
    }

private:
    __statusId_t m_Status = NO_ERR_STATUS;
    const char* m_Printout  = "NO_ERR";

    friend struct Status;
    constexpr SafeStatus(const __statusId_t Status, const char* Printout)
    : m_Status(Status), m_Printout(Printout)
    {}
};

struct Status
{
    inline static SafeStatus NO_ERR;
    inline static SafeStatus ERROR_GENERIC                          = SafeStatus( 0x01, "ERROR_GENERIC"                          );
    inline static SafeStatus ERROR_ALREADY_ACTIVE                   = SafeStatus( 0x02, "ERROR_ALREADY_ACTIVE"                   );
    inline static SafeStatus ERROR_INVALID_ID                       = SafeStatus( 0x03, "ERROR_INVALID_ID"                       );
    inline static SafeStatus ERROR_INVALID_TYPE                     = SafeStatus( 0x04, "ERROR_INVALID_TYPE"                     );
    inline static SafeStatus ERROR_EXCEPTION_CAUGHT                 = SafeStatus( 0x05, "ERROR_EXCEPTION_CAUGHT"                 );
    inline static SafeStatus ERROR_FILE_READ_ERROR                  = SafeStatus( 0x06, "ERROR_FILE_READ_ERROR"                  );
    inline static SafeStatus WindowingBackendWINDOW_CREATION_FAILED = SafeStatus( 0x07, "WindowingBackendWINDOW_CREATION_FAILED" );
    inline static SafeStatus WindowingBackendGRAPHICS_INIT_FAILED   = SafeStatus( 0x08, "WindowingBackendGRAPHICS_INIT_FAILED"   );
    inline static SafeStatus InputManagerKEY_NOT_FOUND              = SafeStatus( 0x09, "InputManagerKEY_NOT_FOUND"              );
    inline static SafeStatus InputManagerKEY_IS_LOCKED              = SafeStatus( 0x0A, "InputManagerKEY_IS_LOCKED"              );
    inline static SafeStatus CommandLineINVALID_COMMAND             = SafeStatus( 0x0B, "CommandLineINVALID_COMMAND"             );
    inline static SafeStatus KeyBindsKEY_HAS_NO_BINDS               = SafeStatus( 0x0C, "KeyBindsKEY_HAS_NO_BINDS"               );
    inline static SafeStatus SettingsINVALID_SETTING_NAME           = SafeStatus( 0x0D, "SettingsINVALID_SETTING_NAME"           );
    inline static SafeStatus DataTypeINVALID_VARIABLE_NAME          = SafeStatus( 0x0E, "DataTypeINVALID_VARIABLE_NAME"          );
    inline static SafeStatus DataTypeEMPTY_VARIABLE                 = SafeStatus( 0x0F, "DataTypeEMPTY_VARIABLE"                 );
    inline static SafeStatus TheatreDataINVALID_TYPE                = SafeStatus( 0x10, "TheatreDataINVALID_TYPE"                );
    inline static SafeStatus ResourceUNKNOWN_FILETYPE               = SafeStatus( 0x11, "ResourceUNKNOWN_FILETYPE"               );
    inline static SafeStatus FileDataNO_DATA_OR_FILE                = SafeStatus( 0x12, "FileDataNO_DATA_OR_FILE"                );
    inline static SafeStatus FileDataFAILED_TO_PROCESS_FILE         = SafeStatus( 0x13, "FileDataFAILED_TO_PROCESS_FILE"         );
    inline static SafeStatus FileSystemINVALID_PATH                 = SafeStatus( 0x14, "FileSystemINVALID_PATH"                 );
    inline static SafeStatus FileSystemFAILED_TO_WRITE_FILE         = SafeStatus( 0x15, "FileSystemFAILED_TO_WRITE_FILE"         );
    inline static SafeStatus ResourceBAD_FILE_DATA                  = SafeStatus( 0x16, "ResourceBAD_FILE_DATA"                  );
    inline static SafeStatus FileDataDATA_IS_LOCKED                 = SafeStatus( 0x17, "FileDataDATA_IS_LOCKED"                 );
    inline static SafeStatus EventQueueINVALID_EVENT                = SafeStatus( 0x18, "EventQueueINVALID_EVENT"                );
    inline static SafeStatus ThingDataINVALID_VARIABLE_NAME         = SafeStatus( 0x19, "ThingDataINVALID_VARIABLE_NAME"         );
    inline static SafeStatus ThingDataINVALID_VARIABLE_TYPE         = SafeStatus( 0x20, "ThingDataINVALID_VARIABLE_TYPE"         );
    inline static SafeStatus ThingDataVARIABLE_VALUE_EMPTY          = SafeStatus( 0x21, "ThingDataVARIABLE_VALUE_EMPTY"          );
    inline static SafeStatus DemoControllerLINE_FAILED              = SafeStatus( 0x22, "DemoControllerLINE_FAILED"              );
    inline static SafeStatus DemoControllerLINE_PARSED              = SafeStatus( 0x23, "DemoControllerLINE_PARSED"              );
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
