#include "DebugPrint.hpp"

#include <cstdio>

#include "DebugFont.hpp"

CDebugPrint::CDebugPrint(void) :
    mUnk04(0), mSBGList(), mUnk14(0), mUnk2C(-1)
{}

CDebugPrint::SBG::SBG(void) {
    unk10 = -1;
}
CDebugPrint::SBG::~SBG(void) {}

CDebugPrint::~CDebugPrint(void) {}

void CDebugPrint::_10(void) {}

void CDebugPrint::_08(void) {}

void CDebugPrint::_14(void) {
    setFontColor(nw4r::ut::Color::WHITE);
    setFontSize(16.0f);
}

void CDebugPrint::fn_801EC674(s32 x, s32 y, s32 frames, const char *format, ...) {
    va_list args;

    if (mEnable) {
        va_start(args, format);

        Vec2i size = fn_801EC850(format, args);
        s32 sizeX = size.x;
        s32 sizeY = size.y;

        va_end(args);

        if (x < mUnk1C) {
            mUnk1C = x;
        }
        if (y < mUnk20) {
            mUnk20 = y;
        }

        if ((x + sizeX) > mUnk24) {
            mUnk24 = (x + sizeX);
        }
        if ((y + sizeY) > mUnk28) {
            mUnk28 = (y + sizeY);
        }
    }

    va_start(args, format);
    doRegister(x, y, frames, format, args);
    va_end(args);
}

// non matching: data
// ptmfs are misplaced, may have something
// to do with the pool data hack below
// (needed to match fn_801EC850)
#pragma push
#pragma pool_data off

struct LineState {
    f32 curLineLen;
    f32 maxLineLen;

    void updateMaxLineLen(void) {
        f32 _maxLineLen = maxLineLen;
        f32 _curLineLen = curLineLen;
        maxLineLen = _curLineLen < _maxLineLen ? _maxLineLen : _curLineLen;
    }
};

Vec2i CDebugPrint::fn_801EC850(const char *format, std::va_list args) {
    if (nw4r::db::DbgPrint::GetInstance() != NULL) {
        nw4r::ut::Font *font = gDebugFont->getFont();
    
        char buffer[256];
        s32 charCount = vsnprintf(buffer, sizeof(buffer), format, args);
    
        nw4r::ut::CharStrmReader charStrmReader = font->GetCharStrmReader();
        charStrmReader.Set(buffer);

        f32 fontScale = nw4r::db::DbgPrint::GetInstance()->GetFontSize() / font->GetHeight();

        LineState lineState = { 0 };
        f32 lineHeight = fontScale * font->GetLineFeed();

        u16 curChar = charStrmReader.Next();
        while ((static_cast<const char *>(charStrmReader.GetCurrentPos()) - buffer) <= charCount) {
            if (curChar == '\n') {
                lineState.updateMaxLineLen();
                lineState.curLineLen = 0.0f;

                lineHeight += fontScale * font->GetLineFeed();
            }
            else {
                lineState.curLineLen += fontScale * font->GetCharWidth(curChar);
            }
    
            curChar = charStrmReader.Next();
        }

        lineState.updateMaxLineLen();

        return (Vec2i) { lineState.maxLineLen, lineHeight };
    }
    else {
        return (Vec2i) { 0, 0 };
    }
}

#pragma pop

void CDebugPrint::_18(void) {}
