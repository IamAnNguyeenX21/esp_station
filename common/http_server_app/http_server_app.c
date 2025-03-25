
/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include <esp_http_server.h>
#include "http_server_app.h"


/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "example";
static httpd_handle_t server = NULL;

static http_sw1_data_callback sw1_callback = NULL; // con tro den ham callback cua su kien sw1
static http_dht11_data_callback dht11_callback = NULL;
static httpd_req_t *REQ;
extern const uint8_t temp_html_binary_start[] asm("_binary_temp_html_start");
extern const uint8_t temp_html_binary_end[] asm("_binary_temp_html_end");
/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    // const char* resp_str = (const char*) "Hello World";
    httpd_resp_set_type(req, "text/html");
    // httpd_resp_send(req, resp_str, strlen(resp_str));
    httpd_resp_send(req, &temp_html_binary_start, temp_html_binary_end - temp_html_binary_start);

    return ESP_OK;
}

static const httpd_uri_t get_data_dht11 = {
    .uri       = "/dht11",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

static esp_err_t hello_get_data_handler(httpd_req_t *req)
{

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    dht11_callback();
    REQ = req;
    // const char* resp_str = (const char*) "{\"temperature\": \"5\", \"humidity\": \"60\"}";
    // httpd_resp_set_type(req, "text/html");
    // httpd_resp_send(req, resp_str, strlen(resp_str));

    return ESP_OK;
}

static const httpd_uri_t get_stats_dht11 = {
    .uri       = "/getdatadht11",
    .method    = HTTP_GET,
    .handler   = hello_get_data_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

void dht11_response (char *data, int len)
{
    httpd_resp_send(REQ, data, len);
}
/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];

    /* Read the data for the request */
    httpd_req_recv(req, buf, req->content_len);
    const char* resp_str = (const char*) "Goi cai nay ra an lon a thg ngu";
    httpd_resp_send(req, resp_str, strlen(resp_str));

    printf("Data: %s\n" , buf);

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};

static esp_err_t sw1_echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    /* Read the data for the request */
    httpd_req_recv(req, buf, req->content_len);
    // printf("Data: %s\n" , buf);
    sw1_callback(&buf, req->content_len);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t sw1_echo = {
    .uri       = "/switch1",
    .method    = HTTP_POST,
    .handler   = sw1_echo_post_handler,
    .user_ctx  = NULL
};
/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/dht11", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

// /* An HTTP PUT handler. This demonstrates realtime
//  * registration and deregistration of URI handlers
//  */
// static esp_err_t ctrl_put_handler(httpd_req_t *req)
// {
//     char buf;
//     int ret;

//     if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
//         if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//             httpd_resp_send_408(req);
//         }
//         return ESP_FAIL;
//     }

//     if (buf == '0') {
//         /* URI handlers can be unregistered using the uri string */
//         ESP_LOGI(TAG, "Unregistering /hello and /echo URIs");
//         httpd_unregister_uri(req->handle, "/hello");
//         httpd_unregister_uri(req->handle, "/echo");
//         /* Register the custom error handler */
//         httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
//     }
//     else {
//         ESP_LOGI(TAG, "Registering /hello and /echo URIs");
//         httpd_register_uri_handler(req->handle, &hello);
//         httpd_register_uri_handler(req->handle, &echo);
//         /* Unregister custom error handler */
//         httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
//     }

//     /* Respond with empty body */
//     httpd_resp_send(req, NULL, 0);
//     return ESP_OK;
// }

// static const httpd_uri_t ctrl = {
//     .uri       = "/ctrl",
//     .method    = HTTP_PUT,
//     .handler   = ctrl_put_handler,
//     .user_ctx  = NULL
// };

void start_webserver(void)
{

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &get_data_dht11);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &sw1_echo);
        httpd_register_uri_handler(server, &get_stats_dht11);
        // httpd_register_uri_handler(server, &ctrl);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);

    }
    else
    {
        ESP_LOGI(TAG, "Error starting server!");
    }

}

void stop_webserver(void)
{
    // Stop the httpd server
    httpd_stop(server);
}

void set_callback_for_sw1 (http_sw1_data_callback cb) // dang ki ham callback cho handler cua sw1
{
    sw1_callback = cb;
}

void set_callback_for_dht11 (http_dht11_data_callback cb)
{
    dht11_callback = cb;
}

// static void connect_handler(void* arg, esp_event_base_t event_base, 
//                             int32_t event_id, void* event_data)
// {
//     httpd_handle_t* server = (httpd_handle_t*) arg;
//     if (*server == NULL) {
//         ESP_LOGI(TAG, "Starting webserver");
//         *server = start_webserver();
//     }
// }



