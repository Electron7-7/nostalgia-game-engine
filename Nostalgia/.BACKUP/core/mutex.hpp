#ifndef MUTEX_H
#define MUTEX_H

using Mutex  = std::mutex;
using RMutex = std::recursive_mutex;
template<typename T>
    using LockGuard  = const std::lock_guard<T>;
template<typename T>
    using ScopedLock = std::scoped_lock<T>;

#endif // MUTEX_H
