#ifndef GAMEUTIL_CRITICAL_SECTION_HPP
#define GAMEUTIL_CRITICAL_SECTION_HPP

#include <revolution/types.h>

#include <revolution/OS.h>

class CriticalSection {
public:
    CriticalSection(void) { mPrevInterrupts = OSDisableInterrupts(); }
    ~CriticalSection(void) { OSRestoreInterrupts(mPrevInterrupts); }

private:
    BOOL mPrevInterrupts;
};

#endif 
