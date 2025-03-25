#ifndef DHT11_H
#define DHT11_H

#include "driver/gpio.h"

typedef enum dht11_status {
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

struct dht11_reading
{
    /* data */
    int status;
    float temperature;
    float humidity;
};

void DHT11_init (gpio_num_t num);

struct dht11_reading DHT11_read ();

#endif