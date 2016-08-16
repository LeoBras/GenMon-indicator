#include <stdio.h>

int main(void)
{
    char buff1[10], buff2[10];

    FILE *f = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
    fgets(buff1, 10, f);
    fclose(f);
    
    f = fopen("/sys/class/power_supply/BAT0/current_now", "r");
	fgets(buff2, 10, f);
	fclose(f);

	printf("%.2fW \n",(float)atoi(buff1)/100000*(float)atoi(buff2)/10000000);
	return 0;
}