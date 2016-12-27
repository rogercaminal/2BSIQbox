//--- Output pins to fire relays and LEDs
int pin_Rx1    = 10;
int pin_Rx2    = 11;
int pin_Tx1    = 8;
int pin_Tx2    = 9;

//--- Input pins to get PTT signal
int pin_PTT1 = 2;
int pin_PTT2 = 3;

bool tx1;
bool tx2;
enum TX_RADIO{NONE, R1, R2};
enum RX_MODES{ALL_R1, ALL_R2, BOTH, AUTO};

TX_RADIO tx = TX_RADIO::NONE;
RX_MODES mode_rx = RX_MODES::AUTO;

bool rx_switch; // 0 if L, 1 if R
bool rx_stereo; // 0 if mono, 1 if stereo

//__________________________________________________________________
void setup() {
  // put your setup code here, to run once:

  pinMode(pin_Rx1, OUTPUT); // HIGH when RX on radio 1
  pinMode(pin_Rx2, OUTPUT); // HIGH when RX on radio 2

  pinMode(pin_Tx1, OUTPUT); // HIGH when TX on radio 1
  pinMode(pin_Tx2, OUTPUT); // HIGH when TX on radio 2
  
  digitalWrite(pin_Rx1, LOW);
  digitalWrite(pin_Rx2, LOW);
  
  pinMode(pin_PTT1, INPUT_PULLUP);
  pinMode(pin_PTT2, INPUT_PULLUP);

  tx = TX_RADIO::NONE;

  Serial.begin(9600);
}

//__________________________________________________________________
void loop() {
  // put your main code here, to run repeatedly:

  //--- TX
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

  //--- RX
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

  //--- Modify once relays are available
  //--- RX
  if (rx_switch==0 && rx_stereo==0){
    digitalWrite(pin_Rx1, HIGH);
    digitalWrite(pin_Rx2, LOW);
  }
  else if (rx_switch==1 && rx_stereo==0){
    digitalWrite(pin_Rx1, LOW);
    digitalWrite(pin_Rx2, HIGH);
  }
  else if (rx_stereo==1){
    digitalWrite(pin_Rx1, HIGH);
    digitalWrite(pin_Rx2, HIGH);
  }
  
  //--- TX
  if (tx==TX_RADIO::R1){
    digitalWrite(pin_Tx1, HIGH);
    digitalWrite(pin_Tx2, LOW);
  }
  else if (tx==TX_RADIO::R2){
    digitalWrite(pin_Tx1, LOW);
    digitalWrite(pin_Tx2, HIGH);
  }
  else{
    digitalWrite(pin_Tx1, LOW);
    digitalWrite(pin_Tx2, LOW);
  }
  

  Serial.print(tx);
  Serial.print("\n");



}


