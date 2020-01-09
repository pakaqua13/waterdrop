
#define WD_OUT 0
#define WD_IN 1

#define NB_DROP  2
#define NB_DEV  12

typedef struct {
  String name;
  byte type;
  byte pin;
  byte en;
} WD_device_t;


  unsigned long wd_delai[NB_DROP*NB_DEV];
  unsigned long wd_duree[NB_DROP*NB_DEV];

WD_device_t wd_device[NB_DEV] = {
 {"EV0"     , WD_OUT , 36 , 0 },
 {"EV1"     , WD_OUT , 37 , 0 },
 {"EV2"     , WD_OUT , 38 , 0 },
 {"EV3"     , WD_OUT , 39 , 0 },
 {"EV4"     , WD_OUT , 34 , 0 },
 {"EV5"     , WD_OUT , 35 , 0 },
 {"EV6"     , WD_OUT , 32 , 0 },
 {"EV7"     , WD_OUT , 33 , 0 },
 {"APN"     , WD_OUT , 17 , 0 },
 {"FLASH0"  , WD_OUT , 2 , 0 },
 {"FLASH1"  , WD_OUT , 12 , 0 },
 {"FLASH2"  , WD_OUT , 13 , 0 }
};
