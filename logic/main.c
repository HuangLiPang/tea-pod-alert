#include <ipc.h>
#include <stdio.h>
#include <string.h>
#include <timer.h>

// Every 500 ms use the RNG service to randomly select an LED to turn on or
// off and then use the LED service to control that LED.

int _led_service = -1;
int _temp_service = -1;
char _led_buf[64] __attribute__((aligned(64)));
char _temp_buf[64] __attribute__((aligned(64)));

uint8_t _number_of_leds = 0;

bool _done = false;

// For this simple example, the callback only need set the yield variable.
static void ipc_callback(__attribute__ ((unused)) int pid,
                         __attribute__ ((unused)) int len,
                         __attribute__ ((unused)) int arg2,
                         __attribute__ ((unused)) void* ud) {
  _done = true;
}

// Uses the LED service to get how many LEDs are on the board.
static uint8_t get_number_of_leds(void) {
  _done       = false;
  _led_buf[0] = 0;
  ipc_notify_svc(_led_service);
  yield_for(&_done);

  return _led_buf[0];
}

// Set an LED with the LED service.
static void set_led(uint8_t led_index, uint8_t led_state) {
  // Turn the last LED on.
  _led_buf[0] = 1;         // Set LED state.
  _led_buf[1] = led_index; // Choose the LED.
  _led_buf[2] = led_state; // Set the LED.
  _done       = false;
  ipc_notify_svc(_led_service);
  yield_for(&_done);
}

static uint8_t get_temp(void) {
  _done       = false;
  _temp_buf[0] = 1;
  ipc_notify_svc(_temp_service);
  yield_for(&_done);

  return _temp_buf[0];
}

int main(void) {
  // Retrieve a handle to the LED service.
  _led_service = ipc_discover("org.tockos.tutorials.ipc.led");
  if (_led_service < 0) {
    printf("No led service\n");
    return -1;
  }

  // Setup IPC for LED service
  ipc_register_client_cb(_led_service, ipc_callback, NULL);
  ipc_share(_led_service, _led_buf, 64);

  // Retrieve a handle to the LED service.
  _temp_service = ipc_discover("org.tockos.tutorials.ipc.temp");
  if (_temp_service < 0) {
    printf("No temperature service\n");
    return -1;
  }

  // Setup IPC for LED service
  ipc_register_client_cb(_temp_service, ipc_callback, NULL);
  ipc_share(_temp_service, _temp_buf, 64);

  // First need to get the number of LEDs.
  _number_of_leds = get_number_of_leds();
  printf("Number of LEDs: %d\n", _number_of_leds);

  // Then randomly turn on and off LEDs using the two services.
  while (1) {
    uint8_t temp = get_temp();

    if(temp > 28) {
      uint8_t led_index = (0xFF) % _number_of_leds;
      uint8_t led_state = 0xFF;
      set_led(led_index, led_state);
    } else {
      uint8_t led_index = (0xFF) % _number_of_leds;
      uint8_t led_state = 0x00;
      set_led(led_index, led_state);
    }

    delay_ms(1000);
  }

  return 0;
}