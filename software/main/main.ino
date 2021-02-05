#define P_RQST (4)
#define SR_RST (3)
#define WD_KICK (2)

#define BD_LED (13)

#define INT_TEMP (16)
#define EXT_TEMP (15)
#define POT (14)


void setup() {
    analogReference( EXTERNAL );
    pinMode(P_RQST, OUTPUT);
    pinMode(SR_RST, OUTPUT);
    pinMode(WD_KICK, OUTPUT);
    pinMode(INT_TEMP, INPUT);
    pinMode(EXT_TEMP, INPUT);
    pinMode(POT, INPUT);
    pinMode(BD_LED, OUTPUT);

    // Startup sequence
    // Set outputs
    digitalWrite(P_RQST, LOW);
    digitalWrite(SR_RST, LOW);
    digitalWrite(WD_KICK, LOW);
    // System settle
    delay(100);
    // Kick
    digitalWrite(WD_KICK, HIGH);
    delay(500);
    digitalWrite(WD_KICK, LOW);
    delay(100);
    // Reset SR
    digitalWrite(SR_RST, HIGH);
    delay(500);
    digitalWrite(SR_RST, LOW);

}

void loop() {
    kick();

    bool st = (millis() / 5000 ) % 2;
    digitalWrite(BD_LED, st);
    digitalWrite(P_RQST, st);
}


static void kick() {
    static unsigned long last = 0;
    unsigned long t = millis();
    // max system runtime
    //if( t > ( 1000UL * 60UL * 1UL ) )
    //    while(true);
    // min WD timeout 1 sec
    if( (t - last) < 800UL )
        return;
    last = t;

    digitalWrite( WD_KICK, !digitalRead(WD_KICK) );
}

