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
 *   This file implements the CSL receiver of the subset of IEEE 802.15.4 MAC primitives.
 */

#include "sub_mac.hpp"

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#include "instance/instance.hpp"
#if OPENTHREAD_CONFIG_WAKEUP_END_DEVICE_ENABLE
#include "thread/mle.hpp"
#endif

namespace ot {
namespace Mac {

RegisterLogModule("SubMac");

void SubMac::CslInit(void)
{
    mCslPeriod    = 0;
    mCslChannel   = 0;
    mCslPeerShort = 0;
    mCslPeerExt.Clear();
    mIsCslSampling      = false;
    mCslSampleTimeRadio = 0;
    mCslSampleTimeLocal.SetValue(0);
#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
    mWedPresent = false;
#endif
    mCslLastSync.SetValue(0);
    mCslTimer.Stop();
}

void SubMac::RestartCslTimerAfterSyncUpdate(void)
{
#if OPENTHREAD_CONFIG_WAKEUP_END_DEVICE_ENABLE
    // Preserve existing wake-up attach/session timing while a WC session is active.
    // Re-arming the CSL timer on each sync update can destabilize WC/WED timing.
    if (Get<Mle::Mle>().IsWakeupCoordinatorPresent())
    {
        return;
    }
#endif

    // Only applies for the case where radio supports receive timing.
    if (RadioSupportsReceiveTiming() && mCslTimer.IsRunning())
    {
        uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;

        mCslTimer.Stop();

        // Rewind sample times by one period. HandleCslTimer() will add this
        // period back, effectively re-evaluating the current CSL period's
        // schedule using the updated mCslLastSync.
        mCslSampleTimeRadio -= periodUs;
        mCslSampleTimeLocal -= periodUs;

        HandleCslTimer();
    }
}

void SubMac::UpdateCslLastSyncTimestamp(TxFrame &aFrame, RxFrame *aAckFrame)
{
    // Actual synchronization timestamp should be from the sent frame instead of the current time.
    // Assuming the error here since it is bounded and has very small effect on the final window duration.
    if (aAckFrame != nullptr && aFrame.HasCslIe())
    {
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_LOCAL_TIME_SYNC
        mCslLastSync = TimerMicro::GetNow();
#else
        // Calculate transmitted frame timestamp based on ACK timestamp
        mCslLastSync = TimeMicro(static_cast<uint32_t>(aAckFrame->mInfo.mRxInfo.mTimestamp));
        mCslLastSync -= kAifsDuration;
        mCslLastSync -= aFrame.GetLength() * kOctetDuration;
#endif
    }

    RestartCslTimerAfterSyncUpdate();
}

void SubMac::UpdateCslLastSyncTimestamp(RxFrame *aFrame, Error aError)
{
    VerifyOrExit(aFrame != nullptr && aError == kErrorNone);

#if OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
    LogReceived(aFrame);
#endif

    // Assuming the risk of the parent missing the Enh-ACK in favor of smaller CSL receive window
    if (((mCslPeriod > 0) && aFrame->mInfo.mRxInfo.mAckedWithSecEnhAck)
#if OPENTHREAD_CONFIG_WAKEUP_END_DEVICE_ENABLE
        || (aFrame->GetHeaderIe(CstIe::kHeaderIeId) != nullptr)
#endif
    )
    {
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_LOCAL_TIME_SYNC
        mCslLastSync = TimerMicro::GetNow();
#else
        mCslLastSync = TimeMicro(static_cast<uint32_t>(aFrame->mInfo.mRxInfo.mTimestamp));
#endif
    }

    RestartCslTimerAfterSyncUpdate();

exit:
    return;
}

void SubMac::CslSample(void)
{
#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
    VerifyOrExit(!mRadioFilterEnabled, IgnoreError(Get<Radio>().Sleep()));
#endif

    SetState(kStateRadioSample);

    if (mIsCslSampling && !RadioSupportsReceiveTiming())
    {
        IgnoreError(Get<Radio>().Receive(mCslChannel));
        ExitNow();
    }

#if !OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
    IgnoreError(Get<Radio>().Sleep()); // Don't actually sleep for debugging
#endif

exit:
    return;
}

bool SubMac::UpdateCsl(uint16_t          aPeriod,
                       uint8_t           aChannel,
                       ShortAddress      aShortAddr,
                       const ExtAddress &aExtAddr,
                       uint32_t         &aSampleTime)
{
    bool diffPeriod  = aPeriod != mCslPeriod;
    bool diffChannel = aChannel != mCslChannel;
    bool diffShort   = aShortAddr != mCslPeerShort;
    bool diffExt     = aExtAddr != mCslPeerExt;
    bool diffTime    = aSampleTime != 0;
    bool retval      = diffPeriod || diffChannel || diffShort || diffExt || diffTime;

    VerifyOrExit(retval);
    mCslChannel = aChannel;

    VerifyOrExit(diffPeriod || diffShort || diffExt || diffTime);
    mCslPeriod    = aPeriod;
    mCslPeerShort = aShortAddr;
    mCslPeerExt   = aExtAddr;
    IgnoreError(Get<Radio>().EnableCsl(aPeriod, aShortAddr, aExtAddr));
#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
    // TODO: Rethink API for enabling CST. For now, enable it with CSL.
    if (mWedPresent)
    {
        IgnoreError(Get<Radio>().EnableCst(aPeriod, aShortAddr, &aExtAddr));
    }
#endif

    // Only modify the sample time when CSL period or sample time change
    VerifyOrExit(diffPeriod || diffTime);

    mIsCslSampling = false;
    mCslPeriod     = aPeriod;

    mCslTimer.Stop();
    mIsCslSampling = false;

    if (mCslPeriod > 0)
    {
        uint32_t  radioNow = static_cast<uint32_t>(Get<Radio>().GetNow());
        TimeMicro localNow = TimerMicro::GetNow();

        // If the caller provides a sample time, retain that phase alignment and
        // schedule the timer for the next valid sampling window.
        if (aSampleTime != 0)
        {
            uint32_t timeAhead;
            uint32_t timeAfter;
            uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;

            OT_UNUSED_VARIABLE(timeAfter);
            GetCslWindowEdges(timeAhead, timeAfter);
            mCslSampleTimeRadio =
                (TimeMicro::SoonestPeriodicEvent(TimeMicro(radioNow), TimeMicro(aSampleTime) - timeAhead, periodUs) +
                 timeAhead)
                    .GetValue();
        }
        else
        {
            mCslSampleTimeRadio = radioNow;
        }

        // Translate radio-domain sample time to local-domain sample time for timer scheduling.
        if (mCslSampleTimeRadio >= radioNow)
        {
            mCslSampleTimeLocal = localNow + (mCslSampleTimeRadio - radioNow);
        }
        else
        {
            mCslSampleTimeLocal = localNow - (radioNow - mCslSampleTimeRadio);
        }
        mIsCslSampling = false;

        if (aSampleTime != 0)
        {
            uint32_t timeAhead;
            uint32_t timeAfter;

            OT_UNUSED_VARIABLE(timeAfter);
            GetCslWindowEdges(timeAhead, timeAfter);

            Get<Radio>().UpdateCslSampleTime(mCslSampleTimeRadio);
#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
            // Keep CST phase aligned with CSL during startup when WED is present.
            if (mWedPresent)
            {
                uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;
                Get<Radio>().UpdateCstSampleTime(mCslSampleTimeRadio + periodUs / 2);
            }
#endif
            mCslTimer.Start(mCslSampleTimeLocal - timeAhead - localNow);
        }
        else
        {
            HandleCslTimer();
        }
    }
    else if (!RadioSupportsReceiveTiming())
    {
        UpdateRadioSampleState();
    }

exit:
    aSampleTime = mCslSampleTimeRadio;
    return retval;
}

void SubMac::HandleCslTimer(Timer &aTimer) { aTimer.Get<SubMac>().HandleCslTimer(); }

void SubMac::HandleCslTimer(void)
{
    uint32_t timeAhead, timeAfter;

    GetCslWindowEdges(timeAhead, timeAfter);

    // The handler works in different ways when the radio supports receive-timing and doesn't.
    if (RadioSupportsReceiveTiming())
    {
        HandleCslReceiveAt(timeAhead, timeAfter);
    }
    else
    {
        HandleCslReceiveOrSleep(timeAhead, timeAfter);
    }
}

void SubMac::HandleCslReceiveAt(uint32_t aTimeAhead, uint32_t aTimeAfter)
{
    /*
     * When the radio supports receive-timing:
     *   The handler will be called once per CSL period. When the handler is called, it will set the timer to
     *   fire at the next CSL sample time and call `Radio::ReceiveAt` to start sampling for the current CSL period.
     *   The timer fires some time before the actual sample time. After `Radio::ReceiveAt` is called, the radio will
     *   remain in sleep state until the actual sample time.
     *   Note that it never call `Radio::Sleep` explicitly. The radio will fall into sleep after `ReceiveAt` ends. This
     *   will be done by the platform as part of the `otPlatRadioReceiveAt` API.
     *
     *   Timer fires                                         Timer fires
     *       ^                                                    ^
     *       x-|------------|-------------------------------------x-|------------|---------------------------------------|
     *            sample                   sleep                        sample                    sleep
     */
    uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;
    uint32_t winStart;
    uint32_t winDuration;

    mCslTimer.FireAt(mCslSampleTimeLocal - aTimeAhead + periodUs);
    aTimeAhead -= kCslReceiveTimeAhead;
    winStart    = mCslSampleTimeRadio - aTimeAhead;
    winDuration = aTimeAhead + aTimeAfter;

    mCslSampleTimeRadio += periodUs;
    mCslSampleTimeLocal += periodUs;

    Get<Radio>().UpdateCslSampleTime(mCslSampleTimeRadio);
#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
    // TODO: Rethink API for configuring CST. For now, set CST sample time to CSL sample time + period/2
    if (mWedPresent)
    {
        Get<Radio>().UpdateCstSampleTime(mCslSampleTimeRadio + periodUs / 2);
    }
#endif

    // Schedule reception window for any state except RX - so that CSL RX Window has lower priority
    // than scanning or RX after the data poll.
    if ((mState != kStateDisabled) && (mState != kStateReceive))
    {
        mCslWinStart = winStart;
        mCslWinDur   = winDuration;
        IgnoreError(Get<Radio>().ReceiveAt(mCslChannel, winStart, winDuration, kCslSlotId));
    }

    LogCslWindow(winStart, winDuration);
}

void SubMac::HandleCslReceiveOrSleep(uint32_t aTimeAhead, uint32_t aTimeAfter)
{
    /*
     * When the radio doesn't support receive-timing:
     *   The handler will be called twice per CSL period: at the beginning of sample and sleep. When the handler is
     *   called, it will explicitly change the radio state due to the current state by calling `Radio::Receive` or
     *   `Radio::Sleep`.
     *
     *   Timer fires  Timer fires                            Timer fires  Timer fires
     *       ^            ^                                       ^            ^
     *       |------------|---------------------------------------|------------|---------------------------------------|
     *          sample                   sleep                        sample                    sleep
     */
#if OPENTHREAD_CONFIG_WAKEUP_END_DEVICE_ENABLE
    // For non-WC sessions, use legacy CSL sample/sleep arbitration behavior.
    if (!Get<Mle::Mle>().IsWakeupCoordinatorPresent())
#endif
    {
        if (mIsCslSampling)
        {
            mIsCslSampling = false;
            mCslTimer.FireAt(mCslSampleTimeLocal - aTimeAhead);
            if (mState == kStateRadioSample)
            {
#if !OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
                IgnoreError(Get<Radio>().Sleep()); // Don't actually sleep for debugging
#endif
                LogDebg("CSL sleep %lu", ToUlong(mCslTimer.GetNow().GetValue()));
            }
        }
        else
        {
            uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;
            uint32_t winStart;
            uint32_t winDuration;

            mCslTimer.FireAt(mCslSampleTimeLocal + aTimeAfter);
            mIsCslSampling = true;
            winStart       = TimerMicro::GetNow().GetValue();
            winDuration    = aTimeAhead + aTimeAfter;

            mCslSampleTimeRadio += periodUs;
            mCslSampleTimeLocal += periodUs;

            Get<Radio>().UpdateCslSampleTime(mCslSampleTimeRadio);
#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
            // TODO: Rethink API for configuring CST. For now, set CST sample time to CSL sample time + period/2
            if (mWedPresent)
            {
                Get<Radio>().UpdateCstSampleTime(mCslSampleTimeRadio + periodUs / 2);
            }
#endif
            if (mState == kStateRadioSample)
            {
                IgnoreError(Get<Radio>().Receive(mCslChannel));
            }

            LogCslWindow(winStart, winDuration);
        }

        return;
    }

    if (mIsCslSampling)
    {
        mIsCslSampling = false;
        mCslTimer.FireAt(mCslSampleTimeLocal - aTimeAhead);
        if (mState == kStateRadioSample)
        {
            LogDebg("CSL sleep %lu", ToUlong(mCslTimer.GetNow().GetValue()));
        }
    }
    else
    {
        uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;
        uint32_t winStart;
        uint32_t winDuration;

        mCslTimer.FireAt(mCslSampleTimeLocal + aTimeAfter);
        mIsCslSampling = true;
        winStart       = TimerMicro::GetNow().GetValue();
        winDuration    = aTimeAhead + aTimeAfter;

        mCslSampleTimeRadio += periodUs;
        mCslSampleTimeLocal += periodUs;

        Get<Radio>().UpdateCslSampleTime(mCslSampleTimeRadio);
#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
        // TODO: Rethink API for configuring CST. For now, set CST sample time to CSL sample time + period/2
        if (mWedPresent)
        {
            Get<Radio>().UpdateCstSampleTime(mCslSampleTimeRadio + periodUs / 2);
        }
#endif

        LogCslWindow(winStart, winDuration);
    }

    UpdateRadioSampleState();
}

void SubMac::GetCslWindowEdges(uint32_t &aAhead, uint32_t &aAfter)
{
    /*
     * CSL sample timing diagram
     *    |<---------------------------------Sample--------------------------------->|<--------Sleep--------->|
     *    |                                                                          |                        |
     *    |<--Ahead-->|<--UnCert-->|<--Drift-->|<--Drift-->|<--UnCert-->|<--MinWin-->|                        |
     *    |           |            |           |           |            |            |                        |
     * ---|-----------|------------|-----------|-----------|------------|------------|----------//------------|---
     * -timeAhead                           CslPhase                             +timeAfter             -timeAhead
     */
    uint32_t semiPeriod = mCslPeriod * kUsPerTenSymbols / 2;
    uint32_t curTime, elapsed, semiWindow;

    curTime = GetLocalTime();
    elapsed = curTime - mCslLastSync.GetValue();

    semiWindow =
        static_cast<uint32_t>(static_cast<uint64_t>(elapsed) *
                              (Get<Radio>().GetCslAccuracy() + mCslParentAccuracy.GetClockAccuracy()) / 1000000);
    semiWindow += mCslParentAccuracy.GetUncertaintyInMicrosec() + Get<Radio>().GetCslUncertainty() * 10;

    aAhead = Min(semiPeriod, semiWindow + kMinReceiveOnAhead + kCslReceiveTimeAhead);
    aAfter = Min(semiPeriod, semiWindow + kMinReceiveOnAfter);
}

uint32_t SubMac::GetLocalTime(void)
{
    uint32_t now;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_LOCAL_TIME_SYNC
    now = TimerMicro::GetNow().GetValue();
#else
    now = static_cast<uint32_t>(Get<Radio>().GetNow());
#endif

    return now;
}

#if OPENTHREAD_CONFIG_WAKEUP_COORDINATOR_ENABLE
void SubMac::WedPresent(bool aPresent) { mWedPresent = aPresent; }
#endif

#if OT_SHOULD_LOG_AT(OT_LOG_LEVEL_DEBG)
void SubMac::LogCslWindow(uint32_t aWinStart, uint32_t aWinDuration)
{
    LogDebg("CSL window start %lu, duration %lu", ToUlong(aWinStart), ToUlong(aWinDuration));
}
#else
void SubMac::LogCslWindow(uint32_t, uint32_t) {}
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
void SubMac::LogReceived(RxFrame *aFrame)
{
    static constexpr uint8_t kLogStringSize = 72;

    String<kLogStringSize> logString;
    Address                dst;
    int32_t                deviation;
    uint32_t               sampleTime, ahead, after;

    IgnoreError(aFrame->GetDstAddr(dst));

    VerifyOrExit((dst.GetType() == Address::kTypeShort && dst.GetShort() == GetShortAddress()) ||
                 (dst.GetType() == Address::kTypeExtended && dst.GetExtended() == GetExtAddress()));

    LogDebg("Received frame in state (SubMac %s, CSL %s), timestamp %lu", StateToString(mState),
            mIsCslSampling ? "CslSample" : "CslSleep",
            ToUlong(static_cast<uint32_t>(aFrame->mInfo.mRxInfo.mTimestamp)));

    VerifyOrExit(mState == kStateRadioSample);

    GetCslWindowEdges(ahead, after);
    ahead -= kMinReceiveOnAhead + kCslReceiveTimeAhead;

    sampleTime = mCslSampleTimeRadio - mCslPeriod * kUsPerTenSymbols;
    deviation  = static_cast<uint32_t>(aFrame->mInfo.mRxInfo.mTimestamp) + kRadioHeaderPhrDuration - sampleTime;

    // This logs three values (all in microseconds):
    // - Absolute sample time in which the CSL receiver expected the MHR of the received frame.
    // - Allowed margin around that time accounting for accuracy and uncertainty from both devices.
    // - Real deviation on the reception of the MHR with regards to expected sample time. This can
    //   be due to clocks drift and/or CSL Phase rounding error.
    // This means that a deviation absolute value greater than the margin would result in the frame
    // not being received out of the debug mode.
    logString.Append("Expected sample time %lu, margin ±%lu, deviation %ld", ToUlong(sampleTime), ToUlong(ahead),
                     static_cast<long>(deviation));

    // Treat as a warning when the deviation is not within the margins. Neither kCslReceiveTimeAhead
    // or kMinReceiveOnAhead/kMinReceiveOnAfter are considered for the margin since they have no
    // impact on understanding possible deviation errors between transmitter and receiver. So in this
    // case only `ahead` is used, as an allowable max deviation in both +/- directions.
    if ((deviation + ahead > 0) && (deviation < static_cast<int32_t>(ahead)))
    {
        LogDebg("%s", logString.AsCString());
    }
    else
    {
        LogWarn("%s", logString.AsCString());
    }

exit:
    return;
}
#endif

} // namespace Mac
} // namespace ot

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
