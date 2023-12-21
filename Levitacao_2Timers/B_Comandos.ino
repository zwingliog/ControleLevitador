// Comandos (funcoes associadas aos comandos escolhidos pelo usuario)
// Precisa das variaveis globais:
//   DelayT -> float
//   nControles
//   modoControle
//   valorControle

void fazOnOffCycles(float DtOff, float DtRep, long nCiclos, float DtZero){
  if (DEBUG){
    Serial.println( F("fazOnOffCycles(DtOff, DtRep, nCiclos, DtZero)") );
    Serial.print( F("fazOnOffCycles(") );
    Serial.print( DtOff );
    Serial.print( ", " );
    Serial.print( DtRep );
    Serial.print( ", " );
    Serial.print( nCiclos );
    Serial.print( ", " );
    Serial.print( DtZero );
    Serial.println( ")" );
  }
  int potOnAtu = potAtual;
  unsigned long DtOff_uL = DtOff * 1000uL;
  unsigned long DtRep_uL = DtRep * 1000uL;
  unsigned long Limite1 = max(tNEXT, micros()+(DtZero*1000uL));;
  unsigned long Limite2 = 0;
  for (unsigned long i=0; i<nCiclos; i++){
    if (i>0){
      Limite1 += DtRep_uL;
    }
    Limite2 = Limite1 + DtOff_uL;
    while (micros()<Limite1){}
    mudaPot( 0 );
    while (micros()<Limite2){}
    mudaPot( potOnAtu );
    if (DEBUG){
      Serial.print( Limite1 );
      Serial.print( " a " );
      Serial.println( Limite2 );
    }
  }
  WAITING = false;
  tLAST = Limite2;
  tNEXT = 0;
}




void fazJumps(float DF, float DtRep, long nJumps, float DtZero){
  if (DEBUG){
    Serial.println( F("fazJumps(DF, DtRep, nJumps, DtZero)") );
    Serial.print( F("fazJumps(") );
    Serial.print( DF );
    Serial.print( ", " );
    Serial.print( DtRep );
    Serial.print( ", " );
    Serial.print( nJumps );
    Serial.print( ", " );
    Serial.print( DtZero );
    Serial.println( ")" );
  }
  float F1 = faseAtual;
  float F2 = faseAtual+DF;
  if (ESCREVE){
    Serial.println( F1 );
    Serial.println( F2 );
  }
  float newFase = F2;
  unsigned long DtRep_uL = DtRep * 1000uL;
  unsigned long Limite = max(tNEXT, micros()+(DtZero*1000uL));
  for (long i=0; i<nJumps; i++ ){
    if (i>0){
      Limite += DtRep_uL;
    }
    while (micros()<Limite) {}
    mudaFase( newFase, false );
    newFase = F1+F2-newFase;
  }
  WAITING = false;
  tLAST = Limite;
  tNEXT = 0;
}




void fazSteps(float DF, long Steps, float Dt, float DtZero){
  if (DEBUG){
    Serial.println( F("fazSteps(DF, Steps, Dt, DtZero)") );
    Serial.print( F("fazSteps(") );
    Serial.print( DF );
    Serial.print( ", " );
    Serial.print( Steps );
    Serial.print( ", " );
    Serial.print( Dt );
    Serial.print( ", " );
    Serial.print( DtZero );
    Serial.println( ")" );
  }
  unsigned long Dt_uL = Dt*1000uL;
  float Fini = faseAtual;
  float Ffin = Fini + DF;
  unsigned long Limite = max(tNEXT, micros()+(DtZero*1000uL));
  for (unsigned long i=0; i<=Steps; i++){
    if (i>0){
      Limite += Dt_uL;
    }
    float Fatu = Fini + float(i*(Ffin-Fini))/max(Steps,1);
    while( micros()<Limite ){}
    mudaFase( Fatu, false );
  }
  WAITING = false;
  tLAST = Limite;
  tNEXT = 0;
}




void mostraImax(){
  Serial.print( F("iMax=") );
  Serial.print( Imax );
  Serial.print( " (" );
  Serial.print( 8000.0/(Imax+1) );
  Serial.println( "kHz)" );
}

void mostraFase(){
  Serial.print( F("phase=") );
  Serial.println( faseAtual );
  Serial.print( F("real phase=") );
  Serial.println( faseAtualReal );
}

