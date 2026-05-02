/*******************************************************************************
 * @file
 * @brief WED application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME "OPENTHREAD_SAMPLE_APP"

#include <assert.h>
#include <string.h>

#include <common/code_utils.hpp>
#include <common/logging.hpp>
#include <openthread/cli.h>
#include <openthread/dataset_ftd.h>
#include <openthread/instance.h>
#include <openthread/message.h>
#include <openthread/thread.h>
#include <openthread/platform/logging.h>

#include "sl_button.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

#include "sl_component_catalog.h"
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#include "sl_power_manager.h"
#endif

#ifdef SL_CATALOG_KERNEL_PRESENT
#include "sl_ot_rtos_adaptation.h"
#endif // SL_CATALOG_KERNEL_PRESENT

// Constants
#define WED_CSL_PERIOD_US 0    // Disable CSL on WED to begin with.
#define WED_CSL_TIMEOUT_SEC 20 // 20 seconds.

extern void otSysEventSignalPending(void);

// Variables
static bool sRxOnIdleButtonPressed = false;
static bool sAllowSleep            = false;
static bool sPrintState            = false;

void sleepyInit(void)
{
    otError error;

    otCliOutputFormat("sleepy-demo-wed starting in EM1 (idle) mode\r\n");
    otCliOutputFormat("Press Button 0 to toggle between EM2 (sleep) and EM1 (idle) modes\r\n");

    otCliOutputFormat("[csl period: %d us.] [csl timeout: %d sec.]\r\n", WED_CSL_PERIOD_US, WED_CSL_TIMEOUT_SEC);
    // SuccessOrExit(error = otLinkSetCslChannel(otInstanceGetSingle(), 15));
    SuccessOrExit(error = otLinkSetCslPeriod(otInstanceGetSingle(), WED_CSL_PERIOD_US));
    SuccessOrExit(error = otLinkSetCslTimeout(otInstanceGetSingle(), WED_CSL_TIMEOUT_SEC));

    /* Set the extended address to 0x12AB34CD56EF78AB */
    otExtAddress extAddress = {.m8 = {0x12, 0xAB, 0x34, 0xCD, 0x56, 0xEF, 0x78, 0xAB}};
    SuccessOrExit(error = otLinkSetExtendedAddress(otInstanceGetSingle(), &extAddress));

    /* Set the Link Mode configs */
    otLinkModeConfig config;
    config.mRxOnWhenIdle = 0;
    config.mDeviceType   = 0;
    config.mNetworkData  = 0;
    SuccessOrExit(error = otThreadSetLinkMode(otInstanceGetSingle(), config));

exit:
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("Initialization failed with: %d, %s\r\n", error, otThreadErrorToString(error));
    }
    return;
}

/*
 * Callback from sl_ot_is_ok_to_sleep to check if it is ok to go to sleep.
 */
bool efr32AllowSleepCallback(void)
{
    return sAllowSleep;
}

/*
 * Override default network settings, such as panid, so the devices can join a network
 */
void setNetworkConfiguration(void)
{
    static char          aNetworkName[] = "eCSL-EFR32";
    otError              error;
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Key, PSKc, Security Policy
     */
    aDataset.mActiveTimestamp.mSeconds             = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 25 */
    aDataset.mChannel                      = 25;
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Wake-up chanel to 11*/
    aDataset.mWakeupChannel                      = 11;
    aDataset.mComponents.mIsWakeupChannelPresent = true;

    /* Set Pan ID to 0xCDAB */
    aDataset.mPanId                      = (otPanId)0xCDAB;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xC1, 0xDE, 0x1A, 0xB5, 0xC0, 0xDE, 0x1A, 0xB6};
    memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    /* Set network key to 112233441AB51234C0DE1AB511223344 */
    uint8_t key[OT_NETWORK_KEY_SIZE] =
        {0x11, 0x22, 0x33, 0x44, 0x1A, 0xB5, 0x12, 0x34, 0xC0, 0xDE, 0x1A, 0xB5, 0x11, 0x22, 0x33, 0x44};
    memcpy(aDataset.mNetworkKey.m8, key, sizeof(aDataset.mNetworkKey));
    aDataset.mComponents.mIsNetworkKeyPresent = true;

    /* Set Network Name to 'eCSL-EFR32' */
    size_t length = strlen(aNetworkName);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, aNetworkName, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    /* Set the Active Operational Dataset to this dataset */
    error = otDatasetSetActive(otInstanceGetSingle(), &aDataset);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("otDatasetSetActive failed with: %d, %s\r\n", error, otThreadErrorToString(error));
        return;
    }
}

void sl_button_on_change(const sl_button_t *handle)
{
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
    {
        if (&sl_button_btn0 == handle)
        {
            sRxOnIdleButtonPressed = true;
        }
        otSysEventSignalPending();
    }
#ifdef SL_CATALOG_KERNEL_PRESENT
    sl_ot_rtos_set_pending_event(SL_OT_RTOS_EVENT_APP);
#endif
}

#ifdef SL_CATALOG_KERNEL_PRESENT
#define applicationTick sl_ot_rtos_application_tick
#endif

void applicationTick(void)
{
    if (sPrintState)
    {
        otCliOutputFormat("sleepy-demo-wed switching to %s mode\r\n", sAllowSleep ? "EM2 (sleep)" : "EM1 (idle)");
        sPrintState = false;
    }

    // Check for BTN0 button press
    if (sRxOnIdleButtonPressed)
    {
        sRxOnIdleButtonPressed = false;
        sAllowSleep            = !sAllowSleep;
        sPrintState            = true;

#if (defined(SL_CATALOG_KERNEL_PRESENT) && defined(SL_CATALOG_POWER_MANAGER_PRESENT))
        if (sAllowSleep)
        {
            sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
        }
        else
        {
            sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
        }
#endif
    }

    return;
}
