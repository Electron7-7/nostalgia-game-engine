#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

#include "error.hpp"
#include "printing.hpp"

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
    constexpr SafeStatus MIGRATION_TO_ERROR                     { 0x01, "MIGRATION_TO_ERROR"                     };
    constexpr SafeStatus ERROR_GENERIC                          { 0x02, "ERROR_GENERIC"                          };
    constexpr SafeStatus ERROR_ALREADY_ACTIVE                   { 0x03, "ERROR_ALREADY_ACTIVE"                   };
    constexpr SafeStatus ERROR_INVALID_ID                       { 0x04, "ERROR_INVALID_ID"                       };
    constexpr SafeStatus ERROR_INVALID_TYPE                     { 0x05, "ERROR_INVALID_TYPE"                     };
    constexpr SafeStatus ERROR_EXCEPTION_CAUGHT                 { 0x06, "ERROR_EXCEPTION_CAUGHT"                 };
    constexpr SafeStatus ERROR_FILE_READ_ERROR                  { 0x07, "ERROR_FILE_READ_ERROR"                  };
    constexpr SafeStatus WindowingBackendWINDOW_CREATION_FAILED { 0x08, "WindowingBackendWINDOW_CREATION_FAILED" };
    constexpr SafeStatus WindowingBackendGRAPHICS_INIT_FAILED   { 0x09, "WindowingBackendGRAPHICS_INIT_FAILED"   };
    constexpr SafeStatus InputManagerKEY_NOT_FOUND              { 0x0A, "InputManagerKEY_NOT_FOUND"              };
    constexpr SafeStatus InputManagerKEY_IS_LOCKED              { 0x0B, "InputManagerKEY_IS_LOCKED"              };
    constexpr SafeStatus CommandLineINVALID_COMMAND             { 0x0C, "CommandLineINVALID_COMMAND"             };
    constexpr SafeStatus KeyBindsKEY_HAS_NO_BINDS               { 0x0D, "KeyBindsKEY_HAS_NO_BINDS"               };
    constexpr SafeStatus SettingsINVALID_SETTING_NAME           { 0x0E, "SettingsINVALID_SETTING_NAME"           };
    constexpr SafeStatus DataTypeINVALID_VARIABLE_NAME          { 0x0F, "DataTypeINVALID_VARIABLE_NAME"          };
    constexpr SafeStatus DataTypeEMPTY_VARIABLE                 { 0x10, "DataTypeEMPTY_VARIABLE"                 };
    constexpr SafeStatus TheatreDataINVALID_TYPE                { 0x11, "TheatreDataINVALID_TYPE"                };
    constexpr SafeStatus ResourceUNKNOWN_FILETYPE               { 0x12, "ResourceUNKNOWN_FILETYPE"               };
    constexpr SafeStatus FileDataNO_DATA_OR_FILE                { 0x13, "FileDataNO_DATA_OR_FILE"                };
    constexpr SafeStatus FileDataFAILED_TO_PROCESS_FILE         { 0x14, "FileDataFAILED_TO_PROCESS_FILE"         };
    constexpr SafeStatus FileSystemINVALID_PATH                 { 0x15, "FileSystemINVALID_PATH"                 };
    constexpr SafeStatus FileSystemFAILED_TO_WRITE_FILE         { 0x16, "FileSystemFAILED_TO_WRITE_FILE"         };
    constexpr SafeStatus ResourceBAD_FILE_DATA                  { 0x17, "ResourceBAD_FILE_DATA"                  };
    constexpr SafeStatus FileDataDATA_IS_LOCKED                 { 0x18, "FileDataDATA_IS_LOCKED"                 };
    constexpr SafeStatus EventQueueINVALID_EVENT                { 0x19, "EventQueueINVALID_EVENT"                };
    constexpr SafeStatus ThingDataINVALID_VARIABLE_NAME         { 0x20, "ThingDataINVALID_VARIABLE_NAME"         };
    constexpr SafeStatus ThingDataINVALID_VARIABLE_TYPE         { 0x21, "ThingDataINVALID_VARIABLE_TYPE"         };
    constexpr SafeStatus ThingDataVARIABLE_VALUE_EMPTY          { 0x22, "ThingDataVARIABLE_VALUE_EMPTY"          };
    constexpr SafeStatus DemoControllerLINE_FAILED              { 0x23, "DemoControllerLINE_FAILED"              };
    constexpr SafeStatus DemoControllerLINE_PARSED              { 0x24, "DemoControllerLINE_PARSED"              };
}

template<typename T>
struct SafeReturn
{
public:
    SafeReturn(const T& Data, SafeStatus ReturnStatus = Status::NO_ERR):
        mData{Data},
        mStatus{ReturnStatus} {}

    SafeReturn(const T& Data, Error ReturnStatus = OK):
        mData{Data},
        mStatus{Status::MIGRATION_TO_ERROR},
        mNewStatus{ReturnStatus} {}

    constexpr T& Data() { return mData; }
    constexpr const SafeStatus& Status() const { return mStatus; }
    constexpr Error ErrStatus() const { return mNewStatus; }

    bool Check() const
    { if(mStatus == Status::MIGRATION_TO_ERROR) { return mNewStatus; } return mStatus; }

    bool PrintCheck(const char* Prefix = "SafeReturn::PrintCheck") const
    {
        if(mStatus == Status::MIGRATION_TO_ERROR)
        {
            return (mNewStatus)
                ? true
                : print_error("{} returned Error code: {}", Prefix, (int)mNewStatus);
        }
        return mStatus.PrintCheck(Prefix);
    }

private:
    T mData{};
    SafeStatus mStatus{Status::NO_ERR};
    Error mNewStatus{OK}; // Migrating to 'Error'
};

#endif // SAFE_RETURN_H
