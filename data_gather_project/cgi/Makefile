INCLUDE = -Iinclude
LIB_DIR = -L./lib/$(host)
SQLITE_LIB = -L/home/akaedu/downloads/sqlite-3.4.0/.lib

CC =$(prefix)gcc

OBJS = time_query.cgi node_query.cgi voltage_query.cgi

all: $(OBJS) 

%.cgi : src/%.c
	$(CC) $< $(INCLUDE)  $(SUF)$(LIB_DIR) -lsqlite3 -o $@

clean:
	rm -f *.cgi *~ *.o
