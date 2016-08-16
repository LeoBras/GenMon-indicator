
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	struct{
	unsigned long int pr;
	unsigned long int pt;
	} rede;


	FILE *f;
	unsigned long int n;
	float potencia,temper,percentual,horas;
	int rx_rate,tx_rate,minutos,charge_now,charge_full;
	char buff1[15], buff2[15];
	char bateria;

	//LE DATA DOS AQUIVOS
	f=fopen("/dev/shm/monitor","r");
	if(f==NULL){
		// Não Existes
		rede.pr = 0;
		rede.pt = 0;
	} else {
		fread(&rede,(size_t)sizeof(long int),(size_t)2,f);
		fclose(f);
	}


	// POTENCIA/Porcentagem_Tempo
	f = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
	if(f==NULL){
		bateria = 0;
	} else {
		bateria = 1;

		fgets(buff1, 10, f);
		fclose(f);

		f = fopen("/sys/class/power_supply/BAT0/current_now", "r");
		fgets(buff2, 10, f);
		fclose(f);
		
		potencia=(float)atoi(buff1)/100000*(float)atoi(buff2)/10000000;
		// printf("%f\n",potencia);

		f = fopen("/sys/class/power_supply/BAT0/charge_now", "r");
		fgets(buff1, 10, f);
		fclose(f);

		f = fopen("/sys/class/power_supply/BAT0/charge_full", "r");
		fgets(buff2, 10, f);
		fclose(f);

		charge_now = atoi(buff1);
		charge_full = atoi(buff2);
		percentual = 100*(float)charge_now/(float)charge_full;

		f = fopen("/sys/class/power_supply/BAT0/status", "r");
		fgets(buff1, 2, f);
		fclose(f);

		if((buff1[0] == 'C')||(buff1[0] == 'F'))
			charge_now = charge_full-charge_now;


		horas = (charge_now/100000)/potencia;
		minutos = 60*(horas-(int)horas);
				//minutos=((charge_now/1000)%(int)(potencia*100))*6/100;

		if (percentual <= 1 && buff1[0] == 'D') {
			system("notify-send 'Carga MUITO fraca' 'Conecte o carregador agora, ou o computador desligará em breve' -i battery -u critical");
			sleep(4);
		}

		if(percentual <= 0.1)
			system("shutdown -h now");



	}
	//TEMPERATURA

	f = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon0/temp2_input", "r");
	if(f == NULL)
		f = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp2_input", "r");

	fgets(buff1, 10, f);
	fclose(f);
	temper = (float)atoi(buff1)/1000;
	//printf("%f\n",temper);

	//NET RX

	f = fopen("/sys/class/net/wlp3s0/statistics/rx_bytes", "r");
	if(f == NULL)
		f = fopen("/sys/class/net/wlan0/statistics/rx_bytes", "r");

	fgets(buff1, 15, f);
	fclose(f);
	n = atoi(buff1);
	rx_rate = (int)((n-rede.pr)/1024);
	// printf("%f \t%u\t%u\n",rx_rate,n,pr);
	rede.pr = n;

	//NET TX

	f = fopen("/sys/class/net/wlp3s0/statistics/tx_bytes", "r");
	if(f == NULL)
		f = fopen("/sys/class/net/wlan0/statistics/tx_bytes", "r");


		fgets(buff2, 15, f);
		fclose(f);
		n = atoi(buff2);
		tx_rate = (int)((n-rede.pt)/1024);
		//printf("%f \t%u\t%u\n\n",tx_rate,atoi(buff2),pt);
		rede.pt = n;

	if(temper > 70)
		strcpy(buff1,"\"red\"");
	else
		strcpy(buff1,"\"green\"");

	if (potencia>30)
		strcpy(buff2,"\"red\"");
	else
		strcpy(buff2,"\"black\"");

	if(bateria == 1)
	printf("<txt><span fgcolor=\"#0000FF\">↓%3d kB/s  </span><span fgcolor=%s>%.2fW  </span><span fgcolor=\"black\">%.2f%%\t</span>\n<span fgcolor=\"#1E90FF\">↑%3d kB/s  </span><span fgcolor=%s>%.1f°C\t</span><span fgcolor=\"black\">%.2d:%.2d\t</span></txt>",rx_rate,buff2,potencia,percentual,tx_rate,buff1,temper,(int)horas,minutos);

	else
	printf("<txt><span fgcolor=\"#0000FF\">↓%3d kB/s  </span><span fgcolor=\"black\">NOBAT</span>\n<span fgcolor=\"#1E90FF\">↑%3d kB/s  </span><span fgcolor=%s>%.1f°C</span></txt>",rx_rate,tx_rate,buff1,temper);

		//printf("%.2fW\n%.1f°C",potencia,temper);

	// GRAVA VARIAVEL NO ARQUIVO
	f = fopen("/dev/shm/monitor","w+");
	fwrite(&rede,(size_t)sizeof(long int),(size_t)2,f);
	fclose(f);
	return 0;
}
		// }
		// else
		// 	printf("Sem Bateria\n");
