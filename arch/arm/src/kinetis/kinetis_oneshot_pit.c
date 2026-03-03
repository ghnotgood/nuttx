/*****************************************************************************
 * arch/arm/src/kinetis/kinetis_oneshot_pit.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/*****************************************************************************
 * Periodic Interrupt Timer (PIT)
 *
 * From the Reference manual:
 *
 *  The PIT module is an array of timers that can be used to raise interrupts
 *  and trigger DMA channels.
 *
 * This file provides oneshot timer driver that uses Kinetis' PIT.
 *
 ****************************************************************************/

#include <nuttx/config.h>

#include <assert.h>
#include <debug.h>

#include <nuttx/irq.h>            /* Enter and leave critical sections. */
#include <nuttx/timers/oneshot.h> /* oneshot_lowerhalf_s */

#include "hardware/kinetis_sim.h" /* KINETIS_SIM_... */
#include "hardware/kinetis_pit.h" /* KINETIS_PIT_... */

#include "kinetis.h"

#if defined(CONFIG_KINETIS_PIT)

#if !defined(CONFIG_ONESHOT) || !defined(CONFIG_ONESHOT_COUNT)
#  error "CONFIG_ONESHOT and CONFIG_ONESHOT_COUNT must be defined"
#endif

/*****************************************************************************
 * Private Types
 ****************************************************************************/

struct kinetis_oneshot_lowerhalf_s
{
  struct oneshot_lowerhalf_s lh;

  int channel;
};

/*****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static clkcnt_t kinetis_oneshot_current(struct oneshot_lowerhalf_s *lower);
static void     kinetis_oneshot_start(struct oneshot_lowerhalf_s *lower,
                                      clkcnt_t delay_count);
static void     kinetis_oneshot_start_absolute(
                  struct oneshot_lowerhalf_s *lower, clkcnt_t at_count);
static void     kinetis_oneshot_cancel(struct oneshot_lowerhalf_s *lower);
static clkcnt_t kinetis_oneshot_maxdelay(struct oneshot_lowerhalf_s *lower);

/*****************************************************************************
 * Private Data
 ****************************************************************************/

static struct oneshot_operations_s const g_oneshot_ops =
{
  .current        = kinetis_oneshot_current,
  .start          = kinetis_oneshot_start,
  .start_absolute = kinetis_oneshot_start_absolute,
  .cancel         = kinetis_oneshot_cancel,
  .max_delay      = kinetis_oneshot_maxdelay,
};

#if defined(CONFIG_KINETIS_PIT_CH0)
static struct kinetis_oneshot_lowerhalf_s g_pit_ch0 =
{
  .lh =
    {
      .ops = &g_oneshot_ops,
    },
  .channel = 0,
};
#endif /* CONFIG_KINETIS_PIT_CH0 */

#if defined(CONFIG_KINETIS_PIT_CH1)
static struct kinetis_oneshot_lowerhalf_s g_pit_ch1 =
{
  .lh =
    {
      .ops = &g_oneshot_ops,
    },
  .channel = 1,
};
#endif /* CONFIG_KINETIS_PIT_CH1 */

#if defined(CONFIG_KINETIS_PIT_CH2)
static struct kinetis_oneshot_lowerhalf_s g_pit_ch2 =
{
  .lh =
    {
      .ops = &g_oneshot_ops,
    },
  .channel = 2,
};
#endif /* CONFIG_KINETIS_PIT_CH2 */

#if defined(CONFIG_KINETIS_PIT_CH3)
static struct kinetis_oneshot_lowerhalf_s g_pit_ch3 =
{
  .lh =
    {
      .ops = &g_oneshot_ops,
    },
  .channel = 3,
};
#endif /* CONFIG_KINETIS_PIT_CH3 */

/*****************************************************************************
 * Private Functions
 ****************************************************************************/

static clkcnt_t
kinetis_oneshot_current(struct oneshot_lowerhalf_s *lower)
{
  struct kinetis_oneshot_lowerhalf_s *priv =
    (struct kinetis_oneshot_lowerhalf_s *)lower;

  DEBUGASSERT(NULL != priv);

  switch (priv->channel)
    {
      case 0:
        return getreg32(KINETIS_PIT_CVAL0);
        break;
      case 1:
        return getreg32(KINETIS_PIT_CVAL1);
        break;
      case 2:
        return getreg32(KINETIS_PIT_CVAL2);
        break;
      case 3:
        return getreg32(KINETIS_PIT_CVAL3);
        break;
      default:
        tmrerr("ERROR: Bad channel %d", priv->channel);
        return 0;
    }
}

