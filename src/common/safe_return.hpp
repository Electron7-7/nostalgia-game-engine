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
    inline static SafeStatus NO_ERROR                               = SafeStatus( 0b000000000000, "NO_ERROR"                               );
    inline static SafeStatus ERROR_GENERIC                          = SafeStatus( 0b100000000000, "ERROR_GENERIC"                          );
    inline static SafeStatus ERROR_ALREADY_ACTIVE                   = SafeStatus( 0b010000000000, "ERROR_ALREADY_ACTIVE"                   );
    inline static SafeStatus ERROR_INVALID_KEY_ID                   = SafeStatus( 0b001000000000, "ERROR_INVALID_KEY_ID"                   );
    inline static SafeStatus WindowingBackendWINDOW_CREATION_FAILED = SafeStatus( 0b000100000000, "WindowingBackendWINDOW_CREATION_FAILED" );
    inline static SafeStatus WindowingBackendGRAPHICS_INIT_FAILED   = SafeStatus( 0b000100000000, "WindowingBackendGRAPHICS_INIT_FAILED"   );
    inline static SafeStatus EventQueueNOT_ENABLED                  = SafeStatus( 0b000010000000, "EventQueueNOT_ENABLED"                  );
    inline static SafeStatus EventQueueNOT_PROCESSING_EVENTS        = SafeStatus( 0b000001000000, "EventQueueNOT_PROCESSING_EVENTS"        );
    inline static SafeStatus EventQueueEMPTY                        = SafeStatus( 0b000000100000, "EventQueueEMPTY"                        );
    inline static SafeStatus InputManagerKEY_NOT_FOUND              = SafeStatus( 0b000000010000, "InputManagerKEY_NOT_FOUND"              );
    inline static SafeStatus InputManagerKEY_IS_LOCKED              = SafeStatus( 0b000000001000, "InputManagerKEY_IS_LOCKED"              );
    inline static SafeStatus CommandLineINVALID_COMMAND             = SafeStatus( 0b000000000100, "CommandLineINVALID_COMMAND"             );
    inline static SafeStatus KeyBindsKEY_HAS_NO_BINDS               = SafeStatus( 0b000000000010, "KeyBindsKEY_HAS_NO_BINDS"               );
    inline static SafeStatus SettingsINVALID_SETTING_NAME           = SafeStatus( 0b000000000001, "SettingsINVALID_SETTING_NAME"           );
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
