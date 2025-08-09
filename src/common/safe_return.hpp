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
    inline static SafeStatus NO_ERROR                               = SafeStatus( 0b00000000000, "NO_ERROR"                               );
    inline static SafeStatus ERROR_GENERIC                          = SafeStatus( 0b10000000000, "ERROR_GENERIC"                          );
    inline static SafeStatus ERROR_ALREADY_ACTIVE                   = SafeStatus( 0b01000000000, "ERROR_ALREADY_ACTIVE"                   );
    inline static SafeStatus WindowingBackendWINDOW_CREATION_FAILED = SafeStatus( 0b00100000000, "WindowingBackendWINDOW_CREATION_FAILED" );
    inline static SafeStatus EventQueueNOT_ENABLED                  = SafeStatus( 0b00010000000, "EventQueueNOT_ENABLED"                  );
    inline static SafeStatus EventQueueNOT_PROCESSING_EVENTS        = SafeStatus( 0b00001000000, "EventQueueNOT_PROCESSING_EVENTS"        );
    inline static SafeStatus EventQueueQUEUE_EMPTY                  = SafeStatus( 0b00000100000, "EventQueueQUEUE_EMPTY"                  );
    inline static SafeStatus EventQueueINVALID_KEY_ID               = SafeStatus( 0b00000010000, "EventQueueINVALID_KEY_ID"               );
    inline static SafeStatus CommandLineFALLBACK_FUNCTION_USED      = SafeStatus( 0b00000001000, "CommandLineFALLBACK_FUNCTION_USED"      );
    inline static SafeStatus CommandLineINVALID_COMMAND             = SafeStatus( 0b00000000100, "CommandLineINVALID_COMMAND"             );
    inline static SafeStatus InputManagerKEY_NOT_FOUND              = SafeStatus( 0b00000000010, "InputManagerKEY_NOT_FOUND"              );
    inline static SafeStatus InputManagerKEY_IS_LOCKED              = SafeStatus( 0b00000000001, "InputManagerKEY_IS_LOCKED"              );
};

template<typename T>
struct SafeReturn
{
public:
    SafeReturn(T Data, SafeStatus ReturnStatus = Status::NO_ERROR)
    :_data(Data), _status(ReturnStatus)
    {}

    void SetStatus(SafeStatus NewStatus) { _status = NewStatus; }

    const T& Data() const { return _data; }
    SafeStatus Status() const { return _status; }

private:
    T _data;
    SafeStatus _status = Status::NO_ERROR;
};

#endif // SAFE_RETURN_H
