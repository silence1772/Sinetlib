#ifndef ATOMIC_H
#define ATOMIC_H

#include <stdint.h>

// 原子类，用于数字增减的原子操作
template<typename T>
class Atomic
{
public:
    Atomic() : value_(0) {}

    T Get() const { return __sync_val_compare_and_swap(const_cast<volatile T*>(&value_), 0, 0); }
    
    T GetAndAdd(T x) { return __sync_fetch_and_add(&value_, x); }
    
    T AddAndGet(T x) { return GetAndAdd(x) + x; }

    T GetAndSet(T x) { return __sync_lock_test_and_set(&value_, x); }

    // ++i
    T IncreaseAndGet() { return AddAndGet(1); }

    // --i
    T DecreaseAndGet() { return AddAndGet(-1); }

    void Add(T x) { GetAndAdd(x); }

    void Increase() { IncreaseAndGet(); }

    void Decrease() { DecreaseAndGet(); }

private:
    volatile T value_;
};

#endif // ATOMIC_H