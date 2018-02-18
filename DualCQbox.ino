//--- Input pins for the RX switch
int pin_switch_RX1  = A3;
int pin_switch_AUTO = A2;
int pin_switch_RX2  = A1;
int pin_switch_BOTH = A0;

//--- Output pins to fire relays and LEDs
int pin_LED_Rx1    = 9;
int pin_LED_Rx2    = 11;
int pin_LED_Tx1    = 8;
int pin_LED_Tx2    = 10;

//--- Input pins to get TX signal
int pin_TX1 = 3;
int pin_TX2 = 5;

//--- Output pins to send inhibit signal
int pin_inh1 = 4;
int pin_inh2 = 6;

//--- Output pins for the RX relays
int pin_relay_switch = 2;
int pin_relay_stereo = 13;

//--- RX and TX states
enum TX_RADIO{NONE, R1, R2};
enum RX_MODES{ALL_R1, ALL_R2, BOTH, AUTO, INVALIDMODE};

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
  
  pinMode(pin_TX1, INPUT_PULLUP);
  pinMode(pin_TX2, INPUT_PULLUP);
  pinMode(pin_inh1, OUTPUT);
  pinMode(pin_inh2, OUTPUT);

  pinMode(pin_switch_RX1, INPUT_PULLUP);
  pinMode(pin_switch_BOTH, INPUT_PULLUP);
  pinMode(pin_switch_AUTO, INPUT_PULLUP);
  pinMode(pin_switch_RX2, INPUT_PULLUP);

  pinMode(pin_relay_switch, OUTPUT);
  pinMode(pin_relay_stereo, OUTPUT);

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

  //--- Get TX signals from rigs
  read_TX();
  
  //--- set RX depending on RX_mode and TX
  set_RX();

  //--- set TX LEDs and inhibits
  set_TX();

//  Serial.print(mode_rx);
//  Serial.print("\n");
}


//_________________________________________________
void get_RX_mode()
{
  if (digitalRead(pin_switch_RX1)==LOW){
    mode_rx = RX_MODES::ALL_R1;
  }
  else if (digitalRead(pin_switch_AUTO)==LOW){
    mode_rx = RX_MODES::AUTO;
  }
  else if (digitalRead(pin_switch_RX2)==LOW){
    mode_rx = RX_MODES::ALL_R2;
  }
  else if (digitalRead(pin_switch_BOTH)==LOW){
    mode_rx = RX_MODES::BOTH;
  }
  else{
    mode_rx = RX_MODES::INVALIDMODE;
  }
}


//_________________________________________________
void read_TX()
{
  if (digitalRead(pin_TX1)==HIGH && digitalRead(pin_TX2)==HIGH){
    tx=TX_RADIO::NONE;
//    Serial.print("Read_TX: 0\n");
  }
  else if (digitalRead(pin_TX1)==LOW && tx!=TX_RADIO::R2)
  {
    tx=TX_RADIO::R1;
//    Serial.print("Read_TX: 1\n");
  }
  else if (digitalRead(pin_TX2)==LOW && tx!=TX_RADIO::R1)
  {
    tx=TX_RADIO::R2;
//    Serial.print("Read_TX: 2\n");
  }
  else{
    tx=TX_RADIO::NONE;
//    Serial.print("Read_TX: 3\n");
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
  if (mode_rx==RX_MODES::INVALIDMODE){
    digitalWrite(pin_LED_Rx1, LOW);
    digitalWrite(pin_LED_Rx2, LOW);
  }

  //--- RX relays
  if (rx_switch){
    digitalWrite(pin_relay_switch, HIGH);
//    Serial.print("pin_relay_switch: HIGH\n");
  }
  else{
    digitalWrite(pin_relay_switch, LOW);
//    Serial.print("pin_relay_switch: LOW\n");
  }
  if (rx_stereo){
    digitalWrite(pin_relay_stereo, HIGH);
//    Serial.print("pin_relay_stereo: HIGH\n");
  }
  else{
    digitalWrite(pin_relay_stereo, LOW);
//    Serial.print("pin_relay_stereo: LOW\n");
  }
//  Serial.print("\n");
  
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
