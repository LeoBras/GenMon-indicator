#include <stdio.h>
#include <string.h>

int main(void)
{
    char buff1[10], buff2[10];


    // POTENCIA
    FILE *f = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
    if(f!=NULL)
    {
    fgets(buff1, 10, f);
    fclose(f); 

    f = fopen("/sys/class/power_supply/BAT0/current_now", "r");
	fgets(buff2, 10, f);
	fclose(f);
	
	float potencia=(float)atoi(buff1)/100000*(float)atoi(buff2)/10000000;
	if (potencia>30)
		strcpy(buff2,"\"red\"");
	else
		strcpy(buff2,"\"black\"");

	//TEMPERATURA

    f = fopen("/sys/devices/platform/coretemp.0/temp2_input", "r");
    fgets(buff1, 5, f);
    fclose(f);
    float temper=(float)atoi(buff1)/100;
    if(temper>70)
		strcpy(buff1,"\"red\"");
	else
		strcpy(buff1,"\"green\"");


	printf("<txt><span fgcolor=%s>%.2fW</span>\n<span fgcolor=%s>%.1f°C</span></txt>",buff2,potencia,buff1,temper);
	//printf("%.2fW\n%.1f°C",potencia,temper);
	return 0;
	}
	else
		printf("Sem Bateria\n");
	return 0;
}
