#include "Controller.hpp"

#include <cstring>

#include <revolution/PAD.h>
#include <revolution/KPAD.h>
#include <revolution/WPAD.h>

#include <nw4r/math.h>

#include "Mem.hpp"

#include "TickFlowManager.hpp"

/* 
        CController
*/

CController::CController(s32 channel) {
    mMyChannel = channel;
    unk136D = false;
    unk136C = false;
    unk1370 = 0;
    mMotorSeqPlaying = 0;
    mButtonHold = 0;
    mButtonTrig = 0;
    mButtonRelease = 0;
    for (s32 i = 0; i < BUTTON_COUNT; i++) {
        mButtonCoolTimer[i] = 0;
    }
    unk1364 = 10;
    unk1365 = 4;
    for (s32 i = 0; i < BUTTON_COUNT; i++) {
        u16 bit = 1 << i;
        if (bit != 0) {
            mButtonCoolFrames[i] = 0;
        }
    }

    for (s32 i = 0; i < BUTTON_COUNT; i++) {
        u16 bit = 1 << i;
        if ((bit & (WPAD_BUTTON_A | WPAD_BUTTON_B)) != 0) {
            mButtonCoolFrames[i] = 6;
        }
    }
}

void CController::_0C(void) {
    mInfoUpdated = false;
}

void CController::_10(void) {
    u32 prevHold = mButtonHold;
    fn_801D4DDC();
    fn_801D4E38(prevHold);
    unk1368 = 0;
    fn_801D4F74(prevHold);
    fn_801D4FD8();

    KPADSetPosParam(mMyChannel, .05f, .8f);
    if (mCoreStatus[0].speed < .01f) {
        KPADSetPosParam(mMyChannel, .05f, .13f);
    }
}

void CController::_14(void) {
    _4C();
}

void CController::fn_801D4DDC(void) {
    mProbeErrcode = WPADProbe(mMyChannel, &mProbeType);
    mKPADReadLength = KPADRead(mMyChannel, mCoreStatus, ARRAY_LENGTH(mCoreStatus));
    KPADGetUnifiedWpadStatus(mMyChannel, mUnifiedStatus, mKPADReadLength);
}

void CController::fn_801D4E38(u32 prevHold) {
    do801D4EA4(prevHold);
}

void CController::fn_801D4EA4(u32 hold, u32 prevHold) {
    u32 trigRelease = prevHold ^ hold;
    u32 trig = trigRelease & hold;
    u32 release = trigRelease & prevHold;

    mButtonHold = 0;
    mButtonTrig = 0;
    mButtonRelease = 0;

    for (s32 i = 0; i < BUTTON_COUNT; i++) {
        u32 bit = 1 << i;
        if (mButtonCoolTimer[i] != 0) {
            if (gTickFlowManager->getPaused()) {
                continue;
            }
            mButtonCoolTimer[i]--;
            if ((mButtonCoolTimer[i] == 0) && (hold & bit)) {
                mButtonCoolTimer[i] = 1;
            }
        }
        else {
            if (hold & bit) {
                mButtonHold |= bit;
            }
            if (trig & bit) {
                mButtonTrig |= bit;
            }
            if (release & bit) {
                mButtonRelease |= bit;
                if (!gTickFlowManager->getPaused()) {
                    mButtonCoolTimer[i] = mButtonCoolFrames[i];
                }
            }
        }
    }
}

void CController::fn_801D4F74(u32 prevButtonHold) {
    unk1368 = 0;
    if (mButtonTrig != 0) {
        mHeldTimer = 0;
    }
    if (prevButtonHold == mButtonHold) {
        mHeldTimer++;
        if ((mHeldTimer == unk1364) || (mHeldTimer == (unk1364 + unk1365))) {
            unk1368 = mButtonHold;
            mHeldTimer = unk1364;
        }
    }
}

void CController::fn_801D4FD8(void) {
    unk136D = unk136C;

    if (!unk136D) {
        if ((unk1370 - 1) > 1) { // TODO: maybe fake?
            if (unk1370 == 0) {
                // TODO: this is definitely an inline
                u32 temp = mButtonTrig;
                if ((((temp >> 11) & 1) != 0) && ((temp & WPAD_BUTTON_B) != 0)) {
                    unk1370 = 3;
                    unk136C = true;
                }
                else if (((temp >> 11) & 1) != 0) {
                    unk1370 = 1;
                    unk1374 = 0;
                }
                else if ((temp & WPAD_BUTTON_B) != 0) {
                    unk1370 = 2;
                    unk1374 = 0;
                }
            }
            return;
        }
        unk1374++;
        if (unk1374 >= 4) {
            unk1370 = 0;
        }
        else if (((mButtonTrig & WPAD_BUTTON_A) != 0) || ((mButtonTrig & WPAD_BUTTON_B) != 0)) {
            unk1370 = 3;
            unk136C = true;
        }
        return;
    }

    if ((unk1370 - 1) > 1) { // TODO: maybe fake?
        if ((s32)unk1370 == 3) {
            // TODO: this is definitely an inline
            u32 temp = mButtonRelease;
            if ((((temp >> 11) & 1) != 0) && ((temp & WPAD_BUTTON_B) != 0)) {
                unk1370 = 0;
                unk136C = false;
            }
            else if (((temp >> 11) & 1) != 0) {
                unk1370 = 2;
                unk1374 = 0;
            }
            else if ((temp & WPAD_BUTTON_B) != 0) {
                unk1370 = 1;
                unk1374 = 0;
            }
        } 
        return;
    }

    unk1374++;
    if (unk1374 >= 4) {
        unk1370 = 0;
        unk136D = false;
        unk136C = false;
    }
    else if (((mButtonRelease & WPAD_BUTTON_A) != 0) || ((mButtonRelease & WPAD_BUTTON_B) != 0)) {
        unk1370 = 0;
        unk136C = false;
    }
}

