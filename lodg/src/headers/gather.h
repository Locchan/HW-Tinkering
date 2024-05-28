#ifndef GLOBS_H
#define GLOBS_H

struct monitoring_data_entry* gather_metric(struct monitoring_point* point);
float get_metric_value(struct monitoring_point* point);

#endif