void mostraPot(){
  Serial.print( F("Pot=") );
  Serial.print( potAtual );
  if (potAtual==0){
    Serial.println( F(" :off*2") );
  }else if (potAtual==1){
    Serial.println( F(" :on*2") );
  }else if (potAtual==2){
    Serial.println( F(" :on-off") );
  }else if (potAtual==3){
    Serial.println( F(" :off-on") );
  }
}

void mostraDelayT(){
  Serial.print( F("DelayT=") );
  Serial.println( DelayT );
  Serial.print( F("DelayTRep=") );
  Serial.println( DelayTRep );
}

void mostraMemos(){
  Serial.print( F("number of memos: ") );
  Serial.println( nMemos );
  for (int i=1; i<=nMemos; i++){  
    mostraMemos(i);
  }
}
void mostraMemos( int QUAL){
  if (ehMemoValido(QUAL)){
    if (memos[QUAL-1].length()>0){
      Serial.print( F("M") );
      Serial.print( QUAL );
      Serial.print( ":" );
      Serial.println( memos[QUAL-1] );
    }
  }
}

void mostraControles(){
  for (int qC=0; qC<nControles; qC++){
    if (qC>0)
      Serial.print( F(" ") );
    mostraControles(qC,false);
  }
  Serial.println();
}
void mostraControles(int QUAL){
  mostraControles(QUAL, true);
}
void mostraControles(int QUAL, bool TERMINA){
  if (ehControleValido(QUAL)){
    Serial.print( F("A") );
    Serial.print( QUAL );
    Serial.print( F(":") );
    Serial.print( (valorControle[QUAL]==LOW)? "L": "H" );
    Serial.print( F(",") );
    Serial.print( (modoControle[QUAL]==OUTPUT)? "O": "I" );
    if (TERMINA)
      Serial.println();
  }
}



void mostraStatus(){
  mostraImax();
  mostraFase();
  mostraPot();
  mostraDelayT();
  mostraMemos();
  mostraControles();
  //Serial.print( F("micros/1e6=") );
  //Serial.println( micros()/1e6 );
}



void mostraHelpComandos(){
  mostraHelpComandos( false );
}
void mostraHelpComandos(bool DETALHADO){
  Serial.println( F("List of valid commands:") );
  Serial.println( F("? > status [ ? ]") );
  Serial.println( F("w > wait (short) [ w DelayT ] (ms)") );
  Serial.println( F("W > wait (long) [ W DelayTRep ] (s)") );
  Serial.println( F("t > set DelayT [ t newDelayT ]  (ms)") );
  Serial.println( F("T > set DelayTRep [ t newDelayTRep ] (s)") );
  Serial.println( F("p > set Pot [ p newP ]  p?->mostra Pot") );
  Serial.println( F("f > set Phase [ f newF ]  f?->mostra Phase") );
  Serial.println( F("o > off [ o DtOff ]") );
  Serial.println( F("c > on-off cycles [ c DtOff, DtRep, nCycles ]") );
  Serial.println( F("j > jump [ j DeltaF, DtRep, nJumps ]") );
  Serial.println( F("u > up (360 degrees) [ u Dt ]") );
  Serial.println( F("d > down (360 degrees) [ d Dt ]") );
  Serial.println( F("s > switch (in steps) [ s DF, Dt ]") );
  Serial.println( F("S > switch (in steps) [ S DF, Steps, Dt ]") );
  if (DETALHADO){
    Serial.println( F("i > define Imax [ i Imax ]   i?->mostra IMax") );
    Serial.println( F("A > set port A0 to A5 [ A 0-5, H_L_I_O ] *A?->mostra") );
    Serial.println( F("M > recover from Memory [ M 1-9 ] M?->mostra Memos" ) );
    Serial.println( F("> > save to Memory [ > 1-9: @ ] >?->mostra Memos") );
    Serial.println( F("R > Repeat [ R(#){@} ]") );
    Serial.println( F("_ > set global variable [_@_VALOR] ...") );
    Serial.println( F("    where @=(DEBUG, ESCREVE, REVERSO, DtZeroPadrao)") );
    Serial.println( F("    true, false or value in us for DtZeroPadrao") );
  }
  Serial.println( F("h > help [ h ]=simple help,  [h1]=full help") );
  if (DETALHADO){
    Serial.println( F(" OBS1: in 'S': Steps may be '!' ou '$':") );
    Serial.println( F("              '!'->2*(iMax+1) e '$'->(iMax+1) per 360 degrees") );
    Serial.println( F(" OBS2: in 'f': '*'+DF=current phase+DF") );
  }
}
