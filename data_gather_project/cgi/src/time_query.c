#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>

#define HTML_HEADER   "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"></head><body><table border='1' cel    lspacing='1'>"
#define TABLE_HEADER   "<tr><td>ID</td><td>Name</td><td>Vol</td><td>Time</td></tr>"


struct query_time{
	char date[12];
	char hour[4];
	char minute[4];
	char second[4];
};

int rscallback (void *p, int argc, char **argv, char **argvv)
{
	int i;
	*(int *)p = 0;
	printf ("<tr>");
	for (i = 0; i < argc; i++)
	{
		printf ("<td>");
		printf ("%s\t",argv[i]?argv[i]:"NULL");
		printf ("</td>");
	}
	putchar('\n');
	return 0;
}

void parse(char *buf,struct query_time *start,struct query_time *end)
{
	char *p = NULL;
	char *q = NULL;

	while ((p = strchr(buf ,'&')) != NULL)
	{
		*p++ = '\0';
		q = strchr(buf, '=');
		*q++ = '\0';

		if (strcmp(buf, "s_date") == 0) 
			strcpy (start->date, q);
		else if(strcmp(buf, "s_hour") == 0)
			strcpy (start->hour, q);
		else if(strcmp(buf, "s_minute") == 0)
			strcpy (start->minute, q);
		else if(strcmp(buf, "s_second") == 0)
			strcpy (start->second, q);
		else if(strcmp(buf,"e_date") == 0)
			strcpy (end->date,q);
		else if(strcmp(buf, "e_minute") == 0)
			strcpy (end->minute, q);
		else if(strcmp(buf, "e_second") == 0)
			strcpy (end->second, q);
		else if(strcmp(buf, "e_hour") == 0)
			strcpy (end->hour, q);
		buf = p;
	}
	q = strchr(buf, '=');
	*q++ = '\0';
	strcpy (end->second, q);
}

int main(void)
{
	sqlite3 *db;
	char buf[1024];
	char s_time[48];
	char e_time[48];
	struct query_time start;
	struct query_time end;
	char *err = 0;
	int ret = 0;
	char *sql = NULL;

	int empty = 1;

	printf("Content-Type:text/html\r\n\r\n");

	ret = sqlite3_open("./gather_data.db",&db);

	if (ret != SQLITE_OK)
	{
		fputs (sqlite3_errmsg(db), stderr);
		fputs ("\n", stderr);
		exit (1);
	}
	printf(HTML_HEADER);
	printf(TABLE_HEADER);

	scanf ("%s", buf);
	parse (buf, &start,&end);
	sprintf(s_time,"%s-%s:%s:%s",start.date,start.hour,start.minute,start.second);
	sprintf(e_time,"%s-%s:%s:%s",end.date,end.hour,end.minute,end.second);

	sql = sqlite3_mprintf("select * from reports where Time>%Q and Time<%Q", s_time, e_time);

	printf("the data from %s to  %s\n",s_time,e_time);
	ret = sqlite3_exec (db, sql, rscallback, &empty, &err);
	if (ret != SQLITE_OK)
	{
		fputs (err, stderr);
		fputs ("\n", stderr);
		sqlite3_close (db);
		exit (1);
	}
	sqlite3_free (sql);
	printf("</table></body></html>");
	if (empty)
	{
		fputs ("table employee is empty\n", stderr);
		exit (1);
	}
	sqlite3_close (db);
	return 0;
}
