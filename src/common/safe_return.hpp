#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

struct SafeStatus
{
public:
    SafeStatus(): _status(0), _printout("NO_ERROR") {}
    SafeStatus(const SafeStatus& CopyFrom): _status(CopyFrom._status), _printout(CopyFrom._printout) {}

    const unsigned short Status() const { return _status; }
    const char* Printout() const { return _printout; }

    constexpr bool operator==(const SafeStatus& CompareTo) const { return (_status == CompareTo._status); }
    constexpr bool operator!=(const SafeStatus& CompareTo) const { return (_status != CompareTo._status); }
    constexpr bool operator< (const SafeStatus& CompareTo) const { return (_status <  CompareTo._status); }
    constexpr bool operator> (const SafeStatus& CompareTo) const { return (_status >  CompareTo._status); }
    constexpr bool operator<=(const SafeStatus& CompareTo) const { return (_status <= CompareTo._status); }
    constexpr bool operator>=(const SafeStatus& CompareTo) const { return (_status >= CompareTo._status); }

    constexpr operator unsigned short() const { return _status; }

private:
    unsigned short _status = 0;
    const char* _printout  = "You shouldn't be seeing this!";

    friend struct Status;
    constexpr SafeStatus(const unsigned short Status, const char* Printout): _status(Status), _printout(Printout) {}
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
    :_data(Data), _status(ReturnStatus)
    {}

    const T& Data() const { return _data; }
    SafeStatus Status() const { return _status; }

private:
    T _data;
    SafeStatus _status = Status::NO_ERROR;
};

#endif // SAFE_RETURN_H
