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
    Serial.println( F("fazOff(DtOff)") );
    Serial.print( F("fazOff(") );
    Serial.print( DtOff_uL );
    Serial.println( ")" );
  }
  int potOnAtu = potAtual;
  unsigned long dLimite1 = dtNEXT;
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dLimite1 = DtZero_uL;
  }
  unsigned long dLimite2 = dLimite1 + DtOff_uL;
  while ( (micros()-tLAST)<dLimite1 ){}
  mudaPot( 0 );
  while ( (micros()-tLAST)<dLimite2 ){}
  mudaPot( potOnAtu );
  tLAST += dLimite2;
  dtNEXT = 0;
}

//void fazOnOffCycles(float DtOff, float DtRep, long nCiclos){
void fazOnOffCycles(unsigned long DtOff_uL, 
                    unsigned long DtRep_uL, long nCiclos){
  if (DEBUG){
    Serial.println( F("fazOnOffCycles(DtOff, DtRep, nCiclos)") );
    Serial.print( F("fazOnOffCycles(") );
    Serial.print( DtOff_uL );
    Serial.print( F(", ") );
    Serial.print( DtRep_uL );
    Serial.print( F(", ") );
    Serial.print( nCiclos );
    Serial.println( ")" );
  }
  int potOnAtu = potAtual;
  unsigned long dLimite1 = dtNEXT;
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dLimite1 = DtZero_uL;
  }
  unsigned long dLimite2 = 0;
  for (unsigned long i=0; i<nCiclos; i++){
    if (i>0){
      dLimite1 = DtRep_uL;
    }
    dLimite2 = dLimite1 + DtOff_uL;
    while ( (micros()-tLAST)<dLimite1 ){}
    mudaPot( 0 );
    while ( (micros()-tLAST)<dLimite2 ){}
    mudaPot( potOnAtu );
    tLAST += dLimite2;
  }
  dtNEXT = 0;
}




void fazJumps(float DF, long DtRep_uL, long nJumps){
  if (DEBUG){
    Serial.println( F("fazJumps(DF, DtRep_uL, nJumps)") );
    Serial.print( F("fazJumps(") );
    Serial.print( DF );
    Serial.print( F(", ") );
    Serial.print( DtRep_uL );
    Serial.print( F(", ") );
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
  unsigned long dLimite = dtNEXT;
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dLimite = DtZero_uL;
  }
  for (long i=0; i<nJumps; i++ ){
    if (i>0){
      dLimite = DtRep_uL;
    }
    while ( (micros()-tLAST)<dLimite) {}
    mudaFase( newFase, false );
    newFase = F1+F2-newFase;
    tLAST += dLimite;
  }
  dtNEXT = 0;
}



void fazSteps(float DF, long Steps, long Dt_uL){
  if (DEBUG){
    Serial.println( F("fazSteps(DF, Steps, Dt_uL)") );
    Serial.print( F("fazSteps(") );
    Serial.print( DF );
    Serial.print( F(", ") );
    Serial.print( Steps );
    Serial.print( F(", ") );
    Serial.print( Dt_uL );
    Serial.println( ")" );
  }
  float Fini = faseAtual;
  float Ffin = Fini + DF;
  unsigned long dLimite = dtNEXT;
  if ((micros()-tLAST)>dtNEXT){
    tLAST = micros();
    dLimite = DtZero_uL;
  }

  for (long i=0; i<=Steps; i++){
    if (i>0){
      dLimite = Dt_uL;
    }
    float Fatu = Fini + float(i*(Ffin-Fini))/max(Steps,1);
    while( (micros()-tLAST)<dLimite ){}
    mudaFase( Fatu, false );
    tLAST += dLimite;
  }
  dtNEXT = 0;
}
