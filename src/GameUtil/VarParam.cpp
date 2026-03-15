#include "VarParam.hpp"
#include "DebugPrint.hpp"

void CVarParam::fn_801ECEC4(s32 x, s32 y) {
    gDebugPrint->fn_801EC674(x, y, 1, "%s", mTitle);
    
    switch (mType) {
        case 0:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mUlongVal);
            break;
        case 1:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mUintVal);
            break;
        case 2:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mUshortVal);
            break;
        case 3:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mUcharVal);
            break;

        case 4:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mLongVal);
            break;
        case 5:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mIntVal);
            break;
        case 6:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mShortVal);
            break;
        case 7:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%d", mCharVal);
            break;

        case 8:
            gDebugPrint->fn_801EC674(x + 200, y, 1, "%f", mFloatVal);
            break;
    }
}

CW_FORCE_STRINGS(var_param, "  %s : \t : %d\n", "  %s : \t : %f\n");
