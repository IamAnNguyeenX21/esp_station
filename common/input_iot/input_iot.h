#ifndef INPUT_IOT_H
#define INPUT_IOT_H
#include <esp_err.h>
#include <hal/gpio_types.h>

typedef enum 
{
    LO_TO_HI = 1,
    HI_TO_LO = 2,
    ANY_EDGE = 3,
}itr_type_edge;

typedef void (*input_callback_t)(int pin);

void input_io_create (gpio_num_t gpio_num, itr_type_edge level);
void input_io_get_level (gpio_num_t gpio_num);
void input_set_callback (void *cb);
void output_io_create (gpio_num_t gpio_num);
void output_io_set_level (gpio_num_t gpio_num, uint32_t level);
void output_io_toggle (gpio_num_t gpio_num);


#endif