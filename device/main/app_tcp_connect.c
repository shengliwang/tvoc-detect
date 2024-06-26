#include <sys/socket.h>

#include "esp_log.h"
#include "app_tcp_connect.h"

#define SERVER_IP   "192.168.31.184"
#define SERVER_PORT 8000

static const char *TAG = "app_tcp_connect";
static int s_socket_id = -1;

// return socket id
static int s_app_tcp_connect(const char * host_ip, uint16_t port){
    int addr_family = 0;
    int ip_protocol = 0;
    struct sockaddr_in dest_addr;
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    
    int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, port);
    
    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        return -1;
    }
    ESP_LOGI(TAG, "Successfully connected");

    return sock;
}

static void s_app_tcp_recv_task(void *pvParameters){
    char rx_buffer[128];

    for (;;){
        int len = recv(s_socket_id, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
        if (len < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }             // Data received
        else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, SERVER_IP);
                ESP_LOGI(TAG, "%s", rx_buffer);
        }
    }
    
    vTaskDelete(NULL);
}
void app_tcp_connect_init(void)
{


    while (1) {
        s_socket_id = s_app_tcp_connect(SERVER_IP, SERVER_PORT);
        if (s_socket_id < 0) {
            break;
        }
        const char *payload = "Message from ESP32 ";
        while (1) {
            int err = send(s_socket_id, payload, strlen(payload), 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }

        }

        if (s_socket_id != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(s_socket_id, 0);
            close(s_socket_id);
        }
    }
}

