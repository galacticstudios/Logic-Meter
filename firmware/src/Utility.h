/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _UTILITY_H    /* Guard against multiple inclusion */
#define _UTILITY_H

#include <string>
#include <stdint.h>
#include "printf.h"

#define countof(x) (sizeof(x) / sizeof((x)[0]))


#define UTF8_MU "\xCE\xBC"
#define UTF8_LEFTARROW "\xE2\x86\x90"
#define UTF8_UPARROW "\xE2\x86\x91"
#define UTF8_RIGHTARROW "\xE2\x86\x92"
#define UTF8_DOWNARROW "\xE2\x86\x93"
#define UTF8_OHM "\xE2\x84\xA6"
#define UTF8_PLUSMINUS "\xC2\xB1"
#define UTF8_DEGREES "\xC2\xB0"
#define UTF8_MULTIPLICATION "\xC3\x97"


#define ASSERT(x) {if (!(x)) __builtin_software_breakpoint();}

struct laString_t;
typedef struct laString_t laString;
struct GFXU_FontAsset_t;
typedef struct GFXU_FontAsset_t GFXU_FontAsset;

laString laString_CreateFromUTF8Buffer(const char *chr, GFXU_FontAsset *fnt);

std::string eng(double value, int digits, const char *units);

class Tool;

class Callback
{
public:
    virtual void operator()(Tool *tool) const = 0;
};
    
template<class T>
class _Callback : public Callback
{
public:
    _Callback(void (T::*fn)()) : _fn(fn) {}
    
    void operator()(Tool *obj) const 
    {
        T *t = dynamic_cast<T *>(obj);
        ASSERT(t);
        (t->*_fn)();
    }
    
private:
    void (T::*_fn)();
};

template <class T> 
Callback *CB(void (T::*fn)()) 
{
    return new _Callback<T>(fn);
}

class DisableInterrupts
{
public:
    DisableInterrupts() : _status(__builtin_disable_interrupts()) {}
    ~DisableInterrupts()
    {
        if (_status & 1)
            __builtin_enable_interrupts();
    }
    
private:
    int _status;
};

// Circular queue. Thread safe.
// If you write to a full queue, the oldest data gets dropped
template <class T, int N>
class CQueue
{
public:
    bool empty() const 
    {
        DisableInterrupts di;
        return (_head == _tail);
    }
    bool full() const
    {
        DisableInterrupts di;
        return (Inc(_head) == _tail);
    }
    void clear() 
    {
        DisableInterrupts di;
        _head = _tail = 0;
    }
    // If the queue is full, we discard the oldest data and add the new one
    bool write(T data)
    {
        bool overflow = false;
        DisableInterrupts di;
        size_t newHead = Inc(_head);
        if (newHead == _tail)
        {
            _tail = Inc(_tail);
            overflow = true;
        }
        _queue[_head] = data;
        _head = newHead;
        return !overflow;
    }
    // Call when you don't need interrupts disabled
    bool writeUnsafe(T data)
    {
        bool overflow = false;
        size_t newHead = Inc(_head);
        if (newHead == _tail)
        {
            _tail = Inc(_tail);
            overflow = true;
        }
        _queue[_head] = data;
        _head = newHead;
        return !overflow;
    }
    bool read(T *data)
    {
        DisableInterrupts di;
        if (_head == _tail)
        {
            return false;
        }
        if (data)
            *data = _queue[_tail];
        _tail = Inc(_tail);
        return true;
    }
    bool peek(T *data)
    {
        DisableInterrupts di;
        if (_head == _tail)
        {
            return false;
        }
        if (data)
            *data = _queue[_tail];
        return true;
    }
    size_t read(T *data, size_t maxSize)
    {
        DisableInterrupts di;
        size_t totalCopySize = 0, copySize;
        while (maxSize && _head != _tail)
        {
            if (_tail < _head)
                copySize = _head - _tail;
            else
                copySize = N - _tail;
            if (copySize > maxSize)
                copySize = maxSize;
            memcpy(data, _queue + _tail, copySize * sizeof(T));
            _tail += copySize;
            if (_tail == N)
                _tail = 0;
            maxSize -= copySize;
            data += copySize;
            totalCopySize += copySize;
        }
        return totalCopySize;
    }

private:
    size_t Inc(size_t in) const
    {
        if (++in == N)
            return 0;
        return in;
    }
    size_t _head = 0, _tail = 0;
    T _queue[N];
};

typedef struct
{
    int32_t x, y, width, height;
} Rect;

#endif /* _UTILITY_H */
