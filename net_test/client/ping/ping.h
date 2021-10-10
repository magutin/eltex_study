#ifndef PING_H
#define PING_H


#pragma pack(push,1)
/*
	Заголовок IP
*/
struct ip_h{
	//нужен разворот для биг индиан  1 байт = VER.HLEN
	unsigned char  HELEN   : 4;		// Номер версии			4  бита--| 	
	unsigned char  VER     : 4;		// Длина заголовка  	4  бита--|
	unsigned char  SERV_TYPE;		// Тип Сервиса			8  бит
	unsigned short TOTL_LEN;		// Общая длина 			16 бит
	unsigned short IDENTIF;			// ID пакета			16 бит
	unsigned short F_OFFST : 13 ;	// Флаги			    3  бита--|
	unsigned short FLAGS   : 3;	    // Смещение фрагмента	13 бит---|
	unsigned char  TTL;				// Время жизни			8  бит
	unsigned char  PROTOCOL;		// Тип протокола		8  бит
	unsigned short H_CSUM;			// контрольная сумма	16 бит
	unsigned int   S_IP;		    // IP отправителя		32 бита
	unsigned int   D_IP;		    // IP получателя		32 бита
									/* -------------------------------*/
									/*         Сумма байт - 20 байт   */
};
#pragma pack(pop)

/*
	Заголовок ICMP
*/
struct icmp_h{
	unsigned short SPORT;	// порт отправителя
	unsigned short DPORT;	// порт назначения
	unsigned short LEN;		// Длина пакета
	unsigned short CRC;		// контрольная сумма
};

#endif

