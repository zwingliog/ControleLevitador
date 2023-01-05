// Controles para Arduino UNO ou Nano sem usar inversores
// Gera sinais nos pinos (9, 10)-Timer1 e (11, 3)-Timer2.
// Para usar com shield, ligar os pinos 9 e 11 ou 10 e 3.

// Usa as variaveis globais:
// Imax -> int
// faseAtual -> float
// faseAtualReal -> float
//
// Usa as funcoes:
//
// 
// Contem as seguintes funcoes:
// configuraTimer12()
// mudaImax12( int newImax, float oldFase )
// mudaFase12( float newFase, bool mostra )

void configuraTimer12(){
  // Timer 1 usa os pinos 9 e 10 (OC1A=9, OC1B=10)
  pinMode( 9, OUTPUT);
  pinMode( 10, OUTPUT);
  TCCR1A = 80;
  TCCR1B = 8;
  OCR1A = Imax;
  OCR1B = Imax;
  // para que OCB seja o inverso de OCA:
  if ( ((PINB & 0b00000010)==0) == ((PINB & 0b00000100)==0) ){
    // Se o OC1B estiver em fase com o OC1A
    TCCR1C = 64; // Inverte o sinal do OC1B
  }
  TCCR1B = 9;

  
  // Timer 2 usa os pinos 11 e 3 (OC2A=11, OC2B=3)
  pinMode( 11, OUTPUT);
  pinMode( 3, OUTPUT);
  TCCR2A = 82;
  TCCR2B = 0;
  OCR2A = Imax;
  OCR2B = Imax;
  // para que OCB seja o inverso de OCA:
  if ( ((PINB & 0b00001000)==0) == ((PIND & 0b00001000)==0) ){
    // Se o OC2B estiver em fase com o OC2A
    TCCR2B = 64; // Inverte o sinal do OC2B
  }
  TCCR2B = 1;

  // Para sincronizar os contadores
  mudaFase( faseAtual, false );
}


void mudaImax12( int newImax, float oldFase ){
  Imax = newImax;
  noInterrupts();
  GTCCR = 131;
  OCR1A = Imax;
  OCR1B = Imax;
  OCR2A = Imax;
  OCR2B = Imax;
  GTCCR = 0;
  interrupts();
  mudaFase( oldFase, false );
}


void mudaFase12( float newFase, bool mostra ){
  //update ocr registers with the value
  int Ip = Imax + 1;
  int Ip2 = 2 * Ip;
  float nFase = newFase;
  while( nFase<0.0 ){
    nFase = nFase + 360.0;
  }
  while( nFase>=360.0){
    nFase = nFase - 360.0;
  }
  int newD = (nFase/360.0)*Ip2 + 0.4999999;
  float newFaseReal = (newD*360.0)/Ip2;

  boolean emFaseAGORA = true;
  boolean emFasePROXIMA = true;
  if (newD>=Ip){
    newD = newD - Ip;
    emFasePROXIMA = false;
  }
  int tcnt2 = 0;
  int tcnt1 = tcnt2 + newD;
  if( tcnt1 >= Imax ){
    emFasePROXIMA = !emFasePROXIMA;
    tcnt2 = tcnt1-Imax+1;
    tcnt1 = 0;
  }
  noInterrupts();
  while( TCNT2>10 ){}
  GTCCR = 131;
  TCNT1 = tcnt1;
  TCNT2 = tcnt2;
  if ( ((PINB & 0b00000010)==0) != ((PINB & 0b00001000)==0) ){
    // OCA1 diferente de OCA2
    emFaseAGORA = false;
  }
  if (emFaseAGORA != emFasePROXIMA){
    TCCR1C = 192;
  }
  GTCCR = 0;
  interrupts();
  
  if ( (ESCREVE) && (mostra) ){
    Serial.print( F("f=") );
    Serial.print( newFase );
    Serial.print( F(" fR=") );
    Serial.print( newFaseReal );
    Serial.print( F(", D=") );
    Serial.print( newD );
    Serial.print( F(", t1=") );
    Serial.print( tcnt1 );
    Serial.print( F(", t2=") );
    Serial.println( tcnt2 );
  }
  faseAtual = newFase;
  faseAtualReal = newFaseReal;
}
