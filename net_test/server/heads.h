#ifndef	HEADS_H
#define	HEADS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>		// для gettimeofday
#include <sys/types.h>		// для getpid
#include <sys/socket.h>		// для сокета
#include <netinet/ip.h>		// для ip заголовка
#include <netinet/udp.h> 	// для udp заголовка
#include <netinet/tcp.h>	// для TCP заголовка

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define SRV_TCP_SPORT 60400	// порт сервера
#define SRV_TCP_DPORT 60401	// порт клиента

#define UDP_SPORT 60402	// порт сервера
#define UDP_DPORT 60403	// порт клиента

#pragma pack(push,1)
/*  Заголовок IP. Сумма байт=20 */
struct ip_h{
	//нужен разворот для биг индиан  1 байт = VER.HLEN
	// ! LITTLEENDIAN_BITFIELD
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
};
#pragma pack(pop)

/*	Заголовок ICMP. Сумма байт=8 */
struct icmp_h{
	unsigned char  TYPE;		// порт отправителя		8  бит
	unsigned char  CODE;		// порт назначения		8  бит
	unsigned short CRC;			// Длина пакета			16 бит
	unsigned short ID;			// контрольная сумма	16 бит
	unsigned short SEQ_NUM;		// контрольная сумма	16 бит
};

/* Заголовок UDP. Сумма байт=8*/
struct udp_h{
	unsigned short SPORT;	// порт отправителя
	unsigned short DPORT;	// порт назначения
	unsigned short LEN;		// Длина пакета
	unsigned short CRC;		// контрольная сумма
};

/* Заголовок TCP. Сумма байт=20*/
struct tcp_h{
	// ! LITTLEENDIAN_BITFIELD
	unsigned short SPORT;		// порт источника
	unsigned short DPORT;		// порт назначения
	unsigned int SEQ_NUM;		// номер последовательности
	unsigned int ACK_SEQ;		// номер подтверждения
	unsigned short FLAGS;
	/*unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв
	unsigned short RES : 4;		// резерв */
	unsigned short WIND;
	unsigned short CRC;
	unsigned short URG_PTR;

};

/* Стуктура команды для клиента. Сумма байт=16 */
/*
<field = cmd>
	1 - старт тест seqence
	2 - старт тест ping
	3 - старт тест load
	4 - завершение работы
	5 - настройки тест seqence
	6 - настройки тест ping 
	7 - настройки тест load
	

<field = type_ptk>
	-I - ICMP пакет
	-T - TCP пакет
	-U - UDP пакет

<filed = len_ptk>
	Длина пакета в байтах

<filed = cnt_ptk>
	Количество пакетов  

<filed = time_test>
	Время проведения теста в секундах

<filed = vlan>
	-n - с поддержкой
	-y - без поддержки

<field = ip_cln>
	IP адрес клиента
	Example: 192.168.1.4
*/
#pragma pack(push,1)
struct cmd_settings{
	unsigned short cmd;			// номер команды теста 
	unsigned short type_ptk;	// тип пакета (TCP/UDP/ICMP)
	unsigned short len_ptk;		// длина пакета
	unsigned short cnt_ptk;		// количество пакетов
	unsigned short time_test;	// время проведения теста
	unsigned short vlan;		// поддержка 802.1q,802.1p
	unsigned int ip_cln;		// ip клиента
};
#pragma pack(pop)



#endif