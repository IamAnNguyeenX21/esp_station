#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <input_iot.h>

input_callback_t input_callback = NULL;
static void IRAM_ATTR gpio_isr_handler (void *arg)
{
    int gpio_num = (uint32_t) arg;
    input_callback(gpio_num);
}

void input_io_create (int pin, itr_type_edge level)
{
    gpio_config_t io_config;
    // set the pin to input mode
    io_config.mode = GPIO_MODE_INPUT;
    // choose the interupt type;
    io_config.intr_type = HI_TO_LO;
    // bit masking to the choosen pin
    io_config.pin_bit_mask = (1ULL << pin);
    // enable pull down mode & disable pull up mode
    io_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_config);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(pin, gpio_isr_handler, (void*) pin);   

}

void input_io_get_level (gpio_num_t gpio_num)
{
    return gpio_get_level (gpio_num);
}

void input_set_callback (void *cb)
{
    input_callback = cb;
}

void output_io_create (gpio_num_t gpio_num)
{
    /* select the gpio pin to use*/
    gpio_pad_select_gpio(gpio_num);
    /*set output for the pin as the push/pull mode*/
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
}

void output_io_set_level (gpio_num_t gpio_num, uint32_t level)
{
    gpio_set_level(gpio_num, level);

}

void output_io_toggle (gpio_num_t gpio_num)
{
    uint32_t old_level = gpio_get_level(gpio_num);
    output_io_set_level(gpio_num, 1 - old_level);
}