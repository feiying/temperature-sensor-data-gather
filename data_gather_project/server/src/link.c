#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "link.h"

/* create a node */
sensor_link NODE(struct rx_info node, sensor_link next)
{
	sensor_link t=(sensor_link)malloc(sizeof *t);
	t->vol = node.vol;
	strcpy(t->time, node.time);
	strcpy(t->name, node.name);
	t->next=next;

	return t;
}

/* show the list */
void show_list(sensor_link t)
{
	t=t->next;
	while(t)
	{
		printf("%s\n",t->name);
		printf("%d\n",t->vol);
		printf("%s\n\n",t->time);
		t=t->next;

	}
	printf("\n");
}

/* init a list */
sensor_link init_list()
{
	struct rx_info node;
	node.vol = 0.0;
	strcpy(node.time,"");
	strcpy(node.name,"");

	sensor_link t=NODE(node,NULL);
	sensor_link head=t;

	return head;
}

/* delete a node from the list */
int delete_node(sensor_link t,char *time)
{
	sensor_link x=NULL,y=NULL;
	x = t;
	y=t->next;
	while(y!=NULL)
	{
		if(!strcmp(time,t->time)) break;
		x=y;
		y=y->next;
	}
	if(y==NULL)
	{
		printf("can't find\n");
		return -1;
	}
	x->next=y->next;
	free(y);
	return 0;
}

/* insert a node to the list */
sensor_link insert_node(sensor_link t,struct rx_info node)
{
	sensor_link x,y=t;
	x=y;
	while(y!=NULL)
	{
		x=y;
		y=y->next;
	}
	x=x->next=NODE(node, y);

	return t;
}

/* distroy a list */
sensor_link distroy_list(const sensor_link t)
{
	sensor_link x=t;
	sensor_link tmp=x->next;
	while(x)
	{
		x=tmp->next;
		free(tmp);
		tmp=NULL;
		tmp=x;
	}
	t->next=NULL;
	return t;
}

