struct config {
    uint8_t id;
    char* key;
    char* value;
    struct config* head;
    struct config* next;
};

struct config* get_config(char* filepath);