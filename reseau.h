typedef uint_8 MAC;
typedef uint_8 IP;

typedef struct Station {
    MAC adrMAC;
    IP adrIP;
    char nom[32];
} Station;

typedef struct Hub {
    char nom[32]
    size_t nb_ports;
} Hub;

typedef struct Trame {
    MAC destination;
    MAC source;
    char data[1500];
} Trame;

typedef struct Commutation {
    MAC adrMAC;
    uint_32 port;
} Commutation;
    
typedef struct wafenSSwitch {
    char nom[32];
    MAC adrMAC;
    size_t nb_ports;
    uint_16 priorite;
    Commutation tabCommutation[nb_ports];
} wafenSSwitch;
