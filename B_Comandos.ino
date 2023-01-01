// Comandos (funcoes associadas aos comandos escolhidos pelo usuario)
// Precisa das variaveis globais:
//   DelayT -> float
//   nControles
//   modoControle
//   valorControle

void fazOnOffCycles(float DtOff, float Dt, long nCiclos, float DtZero){
  unsigned long t0 = micros();
  for (long i=0; i<nCiclos; i++){
    unsigned long Limite1 = t0 + (i*Dt + DtZero)*1000uL;
    unsigned long Limite2 = t0 + (i*Dt + DtOff + DtZero)*1000uL;
    while (micros()<Limite1){}
    mudaPot( 0 );
    while (micros()<Limite2){}
    mudaPot( 1 );
  }
}




void fazJumps(float DF, float Dt, long nJumps, float DtZero){
  float F1 = faseAtual;
  float F2 = faseAtual+DF;
  float newFase = F2;
  Serial.println( F1 );
  Serial.println( F2 );
  unsigned long t0 = micros();
  for (long i=0; i<nJumps; i++ ){
    unsigned long Limite = t0 + (i*Dt + DtZero)*1000uL;
    while (micros()<Limite) {}
    mudaFase( newFase, false );
    newFase = F1+F2-newFase;
  }
}




void fazSteps(float Fini, float Ffin, long Steps, float Dt, float DtZero){
  unsigned long t0 = micros();
  for (long i=0; i<=Steps; i++){
    unsigned long Limite = t0 + (i*Dt + DtZero)*1000uL;
    float Fatu = Fini + float(i*(Ffin-Fini))/max(Steps,1);
    while( micros()<Limite ){}
    mudaFase( Fatu, false );
  }
}






void mostraStatus(){
      Serial.print( F("iMax=") );
      Serial.print( Imax );
      Serial.print( F(" (") );
      Serial.print( 8000.0/(Imax+1) );
      Serial.println( F("kHz)") );
      Serial.print( F("phase=") );
      Serial.println( faseAtual );
      Serial.print( F("real phase=") );
      Serial.println( faseAtualReal );
      Serial.print( F("DelayT=") );
      Serial.println( DelayT );
      Serial.print( F("Pot=") );
      Serial.println( potAtual );
      Serial.print( F("EnA:") );
      Serial.print( pinEnA );
      Serial.print( F(" EnB:") );
      Serial.println( pinEnB );
      for (int qC=0; qC<nControles; qC++){
        if (qC>0)
          Serial.print( F(" ") );
        Serial.print( F("A") );
        Serial.print( qC );
        Serial.print( F(":") );
        Serial.print( (valorControle[qC]==LOW)? "L": "H" );
        Serial.print( F(",") );
        Serial.print( (modoControle[qC]==OUTPUT)? "O": "I" );

      }
      Serial.println();
      //Serial.print( F("micros/1e6=") );
      //Serial.println( micros()/1e6 );
}



void mostraHelpComandos(){
  mostraHelpComandos( false );
}
void mostraHelpComandos(bool DETALHADO){
  Serial.println( F("List of valid commands:") );
  Serial.println( F("? > status [ ? ]") );
  Serial.println( F("w > wait [ w DelayT ]") );
  Serial.println( F("t > set DelayT [ t newDelayT ]") );
  Serial.println( F("p > set Pot [ p newP ]") );
  Serial.println( F("f > set Phase [ f newF ]") );
  Serial.println( F("o > off [ o DtOff ]") );
  Serial.println( F("c > on-off cycles [ c DtOff, Dt, nCycles ]") );
  Serial.println( F("j > jump [ j DeltaF, Dt, nJumps ]") );
  Serial.println( F("u > up (360 degrees) [ u ]") );
  Serial.println( F("d > down (360 degrees) [ d ]") );
  Serial.println( F("s > switch (in steps) [ s Fini, Ffin ]") );
  Serial.println( F("S > switch (in steps) [ S Fini, Ffin, Steps, Dt ]") );
  if (DETALHADO){
    Serial.println( F("i > define Imax [ i Imax ]") );
    Serial.println( F("a > set port A0 to A5 [ a {0-5}, {H,L,I,O} ]") );
  }
  Serial.println( F("h > help [ h ]=simple help,  [h1]=full help") );
  if (DETALHADO){
    Serial.println( F(" OBS1: in 'S': Steps may be '#' ou '$':") );
    Serial.println( F("              '#'->2*(iMax+1) e '$'->(iMax+1) per 360 degrees") );
    Serial.println( F(" OBS2: in 's','S','f': '*'=current phase") );
    Serial.println( F("                       '*'+DF=current phase+DF") );
  }
}