void CController::fn_801D5170(bool arg1) {
    if (arg1) {
        KPADSetPosParam(mMyChannel, .05f, .8f);
        if (mCoreStatus[0].speed < .01f) {
            KPADSetPosParam(mMyChannel, .05f, .13f);
        }
    }
    else {
        KPADSetPosParam(mMyChannel, .0f, 1.0f);
    }
}

Vec2 CController::fn_801D51E4(f32 width, f32 height) {
    return (Vec2) {
        getCorePos().x * width / 2.0f,
        getCorePos().y * height / 2.0f
    };
}

Vec2 CController::fn_801D523C(CLayout *layout) {
    nw4r::ut::Rect rect = layout->getLayout()->GetLayoutRect();
    return fn_801D51E4(rect.right - rect.left, rect.bottom - rect.top);
}

bool CController::fn_801D52D4(void) {
    bool out = false;
    if (_24() && (mKPADReadLength > 0) && (mCoreStatus[0].dpd_valid_fg)) {
        out = true;
    }
    return out;
}

bool CController::fn_801D5340(void) {
    // TODO: maybe inlines here?
    bool temp_r30 = false;
    bool temp_r29 = false;
    bool temp_r28 = false;
    bool temp_r27 = false;
    if (fn_801D52D4()) {
        Vec2 a = mCoreStatus[0].pos;
        if (-1.0f <= a.x) {
            temp_r27 = true;
        }
    }
    if (temp_r27) {
        Vec2 a = mCoreStatus[0].pos;
        if (a.x <= 1.0f) {
            temp_r28 = true;
        }
    }
    if (temp_r28) {
        Vec2 a = mCoreStatus[0].pos;
        if (-1.0f <= a.y) {
            temp_r29 = true;
        }
    }
    if (temp_r29) {
        Vec2 a = mCoreStatus[0].pos;
        if (a.y <= 1.0f) {
            temp_r30 = true;
        }
    }
    return temp_r30;
}

f32 CController::fn_801D547C(void) {
    static Vec2 lbl_80320F90;

    if (_24() && (mKPADReadLength > 0)) {
        lbl_80320F90 = mCoreStatus[0].horizon;
    }
    Vec2 temp = lbl_80320F90;
    return 1.40625f * nw4r::math::Atan2FIdx(temp.y, temp.x);
}

void CController::fn_801D5500(u32 button, u8 frames) {
    for (s32 i = 0; i < BUTTON_COUNT; i++) {
        if (button & (1 << i)) {
            mButtonCoolFrames[i] = frames;
        }
    }
}

void CController::fn_801D55D8(u32 button, u8 frames) {
    for (s32 i = 0; i < BUTTON_COUNT; i++) {
        if (button & (1 << i)) {
            mButtonCoolTimer[i] = frames;
        }
    }
}

void CController::_40(const char *seqText, bool forcePlay) {
    if (!mMotorSeqPlaying || forcePlay) {
        u32 seqLen = strlen(seqText);

        mMotorSeqPlaying = true;
        mMotorSeqPos = 0;

        for (s32 i = 0; i < seqLen; i++) {
            if (seqText[i] == '*') {
                mMotorSeq[i] = MOTOR_SEQ_ON;
            }
            else if (seqText[i] == '-') {
                mMotorSeq[i] = MOTOR_SEQ_OFF;
            }
        }
        mMotorSeq[seqLen] = MOTOR_SEQ_END;

        _4C();
    }
}

void CController::_44(void) {
    mMotorSeqPlaying = false;
    WPADControlMotor(mMyChannel, FALSE);
}

void CController::_4C(void) {
    if (mMotorSeqPlaying) {
        if (mMotorSeq[mMotorSeqPos] == MOTOR_SEQ_ON) {
            WPADControlMotor(mMyChannel, TRUE);
            mMotorSeqPos++;
        }
        else if (mMotorSeq[mMotorSeqPos] == MOTOR_SEQ_OFF) {
            WPADControlMotor(mMyChannel, FALSE);
            mMotorSeqPos++;
        }
        else if (mMotorSeq[mMotorSeqPos] == MOTOR_SEQ_END) {
            WPADControlMotor(mMyChannel, FALSE);
            mMotorSeqPlaying = false;
        }
    }
}

