#ifndef SAFE_RETURN_H
#define SAFE_RETURN_H

template<typename T>
    class SafeReturn
    {
    public:
        explicit SafeReturn(const T* inData) noexcept:
            mData{(inData) ? inData : MakeUnique<T>()}, mStatus{(inData) ? OK : ERR_NULLPTR} {}

        explicit SafeReturn(Farg<T> inData) noexcept:
            mData{MakeUnique<T>(inData)}, mStatus{OK} {}

        SafeReturn(Error inError = FAILED) noexcept:
            mData{MakeUnique<T>()}, mStatus{inError} {}

        Error status() const noexcept
        { return mStatus; }

        Farg<Unique<T>> data() const noexcept
        { return mData; }

        Farg<T> data() noexcept
        { return *mData; }

        operator Error() const noexcept
        { return mStatus; }

        operator Farg<Unique<T>>() const noexcept
        { return mData; }

        operator Farg<T>() const noexcept
        { return *mData; }

    private:
        Unique<T> mData{};
        Error mStatus{OK};
    };

#endif // SAFE_RETURN_H
