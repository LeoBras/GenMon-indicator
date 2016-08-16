#include <stdio.h>

int main(void)
{
	unsigned long int n, p=0;
	FILE *f;
	char buff1[15];
		 	while(1)
	 {
		f = fopen("/sys/class/net/wlan0/statistics/rx_bytes", "r");
 	    fgets(buff1, 15, f);
 	    fclose(f);		
 	    n=atoi(buff1);
	    printf("↓%u kB/s \n",(n-p)/1024);
	     p=n;
	    sleep(1);
	 }
return 0;
}
