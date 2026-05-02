/*
 *  Copyright (c) 2024, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the Wake-up End Device of the subset of IEEE 802.15.4 MAC primitives.
 */

#include "sub_mac.hpp"

#if OPENTHREAD_CONFIG_WAKEUP_END_DEVICE_ENABLE

#include "instance/instance.hpp"

namespace ot {
namespace Mac {

RegisterLogModule("SubMac");

void SubMac::WedInit(void)
{
    mIsWedSampling        = false;
    mIsWedEnabled         = false;
    mWakeupListenInterval = 0;
    mWedTimer.Stop();
}

void SubMac::UpdateWakeupListening(bool aEnable, uint32_t aInterval, uint32_t aDuration, uint8_t aChannel)
{
    mWakeupListenInterval = aInterval;
    mWakeupListenDuration = aDuration;
    mWakeupChannel        = aChannel;
    mIsWedSampling        = false;
    mIsWedEnabled         = aEnable;

    mWedTimer.Stop();

    if (aEnable)
    {
        mWedSampleTime      = TimerMicro::GetNow() + kCslReceiveTimeAhead - mWakeupListenInterval;
        mWedSampleTimeRadio = Get<Radio>().GetNow() + kCslReceiveTimeAhead - mWakeupListenInterval;

        HandleWedTimer();
    }
    else if (!RadioSupportsReceiveTiming())
    {
        UpdateRadioSampleState();
    }
}

void SubMac::HandleWedTimer(Timer &aTimer) { aTimer.Get<SubMac>().HandleWedTimer(); }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
uint16_t SubMac::GetNeededShift(uint32_t aCslStart)
{
    // Apply margin to every edge
    uint32_t x1 = mWedSampleTime.GetValue() - kWakeupOverlapMargin;
    uint32_t x2 = mWedSampleTime.GetValue() + mWakeupListenDuration + kWakeupOverlapMargin;
    uint32_t y1 = aCslStart - kWakeupOverlapMargin;
    uint32_t y2 = aCslStart + mCslWinDur + kWakeupOverlapMargin;

    if (x2 < y1 || y2 < x1)
    {
        // WED:                                   x1-#####################################-x2
        // CSL:   y1-################-y2
        return 0;
    }
    else
    {
        // WED:              x1-#####################################-x2
        // CSL:   y1-################-y2
        //
        // WED:   x1-#####################################-x2
        // CSL:                                  y1-################-y2
        return y2 - x1;
    }
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

void SubMac::HandleWedTimer(void)
{
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // Shift WED receive slot if it overlaps with regular CSL receive windows

    // Shift will come either from the overlap with the currently scheduled CSL slot or with the next one, but
    // not from both (thus we can sum here assuming one is 0)
    uint16_t shift = GetNeededShift(mCslWinStart) + GetNeededShift(mCslWinStart + mCslPeriod * kUsPerTenSymbols);
    if (shift)
    {
        mWedSampleTime += shift;
        LogDebg("WED slot delayed %u us", shift);
    }
#endif

    if (RadioSupportsReceiveTiming())
    {
        HandleWedReceiveAt();
    }
    else
    {
        HandleWedReceiveOrSleep();
    }
}

void SubMac::HandleWedReceiveAt(void)
{
    mWedSampleTime += mWakeupListenInterval;
    mWedSampleTimeRadio += mWakeupListenInterval;
    mWedTimer.FireAt(mWedSampleTime + mWakeupListenDuration + kWedReceiveTimeAfter);

    if (mState != kStateDisabled)
    {
        IgnoreError(Get<Radio>().ReceiveAt(mWakeupChannel, static_cast<uint32_t>(mWedSampleTimeRadio),
                                           mWakeupListenDuration, kWakeupSlotId));
    }
}

void SubMac::HandleWedReceiveOrSleep(void)
{
    TimeMilli fireTime;

    mIsWedSampling = !mIsWedSampling;

    if (mIsWedSampling)
    {
        fireTime = mWedSampleTime + mWakeupListenDuration + kMinReceiveOnAfter;
    }
    else
    {
        mWedSampleTime += mWakeupListenInterval;
        fireTime = mWedSampleTime - kMinReceiveOnAhead;
    }

    mWedTimer.FireAt(fireTime);

    UpdateRadioSampleState();
}
} // namespace Mac
} // namespace ot

#endif // OPENTHREAD_CONFIG_WAKEUP_END_DEVICE_ENABLE
