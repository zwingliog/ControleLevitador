// Comandos (funcoes associadas aos comandos escolhidos pelo usuario)
// Precisa das variaveis globais:
//   DelayT -> float
//   nControles
//   modoControle
//   valorControle
//   memos
//   botoes

//void fazOff(float DtOff){
void fazOff(unsigned long DtOff_uL){
  if (DEBUG){
    Serial.print( F("fazOff(DtOff=") );
    Serial.print( DtOff_uL );
    Serial.println( ")" );
  }
  int potOnAtu = potAtual;
  unsigned long dtNEXT1 = dtNEXT; 
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dtNEXT1 = DtZero_uL;
  }
  dtNEXT = dtNEXT1 + DtOff_uL;
  while ( (micros()-tLAST)<dtNEXT1 ){}
  mudaPot( (potOnAtu==0) );
  while ( (micros()-tLAST)<dtNEXT ){}
  mudaPot( potOnAtu );
  tLAST += dtNEXT;
  dtNEXT = 0;
}

//void fazOnOffCycles(DtOff, DtRep, nCiclos){
void fazOnOffCycles(unsigned long DtOff_uL, 
                    unsigned long DtRep_uL, long nCiclos){
  if (DEBUG){
    //Serial.println( F("fazOnOffCycles(DtOff, DtRep, nCiclos)") );
    Serial.print( F("fazOnOffCycles(DtOff=") );
    Serial.print( DtOff_uL );
    Serial.print( F(", DtR=") );
    Serial.print( DtRep_uL );
    Serial.print( F(", nC=") );
    Serial.print( nCiclos );
    Serial.println( ")" );
  }
  int potOnAtu = potAtual;
  int potOffAtu = (potOnAtu==0);
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dtNEXT = DtZero_uL;
  }
  unsigned long dtNEXT2 = dtNEXT + DtOff_uL;
  unsigned long DtOn_uL = (DtRep_uL>DtOff_uL)? DtRep_uL-DtOff_uL : 0L;
  for (unsigned long i=0; i<nCiclos; i++){
    while ( (micros()-tLAST)<dtNEXT ){}
    mudaPot( potOffAtu );
    while ( (micros()-tLAST)<dtNEXT2 ){}
    mudaPot( potOnAtu );
    tLAST += dtNEXT2;
    dtNEXT = DtOn_uL;
    dtNEXT2 = DtRep_uL;
  }
  //dtNEXT = 0;
}



//void fazJumps(DF, DtRep, nJumps)
void fazJumps(float DF, long DtRep_uL, long nJumps){
  if (DEBUG){
    //Serial.println( F("fazJumps(DF, DtRep_uL, nJumps)") );
    Serial.print( F("fazJumps(DF=") );
    Serial.print( DF );
    Serial.print( F(", DtR=") );
    Serial.print( DtRep_uL );
    Serial.print( F(", nJ=") );
    Serial.print( nJumps );
    Serial.println( ")" );
  }
  float F1 = faseAtual;
  float F2 = faseAtual+DF;
  if (ESCREVE){
    Serial.println( F1 );
    Serial.println( F2 );
  }
  float newFase = F2;
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dtNEXT = DtZero_uL;
  }
  for (long i=0; i<nJumps; i++ ){
    while ( (micros()-tLAST)<dtNEXT) {}
    mudaFase( newFase, false );
    newFase = F1+F2-newFase;
    tLAST += dtNEXT;
    dtNEXT = DtRep_uL;
  }
  //dtNEXT = 0;
}


// fazSteps(DF, Steps, Dt)
void fazSteps(float DF, long Steps, long Dt_uL){
  if (DEBUG){
    //Serial.println( F("fazSteps(DF, Steps, Dt_uL)") );
    Serial.print( F("fazSteps(DF=") );
    Serial.print( DF );
    Serial.print( F(", nS=") );
    Serial.print( Steps );
    Serial.print( F(", Dt=") );
    Serial.print( Dt_uL );
    Serial.println( ")" );
  }
  float Fini = faseAtual;
  //float Ffin = Fini + DF;
  float DFperStep = DF/float(max(Steps,1));
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dtNEXT = DtZero_uL;
  }
  for (long i=0; i<=Steps; i++){
    float Fatu = Fini + float(i*DFperStep);
    while( (micros()-tLAST)<dtNEXT ){}
    mudaFase( Fatu, false );
    tLAST += dtNEXT;
    dtNEXT = Dt_uL;
  }
  //dtNEXT = 0;
}
