//--- Input pins for the RX switch
int pin_switch_RX1  = A0;
int pin_switch_BOTH = A1;
int pin_switch_AUTO = A2;
int pin_switch_RX2  = A3;

//--- Output pins to fire relays and LEDs
int pin_LED_Rx1    = 11;
int pin_LED_Rx2    = 10;
int pin_LED_Tx1    = 9;
int pin_LED_Tx2    = 8;

//--- Input pins to get PTT signal
int pin_PTT1 = 2;
int pin_PTT2 = 3;

//--- Output pins to send inhibit signal
int pin_inh1 = 4;
int pin_inh2 = 5;

//--- Output pins for the RX relays
int pin_relay_switch = 6;
int pin_relay_stereo = 7;

//--- RX and TX modes
enum TX_RADIO{NONE, R1, R2};
enum RX_MODES{ALL_R1, ALL_R2, BOTH, AUTO};

//--- GLobal variables
RX_MODES mode_rx = RX_MODES::ALL_R1;
TX_RADIO tx = TX_RADIO::NONE;
bool rx_switch; // 0 if L, 1 if R
bool rx_stereo; // 0 if mono, 1 if stereo

//__________________________________________________________________
void setup() {
  // put your setup code here, to run once:

  //--- Pins
  pinMode(pin_LED_Rx1, OUTPUT); // HIGH when RX on radio 1
  pinMode(pin_LED_Rx2, OUTPUT); // HIGH when RX on radio 2

  pinMode(pin_LED_Tx1, OUTPUT); // HIGH when TX on radio 1
  pinMode(pin_LED_Tx2, OUTPUT); // HIGH when TX on radio 2
  
  pinMode(pin_PTT1, INPUT_PULLUP);
  pinMode(pin_PTT2, INPUT_PULLUP);
  pinMode(pin_inh1, OUTPUT);
  pinMode(pin_inh2, OUTPUT);

  pinMode(pin_switch_RX1, INPUT_PULLUP);
  pinMode(pin_switch_BOTH, INPUT_PULLUP);
  pinMode(pin_switch_AUTO, INPUT_PULLUP);
  pinMode(pin_switch_RX2, INPUT_PULLUP);

  //--- Initial state
  digitalWrite(pin_LED_Rx1, LOW);
  digitalWrite(pin_LED_Rx2, LOW);
  digitalWrite(pin_LED_Tx1, LOW);
  digitalWrite(pin_LED_Tx2, LOW);
  digitalWrite(pin_inh1, HIGH); // inhibit when shortcut
  digitalWrite(pin_inh2, HIGH); // inhibit when shortcut
  tx = TX_RADIO::NONE;
  get_RX_mode();

  //--- Init Serial
  Serial.begin(9600);
}

//__________________________________________________________________
void loop() {
  // put your main code here, to run repeatedly:

  //--- Read RX mode from switch
  get_RX_mode();

  //--- Get PTT signals from rigs
  read_PTTs();
  
  //--- set RX depending on RX_mode and PTTs
  set_RX();

  //--- set TX LEDs and inhibits
  set_TX();

  Serial.print(mode_rx);
  Serial.print("\n");
}


//_________________________________________________
void get_RX_mode()
{
  if (digitalRead(pin_switch_RX1)==LOW){
    mode_rx = RX_MODES::ALL_R1;
  }
  if (digitalRead(pin_switch_BOTH)==LOW){
    mode_rx = RX_MODES::BOTH;
  }
  if (digitalRead(pin_switch_AUTO)==LOW){
    mode_rx = RX_MODES::AUTO;
  }
  if (digitalRead(pin_switch_RX2)==LOW){
    mode_rx = RX_MODES::ALL_R2;
  }
}


//_________________________________________________
void read_PTTs()
{
  if (digitalRead(pin_PTT1)==HIGH && digitalRead(pin_PTT2)==HIGH){
    tx=TX_RADIO::NONE;
  }
  else if (digitalRead(pin_PTT1)==LOW && tx!=TX_RADIO::R2)
  {
    tx=TX_RADIO::R1;
  }
  else if (digitalRead(pin_PTT2)==LOW && tx!=TX_RADIO::R1)
  {
    tx=TX_RADIO::R2;
  }
  else{
    tx=TX_RADIO::NONE;
  }
}


//_________________________________________________
void set_RX()
{
  if (mode_rx==RX_MODES::ALL_R1){
    rx_switch = 0;
    rx_stereo = 0;
  }
  else if (mode_rx==RX_MODES::ALL_R2){
    rx_switch = 1;
    rx_stereo = 0;
  }
  else{ //--- BOTH or AUTO
    rx_switch = 0;
    rx_stereo = 1;
    if (mode_rx==RX_MODES::AUTO && tx==TX_RADIO::R2){
      rx_switch = 0;
      rx_stereo = 0;
    }
    else if (mode_rx==RX_MODES::AUTO && tx==TX_RADIO::R1){
      rx_switch = 1;
      rx_stereo = 0;
    }
  }

  //--- RX LEDs
  if (rx_switch==0 && rx_stereo==0){
    digitalWrite(pin_LED_Rx1, HIGH);
    digitalWrite(pin_LED_Rx2, LOW);
  }
  else if (rx_switch==1 && rx_stereo==0){
    digitalWrite(pin_LED_Rx1, LOW);
    digitalWrite(pin_LED_Rx2, HIGH);
  }
  else if (rx_stereo==1){
    digitalWrite(pin_LED_Rx1, HIGH);
    digitalWrite(pin_LED_Rx2, HIGH);
  }

  //--- RX relays
  if (rx_switch){
    digitalWrite(pin_relay_switch, HIGH);
  }
  else{
    digitalWrite(pin_relay_switch, LOW);
  }
  if (rx_stereo){
    digitalWrite(pin_relay_stereo, HIGH);
  }
  else{
    digitalWrite(pin_relay_stereo, LOW);
  }
}


//_________________________________________________
void set_TX(){
  if (tx==TX_RADIO::R1){
    digitalWrite(pin_LED_Tx1, HIGH);
    digitalWrite(pin_LED_Tx2, LOW);
    digitalWrite(pin_inh1, HIGH);
    digitalWrite(pin_inh2, LOW);
  }
  else if (tx==TX_RADIO::R2){
    digitalWrite(pin_LED_Tx1, LOW);
    digitalWrite(pin_LED_Tx2, HIGH);
    digitalWrite(pin_inh1, LOW);
    digitalWrite(pin_inh2, HIGH);

  }
  else{
    digitalWrite(pin_LED_Tx1, LOW);
    digitalWrite(pin_LED_Tx2, LOW);
    digitalWrite(pin_inh1, HIGH);
    digitalWrite(pin_inh2, HIGH);
  }
}
