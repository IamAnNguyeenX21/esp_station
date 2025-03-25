#include "esp_timer.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

static gpio_num_t dht11_num;
static struct dht11_reading dht11_last_read;
static int64_t last_read_time = -2000000;


static int waitOrTimeout (uint16_t microSeconds, int level)
{
    int microTicks = 0;
    while (gpio_get_level(dht11_num) == level)
    {
        if (microTicks++ > microSeconds)
        {
            /* code */
            return DHT11_TIMEOUT_ERROR;
        }
        ets_delay_us(1);
    
    }
    return microTicks;
}

static int checkCRC (uint8_t data[])
{
    if ((data[4] == data[0] + data[1] + data[2] + data[3]))
    {
        /* code */
        return DHT11_OK;
    }
    else
    {
        return DHT11_CRC_ERROR;
    }
    
}

static void _sendStartSignals ()
{
    gpio_set_direction(dht11_num, GPIO_MODE_OUTPUT);
    gpio_set_level(dht11_num, 0);
    ets_delay_us(20*1000);
    gpio_set_level(dht11_num, 1);
    ets_delay_us(40);
    gpio_set_direction(dht11_num, GPIO_MODE_INPUT);
}

static int _checkResponse()
{
    if (waitOrTimeout(80, 0) == DHT11_TIMEOUT_ERROR)
    {
        /* code */
        return DHT11_TIMEOUT_ERROR;
    }

    if (waitOrTimeout(80, 1) == DHT11_TIMEOUT_ERROR)
    {
        /* code */
        return DHT11_TIMEOUT_ERROR;
    }
    return DHT11_OK;
    
}

static struct dht11_reading _timeoutError ()
{
    /* data */
    struct dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
};

static struct dht11_reading _CRCError ()
{
    /* data */
    struct dht11_reading CRCError = {DHT11_CRC_ERROR, -1, -1};
    return CRCError;
};

void DHT11_init (gpio_num_t num)
{
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    dht11_num = num;

}

struct dht11_reading DHT11_read ()
{
    // dht11 mat khoang 2s de thuc hien lan doc moi nen neu ham nay dc goi som hon 2s sau lan doc trc do thi tra ve gia tri lan doc truoc do
    if (esp_timer_get_time - 2000000 < last_read_time)
    {
        /* code */
        return dht11_last_read;
    }
    last_read_time = esp_timer_get_time;

    uint8_t data[5] = {0,0,0,0,0};
    _sendStartSignals();
    if (_checkResponse() == DHT11_TIMEOUT_ERROR)
    {
        /* code */
        dht11_last_read = _timeoutError();
        return dht11_last_read;
    }

    // Doc dau ra cam bien
    for (int i = 0; i < 40; i++)
    {
        /* code */
        if (waitOrTimeout(50,0) == DHT11_TIMEOUT_ERROR)
        {
            /* code */
            return dht11_last_read = _timeoutError();
        }
        // neu nhan duoc bit 1
        if (waitOrTimeout(50,1) > 28)
        {
            data[i/8] |= (1<<(7-(i%8)));
        }
    }
    
    if (checkCRC(data) != DHT11_CRC_ERROR)
    {
        /* code */
        dht11_last_read.status = DHT11_OK;
        dht11_last_read.temperature = data[2];
        if(data[3] & 0x80)
        {
            dht11_last_read.temperature = -1 - dht11_last_read.temperature;
        }
        dht11_last_read.temperature += (data[3] & 0x0f) * 0.1;
        dht11_last_read.humidity = data[0] + (float)data[1]*0.1;
        return dht11_last_read;
    }
    else
    {
        return dht11_last_read = _CRCError();
    }
    
    
}