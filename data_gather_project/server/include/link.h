#include "server.h"
#ifndef __LINK_H__
#define __LINK_H__

typedef struct rx_info * sensor_link;
struct rx_info{
	char name[24];
	float vol;
	char time[32];
	sensor_link next;
};

sensor_link NODE(struct rx_info node, sensor_link next);
sensor_link init_list();
void show_list(sensor_link t);
int delete_node(sensor_link t,char * time);
sensor_link insert_node(sensor_link t,struct rx_info node);
sensor_link distroy_list(const sensor_link t);

extern sensor_link wait_handler;
#endif