void CController::fn_801D5830(s32 channel, s32 result) {
    CController *controller = gControllerManager->fn_801D5FF0(channel);
    controller->mInfoErrcode = result;
    controller->mInfoUpdated = false;
}

bool CController::fn_801D5850(void) {
    mInfoUpdated = true;
    mInfoErrcode = WPADGetInfoAsync(mMyChannel, &mInfo, CController::fn_801D5830);
    return mInfoErrcode == WPAD_ERR_OK;
}

bool CController::fn_801D58A0(void) {
    return mInfoUpdated;
}

s32 CController::fn_801D58A8(void) {
    return (mInfoErrcode == WPAD_ERR_OK) ? mInfo.battery : mInfoErrcode; 
}

void CGCController::_0C(void) {
    if (mStatus->err != PAD_ERR_NONE) {
        return;
    }

    u16 prevButton = mStatusPrev->button;
    unk14 = 0;

    if (PADButtonDown(mStatusPrev->button, mStatus->button) != 0) {
        mHeldTimer = 0;
    }

    if (prevButton == mStatus->button) {
        mHeldTimer++;
        if ((mHeldTimer == unk10) || (mHeldTimer == (unk10 + unk11))) {
            unk14 = mStatus->button;
            mHeldTimer = unk10;
        }
    }
}

/* 
        CControllerManager
*/

void *CControllerManager::fn_801D5950(u32 size) {
    return gControllerManager->doAlloc(size);
}

BOOL CControllerManager::fn_801D59B0(void *block) {
    return gControllerManager->doFree(block);
}

CControllerManager::CControllerManager(void) {}

CControllerManager::~CControllerManager(void) {
    _08();
}

void CControllerManager::_10(CController::CreateFn createFn) {
    for (s32 i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mController[i] = createFn(i);
    }
    mNullController = new CNullController(-1);

    u32 heapSize = WPADGetWorkMemorySize() + 0x100;
    mHeapStart = new (eHeap_MEM2, 32) u8[heapSize];
    mHeap = MEMCreateExpHeap(mHeapStart, heapSize);
    MEMInitAllocatorForExpHeap(&mAllocator, mHeap, 32);

    for (s32 i = 0; i < PAD_MAX_CONTROLLERS; i++) {
        mGCController[i] = new CGCController(i);
        mGCController[i]->setStatus(&mPadStatus[i]);
        mGCController[i]->setStatusPrev(&mPadStatusPrev[i]);
    }
}

void CControllerManager::_08(void) {
    for (s32 i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        delete mController[i];
    }
    delete mNullController;

    MEMDestroyExpHeap(mHeap);
    delete[] mHeapStart;
}

void CControllerManager::_14(void) {
    WPADRegisterAllocator(CControllerManager::fn_801D5950, CControllerManager::fn_801D59B0);
    KPADInit();

    for (s32 i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mController[i]->_0C();
    }

    PADInit();

    memset(mPadStatus, 0x00, sizeof(mPadStatus));
    memset(mPadStatusPrev, 0x00, sizeof(mPadStatusPrev));

    u8 pos = WPADGetSensorBarPosition();
    if (pos == 0) {
        KPADSetSensorHeight(0, -.1f);
        return;
    }
    else if (pos == 1) {
        KPADSetSensorHeight(0, .1f);
        return;
    }
}

void CControllerManager::_18(void) {
    for (s32 i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mController[i]->_10();
    }
    for (s32 i = 0; i < PAD_MAX_CONTROLLERS; i++) {
        mGCController[i]->_0C();
        mPadStatusPrev[i] = mPadStatus[i];
    }

    PADRead(mPadStatus);

    u32 resetMask = 0;
    for (s32 chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        if (mPadStatus[chan].err == PAD_ERR_NO_CONTROLLER) {
            resetMask |= (PAD_CHAN0_BIT >> chan);
        }
    }

    if (resetMask != 0) {
        PADReset(resetMask);
    }
}

void CControllerManager::_1C(void) {
    for (s32 i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mController[i]->_14();
    }
    for (s32 i = 0; i < PAD_MAX_CONTROLLERS; i++) {
        mGCController[i]->_10();
    }
}

CController *CControllerManager::fn_801D5FF0(s32 channel) {
    return mController[channel];
}

CGCController *CControllerManager::fn_801D6000(s32 channel) {
    return mGCController[channel];
}

CGCController::~CGCController(void) {}

bool CNullController::_3C(void) {
    return false;
}

bool CNullController::_38(void) {
    return false;
}

bool CNullController::_34(void) {
    return false;
}

bool CNullController::_30(void) {
    return false;
}

bool CNullController::_2C(void) {
    return false;
}

bool CNullController::_28(void) {
    return false;
}

bool CNullController::_24(void) {
    return false;
}

u8 CNullController::_18(void) {
    return WPAD_DEV_NOT_FOUND;
}
