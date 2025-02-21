/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/rmt_n_hal.h"
#include "hal/rmt_n_ll.h"
#include "soc/soc_caps.h"

void rmt_hal_init(rmt_hal_context_t *hal)
{
    hal->regs = (rmt_soc_handle_t)&RMT; //ALTER (CAST)
    rmt_ll_power_down_mem((rmt_dev_t*)hal->regs, false);               // turn on RMTMEM power domain //ALTER (CAST)
    rmt_ll_enable_mem_access_nonfifo((rmt_dev_t*)hal->regs, true);     // APB access the RMTMEM in nonfifo mode //ALTER (CAST)
    rmt_ll_enable_interrupt((rmt_dev_t*)hal->regs, UINT32_MAX, false); // disable all interupt events //ALTER (CAST)
    rmt_ll_clear_interrupt_status((rmt_dev_t*)hal->regs, UINT32_MAX);  // clear all pending events //ALTER (CAST)
#if SOC_RMT_SUPPORT_TX_SYNCHRO
    rmt_ll_tx_clear_sync_group((rmt_dev_t*)hal->regs); //ALTER (CAST)
#endif // SOC_RMT_SUPPORT_TX_SYNCHRO
}

void rmt_hal_deinit(rmt_hal_context_t *hal)
{
    rmt_ll_enable_interrupt((rmt_dev_t*)hal->regs, UINT32_MAX, false); // disable all interupt events //ALTER (CAST)
    rmt_ll_clear_interrupt_status((rmt_dev_t*)hal->regs, UINT32_MAX);  // clear all pending events //ALTER (CAST)
    rmt_ll_power_down_mem((rmt_dev_t*)hal->regs, true);                // turn off RMTMEM power domain //ALTER (CAST)
    hal->regs = NULL;
}

void rmt_hal_tx_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_tx_reset_channels_clock_div((rmt_dev_t*)hal->regs, 1 << channel); //ALTER (CAST)
    rmt_ll_tx_reset_pointer((rmt_dev_t*)hal->regs, channel); //ALTER (CAST)
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_tx_reset_loop_count((rmt_dev_t*)hal->regs, channel); //ALTER (CAST)
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_enable_interrupt((rmt_dev_t*)hal->regs, RMT_LL_EVENT_TX_MASK(channel), false); //ALTER (CAST)
    rmt_ll_clear_interrupt_status((rmt_dev_t*)hal->regs, RMT_LL_EVENT_TX_MASK(channel)); //ALTER (CAST)
}

void rmt_hal_rx_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_rx_reset_channels_clock_div((rmt_dev_t*)hal->regs, 1 << channel); //ALTER (CAST)
    rmt_ll_rx_reset_pointer((rmt_dev_t*)hal->regs, channel); //ALTER (CAST)
    rmt_ll_enable_interrupt((rmt_dev_t*)hal->regs, RMT_LL_EVENT_RX_MASK(channel), false); //ALTER (CAST)
    rmt_ll_clear_interrupt_status((rmt_dev_t*)hal->regs, RMT_LL_EVENT_RX_MASK(channel)); //ALTER (CAST)
}
