struct config {
    uint8_t id;
    char* key;
    char* value;
    struct config* head;
    struct config* next;
};

struct monitoring_point {
    uint8_t device_id;
    char device_name[17];
    uint8_t pin;
    uint32_t gathering_interval_sec;
    struct monitoring_points* head;
    struct monitoring_points* next;
};

struct config* get_config(char* filepath);
struct monitoring_point* configure_monitoring_points(struct config* configuration);