static void
kinetis_oneshot_start(struct oneshot_lowerhalf_s *lower, clkcnt_t delay_count)
{
  struct kinetis_oneshot_lowerhalf_s *priv =
    (struct kinetis_oneshot_lowerhalf_s *)lower;
  irqstate_t flags;

  DEBUGASSERT(NULL != priv);

  flags = enter_critical_section();
  switch (priv->channel)
    {
      case 0:
        putreg32(0x00000000, KINETIS_PIT_TCTRL0);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG0);
        putreg32((uint32_t)delay_count, KINETIS_PIT_LDVAL0);
        putreg32(PIT_TCTRL_TEN | PIT_TCTRL_TIE, KINETIS_PIT_TCTRL0);
        break;
      case 1:
        putreg32(0x00000000, KINETIS_PIT_TCTRL1);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG1);
        putreg32((uint32_t)delay_count, KINETIS_PIT_LDVAL1);
        putreg32(PIT_TCTRL_TEN | PIT_TCTRL_TIE, KINETIS_PIT_TCTRL1);
        break;
      case 2:
        putreg32(0x00000000, KINETIS_PIT_TCTRL2);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG2);
        putreg32((uint32_t)delay_count, KINETIS_PIT_LDVAL2);
        putreg32(PIT_TCTRL_TEN | PIT_TCTRL_TIE, KINETIS_PIT_TCTRL2);
        break;
      case 3:
        putreg32(0x00000000, KINETIS_PIT_TCTRL3);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG3);
        putreg32((uint32_t)delay_count, KINETIS_PIT_LDVAL3);
        putreg32(PIT_TCTRL_TEN | PIT_TCTRL_TIE, KINETIS_PIT_TCTRL3);
        break;
      default:
        tmrerr("ERROR: Bad channel 0 <= %d <= 3", priv->channel);
    }

  leave_critical_section(flags);
}

static void
kinetis_oneshot_start_absolute(struct oneshot_lowerhalf_s *lower,
                               clkcnt_t at_count)
{
  struct kinetis_oneshot_lowerhalf_s *priv =
    (struct kinetis_oneshot_lowerhalf_s *)lower;

  DEBUGASSERT(NULL != priv);
  UNUSED(priv);

  /* Copied from the esp_oneshot.c file. */

  uint32_t alarm   = (uint32_t)at_count;
  uint32_t counter = (uint32_t)kinetis_oneshot_current(lower);

  counter = alarm - counter >= alarm ? 0 : alarm - counter;
  kinetis_oneshot_start(lower, counter);
}

static void
kinetis_oneshot_cancel(struct oneshot_lowerhalf_s *lower)
{
  struct kinetis_oneshot_lowerhalf_s *priv =
    (struct kinetis_oneshot_lowerhalf_s *)lower;
  irqstate_t flags;

  DEBUGASSERT(NULL != priv);

  /* Reset PIT_TCTRLn and KINETIS_PIT_TFLGn. */

  flags = enter_critical_section();
  switch (priv->channel)
    {
      case 0:
        putreg32(0x00000000, KINETIS_PIT_TCTRL0);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG0);
        break;
      case 1:
        putreg32(0x00000000, KINETIS_PIT_TCTRL1);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG1);
        break;
      case 2:
        putreg32(0x00000000, KINETIS_PIT_TCTRL2);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG2);
        break;
      case 3:
        putreg32(0x00000000, KINETIS_PIT_TCTRL3);
        putreg32(PIT_TFLG_TIF, KINETIS_PIT_TFLG3);
        break;
      default:
        tmrerr("ERROR: Bad channel 0 <= %d <= 3", priv->channel);
    }

  leave_critical_section(flags);
}

static clkcnt_t
kinetis_oneshot_maxdelay(struct oneshot_lowerhalf_s *lower)
{
  struct kinetis_oneshot_lowerhalf_s *priv =
    (struct kinetis_oneshot_lowerhalf_s *)lower;

  DEBUGASSERT(NULL != priv);
  UNUSED(priv);

  /* PIT is 32-bit wide register, we cannot put in more. */

  return 0xffffffff;
}

/*****************************************************************************
 * Public Functions
 ****************************************************************************/

struct oneshot_lowerhalf_s *oneshot_initialize(int chan, uint16_t resolution)
{
  DEBUGASSERT(0 <= chan);
  DEBUGASSERT(chan <= 3);

  uint32_t reg;

  /* Enable PIT in SIM. */

  reg = getreg32(KINETIS_SIM_SCGC6);
  reg |= SIM_SCGC6_PIT;
  putreg32(reg, KINETIS_SIM_SCGC6);

  /* Enable PIT in PIT_MCR. */

  reg = getreg32(KINETIS_PIT_MCR);
  reg &= ~PIT_MCR_MDIS;
  putreg32(reg, KINETIS_PIT_MCR);

  UNUSED(resolution);

  switch (chan)
    {
      case 0:
#if defined(CONFIG_KINETIS_PIT_CH0)
        return &g_pit_ch0.lh;
#else
        return NULL;
#endif
      case 1:
#if defined(CONFIG_KINETIS_PIT_CH1)
        return &g_pit_ch1.lh;
#else
        return NULL;
#endif
      case 2:
#if defined(CONFIG_KINETIS_PIT_CH2)
        return &g_pit_ch2.lh;
#else
        return NULL;
#endif
      case 3:
#if defined(CONFIG_KINETIS_PIT_CH3)
        return &g_pit_ch3.lh;
#else
        return NULL;
#endif
    }

  return NULL;
}

#endif /* CONFIG_KINETIS_PIT */
