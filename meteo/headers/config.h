struct config* get_config(char* filepath);
struct monitoring_point* configure_monitoring_points(struct config* configuration);
void err_malformed_config();