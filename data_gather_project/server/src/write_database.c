#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sqlite3.h>
#include "server.h"
#include "link.h"

#define DEBUG
//#define DEBUG

int write_database(struct rx_info data)
{
	sqlite3 *db;
	int ret;
	char sql[256];

	ret = sqlite3_open(DataBasePath, &db);

#ifdef DEBUG
	printf("%s\n",DataBasePath);
#endif

	if(ret != SQLITE_OK) {
		fputs(sqlite3_errmsg(db), stdout);
		fputs("\n", stdout);
		exit(1);
	}

#ifdef DEBUG
	printf("name:%s\nvol:%f\ntime:%s\n",data.name,data.vol,data.time);
#endif
						  
	sprintf(sql,"insert into reports (Name,Vol,Time) values(\"%s\",%f,\"%s\");",data.name,data.vol,data.time);
#ifdef DEBUG
	printf("ok\n");
#endif
	pthread_mutex_lock(&database_mutex);
	ret = sqlite3_exec(db, sql, NULL, NULL, NULL);
	pthread_mutex_unlock(&database_mutex);

#ifdef DEBUG
	if(ret != SQLITE_OK) {
		fputs("insert error\n",stdout);
	}  
	else{
		printf("Insert Success\n");
	}
#endif

	return 0;
}
