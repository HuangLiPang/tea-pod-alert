#include <stdbool.h>
#include <stdio.h>
#include <temperature.h>
#include <tock.h>
#include <ipc.h>

static bool temperature = false;

static void ipc_callback(int pid, int len, int buf, __attribute__ ((unused)) void* ud) {
  uint8_t* buffer = (uint8_t*) buf;
  int* temp;

  if (len < 1) {
    // Need at least one byte for the number of bytes
    return;
  }

  uint8_t number_of_bytes = buffer[0];

  if (len < number_of_bytes) {
    // Buffer must be able to hold the random bytes requested.
    return;
  }

  temp = malloc(sizeof(int));
  if (temp == NULL) {
    return;
  }
  *temp = 0;

  temperature = driver_exists(DRIVER_NUM_TEMPERATURE);

  if (temperature) {
    temperature_read_sync(temp);
    *temp /= 100;
    printf("Temperature: %d deg C\n", *temp);

    uint8_t temp_8 = (uint8_t) ((*temp) % 256);
    // printf("temp_8 = %u\n", temp_8);
    memcpy(buffer, &temp_8, len);
    free(temp);
  } else {
    printf("temperature sensor not available\n");
    return;
  }

  // Signal done.
  ipc_notify_client(pid);
}

int main(void) {

  ipc_register_svc(ipc_callback, NULL);
  return 0;
}