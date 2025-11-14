#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

#include "common/printing.hpp"

namespace
{
    typedef unsigned int __statusId_t;
    constexpr __statusId_t NO_ERR_STATUS = 0x0;
}

struct SafeStatus
{
public:
    constexpr SafeStatus(const __statusId_t Status, const char* Printout):
        mStatus(Status), mPrintout(Printout) {}

    constexpr __statusId_t Status()  const { return mStatus;   }
    constexpr const char* Printout() const { return mPrintout; }

    constexpr bool operator==(const SafeStatus& CompareTo) const { return (mStatus == CompareTo.mStatus); }
    constexpr bool operator!=(const SafeStatus& CompareTo) const { return (mStatus != CompareTo.mStatus); }
    constexpr bool operator< (const SafeStatus& CompareTo) const { return (mStatus <  CompareTo.mStatus); }
    constexpr bool operator> (const SafeStatus& CompareTo) const { return (mStatus >  CompareTo.mStatus); }
    constexpr bool operator<=(const SafeStatus& CompareTo) const { return (mStatus <= CompareTo.mStatus); }
    constexpr bool operator>=(const SafeStatus& CompareTo) const { return (mStatus >= CompareTo.mStatus); }

    constexpr operator bool() const
    { return mStatus == NO_ERR_STATUS; }

    bool PrintCheck(const char* Prefix = "SafeStatus::PrintCheck") const
    {
        return (*this)
            ? true
            : print_error("{} returned '{}'", Prefix, Printout());
    }

private:
    __statusId_t mStatus{NO_ERR_STATUS};
    const char* mPrintout{"NO_ERR"};
};

namespace Status
{
    constexpr SafeStatus NO_ERR                                 { 0x00, "NO_ERR"                                 };
    constexpr SafeStatus ERROR_GENERIC                          { 0x01, "ERROR_GENERIC"                          };
    constexpr SafeStatus ERROR_ALREADY_ACTIVE                   { 0x02, "ERROR_ALREADY_ACTIVE"                   };
    constexpr SafeStatus ERROR_INVALID_ID                       { 0x03, "ERROR_INVALID_ID"                       };
    constexpr SafeStatus ERROR_INVALID_TYPE                     { 0x04, "ERROR_INVALID_TYPE"                     };
    constexpr SafeStatus ERROR_EXCEPTION_CAUGHT                 { 0x05, "ERROR_EXCEPTION_CAUGHT"                 };
    constexpr SafeStatus ERROR_FILE_READ_ERROR                  { 0x06, "ERROR_FILE_READ_ERROR"                  };
    constexpr SafeStatus WindowingBackendWINDOW_CREATION_FAILED { 0x07, "WindowingBackendWINDOW_CREATION_FAILED" };
    constexpr SafeStatus WindowingBackendGRAPHICS_INIT_FAILED   { 0x08, "WindowingBackendGRAPHICS_INIT_FAILED"   };
    constexpr SafeStatus InputManagerKEY_NOT_FOUND              { 0x09, "InputManagerKEY_NOT_FOUND"              };
    constexpr SafeStatus InputManagerKEY_IS_LOCKED              { 0x0A, "InputManagerKEY_IS_LOCKED"              };
    constexpr SafeStatus CommandLineINVALID_COMMAND             { 0x0B, "CommandLineINVALID_COMMAND"             };
    constexpr SafeStatus KeyBindsKEY_HAS_NO_BINDS               { 0x0C, "KeyBindsKEY_HAS_NO_BINDS"               };
    constexpr SafeStatus SettingsINVALID_SETTING_NAME           { 0x0D, "SettingsINVALID_SETTING_NAME"           };
    constexpr SafeStatus DataTypeINVALID_VARIABLE_NAME          { 0x0E, "DataTypeINVALID_VARIABLE_NAME"          };
    constexpr SafeStatus DataTypeEMPTY_VARIABLE                 { 0x0F, "DataTypeEMPTY_VARIABLE"                 };
    constexpr SafeStatus TheatreDataINVALID_TYPE                { 0x10, "TheatreDataINVALID_TYPE"                };
    constexpr SafeStatus ResourceUNKNOWN_FILETYPE               { 0x11, "ResourceUNKNOWN_FILETYPE"               };
    constexpr SafeStatus FileDataNO_DATA_OR_FILE                { 0x12, "FileDataNO_DATA_OR_FILE"                };
    constexpr SafeStatus FileDataFAILED_TO_PROCESS_FILE         { 0x13, "FileDataFAILED_TO_PROCESS_FILE"         };
    constexpr SafeStatus FileSystemINVALID_PATH                 { 0x14, "FileSystemINVALID_PATH"                 };
    constexpr SafeStatus FileSystemFAILED_TO_WRITE_FILE         { 0x15, "FileSystemFAILED_TO_WRITE_FILE"         };
    constexpr SafeStatus ResourceBAD_FILE_DATA                  { 0x16, "ResourceBAD_FILE_DATA"                  };
    constexpr SafeStatus FileDataDATA_IS_LOCKED                 { 0x17, "FileDataDATA_IS_LOCKED"                 };
    constexpr SafeStatus EventQueueINVALID_EVENT                { 0x18, "EventQueueINVALID_EVENT"                };
    constexpr SafeStatus ThingDataINVALID_VARIABLE_NAME         { 0x19, "ThingDataINVALID_VARIABLE_NAME"         };
    constexpr SafeStatus ThingDataINVALID_VARIABLE_TYPE         { 0x20, "ThingDataINVALID_VARIABLE_TYPE"         };
    constexpr SafeStatus ThingDataVARIABLE_VALUE_EMPTY          { 0x21, "ThingDataVARIABLE_VALUE_EMPTY"          };
    constexpr SafeStatus DemoControllerLINE_FAILED              { 0x22, "DemoControllerLINE_FAILED"              };
    constexpr SafeStatus DemoControllerLINE_PARSED              { 0x23, "DemoControllerLINE_PARSED"              };
}

template<typename T>
struct SafeReturn
{
public:
    SafeReturn(const T& Data, SafeStatus ReturnStatus = Status::NO_ERR):
        mData{Data},
        mStatus{ReturnStatus} {}

    constexpr T& Data() { return mData; }
    constexpr const SafeStatus& Status() const { return mStatus; }

    bool Check() const
    { return mStatus; }

    bool PrintCheck(const char* Prefix = "SafeReturn::PrintCheck") const
    { return mStatus.PrintCheck(Prefix); }

private:
    T mData{};
    SafeStatus mStatus{Status::NO_ERR};
};

template<typename T>
struct SafePointer
{
public:
    constexpr SafePointer(T* DataPtr, SafeStatus ReturnStatus = Status::NO_ERR):
        m_pData{DataPtr},
        mStatus{ReturnStatus} {}

    constexpr SafePointer(T& NonLocalData, SafeStatus ReturnStatus = Status::NO_ERR):
        SafePointer(&NonLocalData, ReturnStatus) {}

    constexpr T& Data() { return *m_pData; }
    constexpr const SafeStatus& Status() const { return mStatus; }

private:
    T const* m_pData{nullptr};
    SafeStatus mStatus{Status::NO_ERR};
};

#endif // SAFE_RETURN_H
