/*   ____           ____          _
//  |  _ \ __  __  / ___|___   __| | ___
//  | |_) |\ \/ / | |   / _ \ / _` |/ _ \
//  |  _ <  >  <  | |__| (_) | (_| |  __/
//  |_| \_\/_/\_\  \____\___/ \__,_|\___|
//
//  app.c
*/
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "sl_rail_util_init.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_sleeptimer.h"
#include "rail.h"

#define NUM_SYNC_WORD_BYTES (2U)
#define SYNC_WORD           (0xB16FU)
#define USE_EM4             true        // Else: uses EM3

static RAIL_Handle_t rail_handle;
bool awoken = false;

void RAILCb_SensedRf(void)
{
  if (!awoken)
    {
      awoken = true;
    }
}

static RAIL_RfSenseSelectiveOokConfig_t rfsense_config = {
    .band = RAIL_RFSENSE_2_4GHZ,
    //.band = RAIL_RFSENSE_2_4GHZ_LOW_SENSITIVITY,
   .syncWordNumBytes = NUM_SYNC_WORD_BYTES,
   .syncWord = SYNC_WORD,
   .cb = &RAILCb_SensedRf
};

void enable_rfsense(void)
{
  awoken = false;
  RAIL_StartSelectiveOokRfSense(rail_handle, &rfsense_config);
}

void app_init(void)
{
  // Abort Detection -- Stops execution of app, if button is held on startup
  if (sl_button_get_state(SL_SIMPLE_BUTTON_INSTANCE(0)) == SL_SIMPLE_BUTTON_PRESSED)
    {
      while (true) {
        sl_led_turn_on(&sl_led_led0);
        sl_sleeptimer_delay_millisecond(75);
        sl_led_turn_off(&sl_led_led0);
        sl_sleeptimer_delay_millisecond(75);
        sl_led_turn_on(&sl_led_led0);
        sl_sleeptimer_delay_millisecond(75);
        sl_led_turn_off(&sl_led_led0);
        sl_sleeptimer_delay_millisecond(1775);
      }
    }
  // Set up GPIO for outputting signal on wake
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);

  // Get RAIL handle, used later by the application
  rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  enable_rfsense();

  // Flash LED to indicate the loaded firmware, and system ready
  sl_led_turn_on(&sl_led_led0);
  sl_sleeptimer_delay_millisecond(500);
  sl_led_turn_off(&sl_led_led0);

}

void app_process_action(void)
{
  if (awoken)
    {
      GPIO_PinOutSet(gpioPortD, 3);
      sl_sleeptimer_delay_millisecond(1);
      GPIO_PinOutClear(gpioPortD, 3);

      enable_rfsense();
    }
  if (USE_EM4)
    {
      EMU_EnterEM4();
    } else {
      EMU_EnterEM3(false);
    }
}