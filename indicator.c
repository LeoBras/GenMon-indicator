#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

enum colors_e{
	RED,
	GREEN,
	BLACK
};

const char colors[3][10] = {"\"red\"", "\"green\"", "\"black\"" };

struct hm {
	int horas, minutos;
};

inline int getBateria(float *percentual,
	       float *potencia,
	       struct hm *tempo );
inline float getTemperature(void);
inline void getSpeed(int *upload, int *download);

int main(void)
{

	int rx_rate, tx_rate;
	getSpeed(&tx_rate, &rx_rate);

	float potencia, percentual;
	struct hm tempo;
	int bateria = getBateria(&percentual, &potencia, &tempo);

	float temper = getTemperature();

	//LOWBAT!
	if(bateria == 'D'){ //DESCARREGANDO
		if (percentual <= 1){
			system("notify-send 'Carga MUITO fraca' 'Conecte o carregador agora, ou o computador desligará em breve' -i battery -u critical");
			sleep(4);
		}

		if(percentual <= 0.1)
			system("shutdown -h now");

	}

	//Print Color
	int temp_color = GREEN;
	if(temper > 70)
		temp_color = RED;

	int pot_color = BLACK;
	if (potencia > 30)
		pot_color = RED;

	// PRINTS!
	if(bateria > 0) // BAT PRESENT
		printf("<txt><span fgcolor=\"#0000FF\">↓%3d kB/s  </span>" "<span fgcolor=%s>%.2fW  </span>" "<span fgcolor=\"black\">%.2f%%\t</span>\n"
			    "<span fgcolor=\"#1E90FF\">↑%3d kB/s  </span>" "<span fgcolor=%s>%.1f°C\t</span>""<span fgcolor=\"black\">%.2d:%.2d\t</span></txt>",
		       rx_rate,colors[pot_color],potencia,percentual,
		       tx_rate,colors[temp_color],temper,tempo.horas,tempo.minutos);
	else
		printf("<txt><span fgcolor=\"#0000FF\">↓%3d kB/s  </span>" "<span fgcolor=\"black\">NOBAT</span>\n"
			    "<span fgcolor=\"#1E90FF\">↑%3d kB/s  </span>" "<span fgcolor=%s>%.1f°C</span></txt>",
		       rx_rate,tx_rate,colors[temp_color],temper);

	return 0;
}


inline int getBateria(float *percentual,
	       float *potencia,
	       struct hm *tempo )
{
	FILE *f;
	char buff1[15], buff2[15];
	int charge_now, charge_full;

	f = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
	if(f == NULL)
		return 0;

	fgets(buff1, 10, f);
	fclose(f);

	f = fopen("/sys/class/power_supply/BAT0/current_now", "r");
	fgets(buff2, 10, f);
	fclose(f);

	*potencia = (float)atoi(buff1) / 100000 * (float)atoi(buff2) / 10000000;

	f = fopen("/sys/class/power_supply/BAT0/charge_now", "r");
	fgets(buff1, 10, f);
	fclose(f);

	f = fopen("/sys/class/power_supply/BAT0/charge_full", "r");
	fgets(buff2, 10, f);
	fclose(f);

	charge_now = atoi(buff1);
	charge_full = atoi(buff2);
	*percentual = 100 * (float)charge_now / (float)charge_full;

	f = fopen("/sys/class/power_supply/BAT0/status", "r");
	fgets(buff1, 2, f);
	fclose(f);

	if((buff1[0] == 'C')||(buff1[0] == 'F'))
		charge_now = charge_full-charge_now;

	float aux = (charge_now / 100000) / *potencia;
	tempo->horas = (int) aux;
	tempo->minutos = 60 * (float)(aux - tempo->horas) ;

	return buff1[0];
}

inline float getTemperature(void)
{
	char buff1[15];
	FILE *f;

	f = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon0/temp2_input", "r");
	if(f == NULL)
		f = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp2_input", "r");

	fgets(buff1, 10, f);
	fclose(f);
	return (float)atoi(buff1)/1000;
}

inline void getSpeed(int *upload, int *download)
{
	FILE *f;
	char buff1[15], buff2[15];

	struct{
		unsigned long int pr;
		unsigned long int pt;
	} rede; //Anonymous struct

	//LE DATA DOS AQUIVOS
	f=fopen("/dev/shm/monitor","r");
	if(f == NULL){
		// Não Existe
		rede.pr = 0;
		rede.pt = 0;
	} else {
		fread(&rede, sizeof(long int), 2, f);
		fclose(f);
	}

	//NET RX

	f = fopen("/sys/class/net/wlp3s0/statistics/rx_bytes", "r");
	if(f == NULL)
		f = fopen("/sys/class/net/wlan0/statistics/rx_bytes", "r");
	fgets(buff1, 15, f);
	fclose(f);

	unsigned long int n = atoi(buff1);
	*download = (int)((n-rede.pr)/1024);
	rede.pr = n;

	//NET TX

	f = fopen("/sys/class/net/wlp3s0/statistics/tx_bytes", "r");
	if(f == NULL)
		f = fopen("/sys/class/net/wlan0/statistics/tx_bytes", "r");
	fgets(buff2, 15, f);
	fclose(f);

	n = atoi(buff2);
	*upload = (int)((n-rede.pt)/1024);
	rede.pt = n;

	// GRAVA VARIAVEL NO ARQUIVO
	f = fopen("/dev/shm/monitor","w");
	fwrite(&rede, (size_t)sizeof(long int), (size_t)2,f);
	fclose(f);
}
