#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

namespace
{
    typedef unsigned int __statusId_t;
    constexpr __statusId_t NO_ERROR_STATUS = 0;
}

struct SafeStatus
{
public:
    SafeStatus(): m_Status(NO_ERROR_STATUS), m_Printout("NO_ERROR") {}
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



private:
    __statusId_t m_Status = NO_ERROR_STATUS;
    const char* m_Printout  = "NO_ERROR?";

    friend struct Status;
    constexpr SafeStatus(const __statusId_t Status, const char* Printout): m_Status(Status), m_Printout(Printout) {}
};

struct Status
{
    inline static SafeStatus NO_ERROR;
    inline static SafeStatus ERROR_GENERIC                          = SafeStatus( 0x1,  "ERROR_GENERIC"                          );
    inline static SafeStatus ERROR_ALREADY_ACTIVE                   = SafeStatus( 0x2,  "ERROR_ALREADY_ACTIVE"                   );
    inline static SafeStatus ERROR_INVALID_KEY_ID                   = SafeStatus( 0x3,  "ERROR_INVALID_KEY_ID"                   );
    inline static SafeStatus WindowingBackendWINDOW_CREATION_FAILED = SafeStatus( 0x4,  "WindowingBackendWINDOW_CREATION_FAILED" );
    inline static SafeStatus WindowingBackendGRAPHICS_INIT_FAILED   = SafeStatus( 0x5,  "WindowingBackendGRAPHICS_INIT_FAILED"   );
    inline static SafeStatus EventQueueNOT_ENABLED                  = SafeStatus( 0x6,  "EventQueueNOT_ENABLED"                  );
    inline static SafeStatus EventQueueNOT_PROCESSING_EVENTS        = SafeStatus( 0x7,  "EventQueueNOT_PROCESSING_EVENTS"        );
    inline static SafeStatus EventQueueEMPTY                        = SafeStatus( 0x8,  "EventQueueEMPTY"                        );
    inline static SafeStatus InputManagerKEY_NOT_FOUND              = SafeStatus( 0x9,  "InputManagerKEY_NOT_FOUND"              );
    inline static SafeStatus InputManagerKEY_IS_LOCKED              = SafeStatus( 0xA,  "InputManagerKEY_IS_LOCKED"              );
    inline static SafeStatus CommandLineINVALID_COMMAND             = SafeStatus( 0xB,  "CommandLineINVALID_COMMAND"             );
    inline static SafeStatus KeyBindsKEY_HAS_NO_BINDS               = SafeStatus( 0xC,  "KeyBindsKEY_HAS_NO_BINDS"               );
    inline static SafeStatus SettingsINVALID_SETTING_NAME           = SafeStatus( 0xD,  "SettingsINVALID_SETTING_NAME"           );
    inline static SafeStatus ResourceManagerFAILED_TO_LOAD_OBJ      = SafeStatus( 0xE,  "ResourceManagerFAILED_TO_LOAD_OBJ"      );
    inline static SafeStatus TheatreParserFILE_DOES_NOT_EXIST       = SafeStatus( 0xF,  "TheatreParserFILE_DOES_NOT_EXIST"       );
    inline static SafeStatus TheatreParserWRONG_FILE_EXTENSION      = SafeStatus( 0x10, "TheatreParserWRONG_FILE_EXTENSION"      );
    inline static SafeStatus TheatreParserNO_THEATRE_LOADED         = SafeStatus( 0x11, "TheatreParserNO_THEATRE_LOADED"         );
};

template<typename T>
struct SafeReturn
{
public:
    SafeReturn(T Data, SafeStatus ReturnStatus = Status::NO_ERROR)
    : m_Data(Data), m_Status(ReturnStatus)
    {}

    const T& Data() const { return m_Data; }
    SafeStatus Status() const { return m_Status; }

private:
    T m_Data;
    SafeStatus m_Status = Status::NO_ERROR;
};

#endif // SAFE_RETURN_H
