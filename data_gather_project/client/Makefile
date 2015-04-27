####################################################
# * Function : driver makefile 
# * Author: asmcos@gmail.com
# * Date : 2005-08-24
# * $Id: Makefile,v 1.3 2006/05/17 12:37:19 asmcos Exp $
#####################################################

ifneq ($(KERNELRELEASE),)
obj-m :=driver.o
else

KERNELSRC :=/home/akaedu/Nick/MC2410E/linux-2.6.17.14
DIR=/home/akaedu/Desktop/last_project/client

all:modules adc_client

modules :  
	make -C $(KERNELSRC) SUBDIRS=$(DIR) $@

adc_main:
	arm-linux-gcc main.c -o adc_main

adc_client:
	arm-linux-gcc client.c -o adc_client

clean:
	rm -f *.o *.ko *.mod.c *~ adc_client adc_main

endif
