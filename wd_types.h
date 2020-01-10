
#define WD_OUT 0
#define WD_IN 1

#define NB_DROP  2
#define NB_DEV  8
#define NB_TRIGGER  4

typedef struct {
  String name;
  byte type;
  byte pin;
  byte en;
} WD_device_t;


  unsigned long wd_delai[NB_DROP*NB_DEV];
  unsigned long wd_duree[NB_DROP*NB_DEV];
  unsigned long wd_trig[NB_DROP*NB_DEV];
  unsigned long wd_trig_Start[NB_TRIGGER+1];
  unsigned long wd_trig_Current[NB_TRIGGER+1];
  unsigned long wd_trig_MaxTime[NB_TRIGGER+1];

WD_device_t wd_device[NB_DEV] = {
 {"EV0"     , WD_OUT , 36 , 0 },
 {"EV1"     , WD_OUT , 37 , 0 },
 {"EV2"     , WD_OUT , 38 , 0 },
 {"EV3"     , WD_OUT , 39 , 0 },
 /*{"EV4"     , WD_OUT , 34 , 0 },
 {"EV5"     , WD_OUT , 35 , 0 },
 {"EV6"     , WD_OUT , 32 , 0 },
 {"EV7"     , WD_OUT , 33 , 0 },*/
 {"APN"     , WD_OUT , 17 , 0 },
 {"FLASH0"  , WD_OUT , 2  , 0 },
 {"FLASH1"  , WD_OUT , 12 , 0 },
 {"FLASH2"  , WD_OUT , 13 , 0 }
};

WD_device_t wd_trigger[NB_DEV] = {
 {"TRIG0"     , WD_IN , 3 , 0 },
 {"TRIG1"     , WD_IN , 1 , 0 },
 {"TRIG2"     , WD_IN , 22 , 0 },
 {"TRIG3"     , WD_IN , 23 , 0 },
};
