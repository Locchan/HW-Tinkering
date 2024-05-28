#ifndef METSTOR_H
#define METSTOR_H

void push_mon_entry(struct monitoring_data_entry* input_data);
uint32_t offload_metrics();
int offload_file();
uint32_t get_metric_arr_len(bool ignore_mutex);
#endif