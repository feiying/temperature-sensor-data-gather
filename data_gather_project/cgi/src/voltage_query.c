#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>

#define HTML_HEADER   "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"></head><body><table border='1' cel    lspacing='1'>"
#define TABLE_HEADER   "<tr><td>ID</td><td>Name</td><td>Vol</td><td>Time</td></tr>"

int rscallback (void *p, int argc, char **argv, char **argvv)
{
	int i;
	*(int *)p = 0;
	printf ("<tr>");
	for (i = 0; i < argc; i++)
	{
		printf ("<td>");
		printf ("%s\t", argv[i]?argv[i]:"NULL");
		printf ("</td>");
	}
	putchar('\n');
	return 0;
}
void parse(char *buf, char *vol_up, char *vol_net)
{
	char *p = NULL;
	char *q = NULL;
	
	while ((p = strchr(buf ,'&')) != NULL)
	{
		*p++ = '\0';
		q = strchr(buf, '=');
		*q++ = '\0';

		if (strcmp(buf, "vol_up") == 0) 
		{
			strcpy (vol_up, q);
		}
		else if(strcmp(buf, "vol_net") == 0)
			strcpy (vol_net, q);
		buf = p;
	}

	q = strchr (buf, '=');
	*q++ = '\0';
	strcpy (vol_net , q);
}

int main(void)
{
	sqlite3 *db;
	char buf[1024];
	char vol_up[10];
	char vol_net[10];
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
	parse (buf, vol_up, vol_net);
	sql = sqlite3_mprintf("select * from reports where Vol> %s and Vol<%s", vol_up, vol_net);

	printf("%s\n",sql);

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
