/*   _____         ____          _
//  |_   _|_  __  / ___|___   __| | ___
//    | | \ \/ / | |   / _ \ / _` |/ _ \
//    | |  >  <  | |__| (_) | (_| |  __/
//    |_| /_/\_\  \____\___/ \__,_|\___|
//
//  app.c
*/
#include "sl_rail_util_init.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_sleeptimer.h"
#include "rail.h"
#include "em_cmu.h"
#include "em_gpio.h"

#define NUM_SYNC_WORD_BYTES (2U)
#define SYNC_WORD           (0xB16FU)
#define USE_EM4             true        // Else: uses EM3
#define GPIO_PORT gpioPortB
#define GPIO_PIN  3

static RAIL_Handle_t rail_handle;
bool TxRequested = false;

void setupRisingEdgeInterrupt(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PA3 as input
  GPIO_PinModeSet(GPIO_PORT, GPIO_PIN, gpioModeInputPull, 0);

  // Enable rising edge interrupt on PA3
  GPIO_ExtIntConfig(GPIO_PORT, GPIO_PIN, GPIO_PIN, true, false, true);

  // Enable GPIO interrupt
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  // Enable interrupt for PA3
  GPIO_IntEnable(1 << GPIO_PIN);
}

// GPIO odd interrupt handler
void GPIO_ODD_IRQHandler(void)
{
  // Clear the interrupt flag
  uint32_t flags = GPIO_IntGet();
  GPIO_IntClear(flags);

  // Handle the rising edge event here
  if (flags & (1 << GPIO_PIN))
  {
    // Rising edge detected on PA3
    // Add your code here
    sl_led_turn_on(&sl_led_led0);
    TxRequested = true;
    sl_led_turn_off(&sl_led_led0);
  }
}




void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED && !TxRequested) {
      TxRequested = true;
  }
}

void sendWakeUp(void)
{
  sl_led_turn_on(&sl_led_led0);


  RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, true);
  RAIL_ConfigRfSenseSelectiveOokWakeupPhy(rail_handle);
  RAIL_SetRfSenseSelectiveOokWakeupPayload(rail_handle, NUM_SYNC_WORD_BYTES, SYNC_WORD);

  // Configure the radio for max power
  RAIL_TxPowerConfig_t txPowerConfig = {
      .mode = RAIL_TX_POWER_MODE_2P4_HP,
      //.mode = RAIL_TX_POWER_MODE_2P4GIG_HIGHEST,
      .voltage = 3300,
      .rampTime = 10
  };
  RAIL_Status_t status = RAIL_ConfigTxPower(rail_handle, &txPowerConfig);
  if (status != RAIL_STATUS_NO_ERROR) {
      // Handle error
      flashErrorCode(1);
  }
  RAIL_TxPower_t power = RAIL_TX_POWER_LEVEL_2P4_HP_MAX;

  status = RAIL_SetTxPower(rail_handle, power);
  if (status != RAIL_STATUS_NO_ERROR) {
      // Handle error
      flashErrorCode(2);
  }

  RAIL_StartTx(rail_handle, 0, RAIL_TX_OPTIONS_DEFAULT, NULL);
  sl_sleeptimer_delay_millisecond(75);
  sl_led_turn_off(&sl_led_led0);
}

void flashErrorCode(int error_code)
{
  while (true) {
      sl_led_turn_on(&sl_led_led0);
      sl_sleeptimer_delay_millisecond(500);
      sl_led_turn_off(&sl_led_led0);
      sl_sleeptimer_delay_millisecond(200);

      for (int i = 0; i < error_code; i++) {
          sl_led_turn_on(&sl_led_led0);
          sl_sleeptimer_delay_millisecond(100);
          sl_led_turn_off(&sl_led_led0);
          sl_sleeptimer_delay_millisecond(100);
      }
      sl_sleeptimer_delay_millisecond(500);
  }
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

  setupRisingEdgeInterrupt();

  // Get RAIL handle, used later by the application
  rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  
  // Flash LED to indicate the loaded firmware, and system ready
  sl_led_turn_on(&sl_led_led0);
  sl_sleeptimer_delay_millisecond(200);
  sl_led_turn_off(&sl_led_led0);
  sl_sleeptimer_delay_millisecond(200);
  sl_led_turn_on(&sl_led_led0);
  sl_sleeptimer_delay_millisecond(200);
  sl_led_turn_off(&sl_led_led0);
}

// App Loop
void app_process_action(void)
{
  if (TxRequested)
    {
      sendWakeUp();
      TxRequested = false;
      if (USE_EM4)
        {
          sl_sleeptimer_delay_millisecond(200);
          sendWakeUp();
        }
    }
}