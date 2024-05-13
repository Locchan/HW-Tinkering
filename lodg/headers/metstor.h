#ifndef METSTOR_H
#define METSTOR_H

void push_mon_entry(struct monitoring_data_entry* input_data);
uint32_t offload_metrics();
int offload_mqtt(struct monitoring_data_entry* metrics_to_offload);
int offload_file(struct monitoring_data_entry* metrics_to_offload);
void return_data(struct monitoring_data_entry* metrics_to_return);
#endif