CLIENT=./client
SERVER=./server
CGI=./CGI
QT=./QT

OBJ= $(CLIENT)

modules :  
	make -C $(CLIENT) SUBDIRS=$(PWD) 
	make -C $(SERVER) SUBDIRS=$(PWD) 
	make -C $(CGI) SUBDIRS=$(PWD)

install:
	mv	$(CGI)/*.cgi ./APP
	mv $(CLIENT)/adc_client ./APP
	sudo mv $(CLIENT)/driver.ko	./APP
	mv $(SERVER)/*server		./APP
	mv $(SERVER)/mydaemon	./APP

clean:
	rm -f $(CGI)/*.cgi $(CLIENT)/adc_client $(CLIENT)/*.ko $(SERVER)/*.o $(SERVER)/server $(SERVER)/mydaemon
	rm -f ./APP/*
