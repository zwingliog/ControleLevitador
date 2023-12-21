// Controles para arduino UNO ou Nano com inversores
// Gera sinais nos pinos do Timer1 (pinos 9 e 10)

// Usa as variaveis globais:
// Imax -> int
// DeltaOCR -> int
// faseAtual -> float
// faseAtualReal -> float
// REVERSO
//
// Usa as funcoes:
//
// 
// Contem as seguintes funcoes:
// configuraTimer1()
// mudaImax1( int newImax, float oldFase )
// mudaFase1( float newFase, bool mostra )

void configuraTimer1(){
  // Timer 1 usa os pinos 9 e 10
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);

  ICR1 = Imax ;
  OCR1A = DeltaOCR;
  OCR1B = DeltaOCR;
//  TCNT1 = 0;
  TCCR1A = 80; // 0x50 // 0b00110010

  TCCR1B = 24; // 0x18  //  0b00011000
  if ( ((PINB&B010)==0) != ((PINB&B100)==0) ){
    TCCR1C = 64; // 0x40  //  0b01000000
  }
  TCNT1=Imax-1;
  TCCR1B = 25; // 0x19  //  0b00011001    
}


void mudaImax1( int newImax, float oldFase ){
  Imax = newImax;
  int LimiteImax = newImax-40;
  while( TCNT1>LimiteImax ){}
  ICR1 = Imax;
  mudaFase( oldFase, false );
}



void mudaFase1( float newFase, bool mostra ){
    //update ocr registers with the value
    int Ip = Imax + 1;
    int Ip2 = 2 * Ip;
    int L1 = Ip / 2;
    int newOCR1A;
    int newOCR1B;
    float nFase = newFase;
    if (REVERSO){
      nFase = -nFase;
    }
    while( nFase<0.0 ){
      nFase = nFase + 360.0;
    }
    while( nFase>360.0){
      nFase = nFase - 360.0;
    }
    boolean newFF = false;
    int newD = (nFase/360.0)*Ip2 + 0.4999999;
    float newFaseReal = (newD*360.0)/Ip2;
//        int regiao = 0;
    if(newD<=L1){
//          regiao = 1; // 1A no 0 e 1B entre 0 e Ip/2, não inverte
      newOCR1A = DeltaOCR;
      newOCR1B = DeltaOCR + newD;
      newFF = false;
    } else if(newD<=Ip){ 
//          regiao = 2; //  1A de Ip/2 a 0,  1B no 0, inverte
      newOCR1A = DeltaOCR + Ip-newD;
      newOCR1B = DeltaOCR;
      newFF = true;
    } else if(newD<=(Ip+L1)){
//          regiao = 3; // 1A no 0 e 1B entre 0 e Ip/2, inverte
      newOCR1A = DeltaOCR;
      newOCR1B = DeltaOCR + newD-Ip;
      newFF = true;
    } else {
//          regiao = 4; //  1A de Ip/2 a 0,  1B no 0, não inverte
      newOCR1A = DeltaOCR + Ip2-newD;
      newOCR1B = DeltaOCR;
      newFF = false;
    }

    int TC1;
    int LimInf = L1 + DeltaOCR + 1;
    int LimSup = Imax-10;
    bool OLDfazFF = (newFF != oldFF);
    bool fazFF;
    noInterrupts();
    do{
      TC1 = TCNT1;
    }while( (TC1<=LimInf) || (TC1>=LimSup) );
    TCCR1B = 24;
    int TC1b = TCNT1;
    byte estadosPortaB = PINB;
    fazFF = ( ( ((estadosPortaB & B010)!=0)==((estadosPortaB & B100)!=0) ) != !newFF );
    if (fazFF){
      TCCR1C = 64;
    }
    OCR1A = newOCR1A;
    OCR1B = newOCR1B;
    TCCR1B = 25;
    interrupts();
    if ( (fazFF != OLDfazFF) ){ 
      //para o caso de haver um erro
      Serial.print( "nF=" );
      Serial.print( newFase );
      Serial.print( " nFR=" );
      Serial.print( newFaseReal );
      Serial.print( " pB=" );
      Serial.print( estadosPortaB, BIN );
      Serial.print( " pB1=" );
      Serial.print( (estadosPortaB & B010)!=0, BIN );
      Serial.print( " pB2=" );
      Serial.print( (estadosPortaB & B100)!=0, BIN );
      Serial.print( " fFF=" );
      Serial.print( fazFF );
      Serial.print( " oFF=" );
      Serial.print( oldFF );
      Serial.print( " nFF=" );
      Serial.print( newFF );
      Serial.print( "->" );
      Serial.println( OLDfazFF );
      newFF = !newFF;
    }
    oldFF = newFF;
    if ((ESCREVE)&&(mostra)){
      Serial.print( F("f=") );
      Serial.print( newFase );
      Serial.print( F(" fR=") );
      Serial.print( newFaseReal );
      Serial.print( F(", D=") );
      Serial.print( newD );
      Serial.print( F(", OCR1A=") );
      Serial.print( OCR1A );
      Serial.print( F(", OCR1B=") );
      Serial.println( OCR1B );
    }
    faseAtual = newFase;
    faseAtualReal = newFaseReal;
}
