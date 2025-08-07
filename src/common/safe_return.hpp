#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

#include <map>
#include <string>

struct SafeStatus
{
public:
    SafeStatus(): _status(0), _printout("No Error") {}
    SafeStatus(const SafeStatus& CopyFrom): _status(CopyFrom._status), _printout(CopyFrom._printout) {}

    const unsigned short Status() const { return _status; }
    const char* Printout() const { return _printout.c_str(); }

    constexpr bool operator==(const SafeStatus& CompareTo) const { return (_status == CompareTo._status); }
    constexpr bool operator!=(const SafeStatus& CompareTo) const { return (_status != CompareTo._status); }
    constexpr bool operator< (const SafeStatus& CompareTo) const { return (_status <  CompareTo._status); }
    constexpr bool operator> (const SafeStatus& CompareTo) const { return (_status >  CompareTo._status); }
    constexpr bool operator<=(const SafeStatus& CompareTo) const { return (_status <= CompareTo._status); }
    constexpr bool operator>=(const SafeStatus& CompareTo) const { return (_status >= CompareTo._status); }

    constexpr operator unsigned short() const { return _status; }

private:
    unsigned short _status;
    std::string _printout = "ERR";

    friend struct Status;
    constexpr SafeStatus(const unsigned short Status, const char* Printout): _status(Status), _printout(Printout) {}
};

struct Status
{
    inline static SafeStatus NO_ERROR                          = SafeStatus( 0b00000000, "NO_ERROR"                          );
    inline static SafeStatus ERROR_GENERIC                     = SafeStatus( 0b00000001, "ERROR_GENERIC"                     );
    inline static SafeStatus ERROR_ALREADY_ACTIVE              = SafeStatus( 0b00000010, "ERROR_ALREADY_ACTIVE"              );
    inline static SafeStatus EventQueueNOT_ENABLED             = SafeStatus( 0b00000100, "EventQueueNOT_ENABLED"             );
    inline static SafeStatus EventQueueNOT_PROCESSING_EVENTS   = SafeStatus( 0b00001000, "EventQueueNOT_PROCESSING_EVENTS"   );
    inline static SafeStatus EventQueueQUEUE_EMPTY             = SafeStatus( 0b00010000, "EventQueueQUEUE_EMPTY"             );
    inline static SafeStatus EventQueueINVALID_KEY_ID          = SafeStatus( 0b00100000, "EventQueueINVALID_KEY_ID"          );
    inline static SafeStatus CommandLineFALLBACK_FUNCTION_USED = SafeStatus( 0b01000000, "CommandLineFALLBACK_FUNCTION_USED" );
    inline static SafeStatus CommandLineINVALID_COMMAND        = SafeStatus( 0b01000000, "CommandLineINVALID_COMMAND"        );
};


inline const std::map<SafeStatus*, const char*> StatusPrintouts =
{
    { &Status::NO_ERROR,                          "NO_ERROR"                          },
    { &Status::ERROR_GENERIC,                     "ERROR_GENERIC"                     },
    { &Status::ERROR_ALREADY_ACTIVE,              "ERROR_ALREADY_ACTIVE"              },
    { &Status::EventQueueNOT_ENABLED,             "EventQueueNOT_ENABLED"             },
    { &Status::EventQueueNOT_PROCESSING_EVENTS,   "EventQueueNOT_PROCESSING_EVENTS"   },
    { &Status::EventQueueQUEUE_EMPTY,             "EventQueueQUEUE_EMPTY"             },
    { &Status::EventQueueINVALID_KEY_ID,          "EventQueueINVALID_KEY_ID"          },
    { &Status::CommandLineFALLBACK_FUNCTION_USED, "CommandLineFALLBACK_FUNCTION_USED" },
    { &Status::CommandLineINVALID_COMMAND,        "CommandLineINVALID_COMMAND"        },
};


template<typename T>
struct SafeReturn
{
public:

    SafeReturn(T Data, SafeStatus ReturnStatus = Status::NO_ERROR)
    :_data(Data), _status(ReturnStatus)
    {}

    void SetStatus(SafeStatus NewStatus) { _status = NewStatus; }

    const T& GetData() const { return _data; }
    SafeStatus GetStatus() const { return _status; }

private:
    T _data;
    SafeStatus _status = Status::NO_ERROR;
};

#endif // SAFE_RETURN_H
