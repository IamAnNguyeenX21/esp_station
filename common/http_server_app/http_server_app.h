#ifndef __HTTP_SERVER_APP_H
#define __HTTP_SERVER_APP_H

typedef void (*http_sw1_data_callback) (char *data, int *len);

typedef void (*http_dht11_data_callback) (void);

void start_webserver(void);
void stop_webserver(void);
void set_callback_for_sw1 (http_sw1_data_callback cb);
void set_callback_for_dht11 (http_dht11_data_callback cb);
void dht11_response (char *data, int len);

#endif
