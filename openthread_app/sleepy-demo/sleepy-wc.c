/*******************************************************************************
 * @file
 * @brief WC application logic.
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

#include "sl_component_catalog.h"

#ifdef SL_CATALOG_KERNEL_PRESENT
#include "sl_ot_rtos_adaptation.h"
#endif // SL_CATALOG_KERNEL_PRESENT

// Constants
#define CSL_PERIOD_US 40000

void sleepyInit(void)
{
    otError error;

    // Set the CSL period on Wake-up coordinator. The same CSL period will be
    // delivered to WED using CST IE.
    SuccessOrExit(error = otLinkSetCslPeriod(otInstanceGetSingle(), CSL_PERIOD_US));

    /* Configure as WC*/
    otLinkModeConfig config;
    config.mRxOnWhenIdle = 0;
    config.mDeviceType   = 1;
    config.mNetworkData  = 1;
    SuccessOrExit(error = otThreadSetLinkMode(otInstanceGetSingle(), config));

    otCliOutputFormat("sleepy-demo-wc started\r\n");

exit:
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("Initialization failed with: %d, %s\r\n", error, otThreadErrorToString(error));
    }
    return;
}

/**
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

void initUdp(void)
{
    return;
}

#ifdef SL_CATALOG_KERNEL_PRESENT
#define applicationTick sl_ot_rtos_application_tick
#endif

void applicationTick(void)
{
    return;
}
