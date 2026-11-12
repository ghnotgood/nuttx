/****************************************************************************
 * arch/arm/src/kinetis/kinetis_adc.c
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

/****************************************************************************
 * 16-bit Analog-to-Digital Converter (ADC)
 *
 * From the Reference Manual:
 *
 *  The 16-bit analog-to-digital converter (ADC) is a successive
 *  approximation ADC designed for operation within an integrated
 *  microcontroller system-on-chip.
 *
 * This file provides ADC driver for the Kinetis' 16-bit ADC.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <debug.h>
#include <nuttx/analog/adc.h>

#include "arm_internal.h"             /* putreg... */
#include "hardware/kinetis_adc.h"     /* KINETIS_ADC... */
#include "hardware/kinetis_sim.h"     /* SIM_... */
#include "hardware/kinetis_vrefv1.h"  /* VREF_... */

#if defined(CONFIG_KINETIS_ADC0) || defined(CONFIG_KINETIS_ADC1)

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  adc_bind(struct adc_dev_s *dev, struct adc_callback_s const *cb);
static void adc_reset(struct adc_dev_s *dev);
static int  adc_setup(struct adc_dev_s *dev);
static void adc_shutdown(struct adc_dev_s *dev);
static void adc_rxint(struct adc_dev_s *dev, bool enable);
static int  adc_ioctl(struct adc_dev_s *dev, int cmd, unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct adc_ops_s const g_adcops =
{
  .ao_bind     = adc_bind,
  .ao_reset    = adc_reset,
  .ao_setup    = adc_setup,
  .ao_shutdown = adc_shutdown,
  .ao_rxint    = adc_rxint,
  .ao_ioctl    = adc_ioctl,
};

#if defined(CONFIG_KINETIS_ADC0)
static struct adc_dev_s g_adc0_dev =
{
  .ad_ops  = &g_adcops,
  .ad_priv = NULL,
};
#endif

#if defined(CONFIG_KINETIS_ADC1)
static struct adc_dev_s g_adc1_dev =
{
  .ad_ops  = &g_adcops,
  .ad_priv = NULL,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int adc_bind(struct adc_dev_s *dev, struct adc_callback_s const *cb)
{
  UNUSED(dev);
  UNUSED(cb);

  awarn("Callbacks not implemented for Kinetis' ADC");
  return OK;
}

static void adc_reset(struct adc_dev_s *dev)
{
#if defined(CONFIG_KINETIS_ADC0)
  if (&g_adc0_dev == dev)
    {
    }
#endif /* defined(CONFIG_KINETIS_ADC0) */

#if defined(CONFIG_KINETIS_ADC1)
  if (&g_adc1_dev == dev)
    {
    }
#endif /* defined(CONFIG_KINETIS_ADC1) */
}

static int adc_setup(struct adc_dev_s *dev)
{
#if defined(CONFIG_KINETIS_ADC0)
  if (&g_adc0_dev == dev)
    {
    }
#endif /* defined(CONFIG_KINETIS_ADC0) */

#if defined(CONFIG_KINETIS_ADC1)
  if (&g_adc1_dev == dev)
    {
    }
#endif /* defined(CONFIG_KINETIS_ADC1) */
}

static void adc_shutdown(struct adc_dev_s *dev)
{
#if defined(CONFIG_KINETIS_ADC0)
  if (&g_adc0_dev == dev)
    {
    }
#endif /* defined(CONFIG_KINETIS_ADC0) */

#if defined(CONFIG_KINETIS_ADC1)
  if (&g_adc1_dev == dev)
    {
    }
#endif /* defined(CONFIG_KINETIS_ADC1) */
}

static void adc_rxint(struct adc_dev_s *dev, bool enable)
{
  UNUSED(dev);
  UNUSED(enable);

  awarn("Interrupts not implemented for Kinetis' ADC");
}

static int adc_ioctl(struct adc_dev_s *dev, int cmd, unsigned long arg)
{
  UNUSED(dev);
  UNUSED(cmd);
  UNUSED(arg);
  return -ENOTTY;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

struct adc_dev_s *kinetis_adc_initialize(int which)
{
  switch (which)
    {
      case 0:
#if defined(CONFIG_KINETIS_ADC0)
        return &g_adc0_dev;
#else
        awarn("CONFIG_KINETIS_ADC0 must be defined");
        return NULL;
#endif
        break;
      case 1:
#if defined(CONFIG_KINETIS_ADC1)
        return &g_adc1_dev;
#else
        awarn("CONFIG_KINETIS_ADC1 must be defined");
        return NULL;
#endif
        break;
      default:
        awarn("Bad device specified, must be 0 <= %d <= 1", which);
    }

  return NULL;
}

#endif /* defined(CONFIG_KINETIS_ADC0) || defined(CONFIG_KINETIS_ADC1) */
