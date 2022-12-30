// Comandos (funcoes associadas aos comandos escolhidos pelo usuario)
// Precisa das variaveis globais:
//   DelayT -> float


void onOffCycles(float DtOff, float Dt, int nCiclos, float DtZero){
  unsigned long t0 = micros();
  for (int i=0; i<nCiclos; i++){
    unsigned long Limite1 = t0 + (i*Dt + DtZero)*1000uL;
    unsigned long Limite2 = t0 + (i*Dt + DtOff + DtZero)*1000uL;
    while (micros()<Limite1){}
    mudaPot( 0, false, i==0 );
    while (micros()<Limite2){}
    mudaPot( 1, false, i==(nCiclos-1), false );
  }
//  executandoAcao( false );
}




void jumps(float DF, float Dt, int nJumps, bool Mostra, float DtZero){
  float F1 = faseAtual;
  float F2 = faseAtual+DF;
  float newFase = F2;
  Serial.println( F1 );
  Serial.println( F2 );
  unsigned long t0 = micros();
  for (int i=0; i<nJumps; i++ ){
    unsigned long Limite = t0 + (i*Dt + DtZero)*1000uL;
    while (micros()<Limite) {}
    mudaFase( newFase, Mostra, (i==0) );
    newFase = F1+F2-newFase;
  }
  executandoAcao( false );
}




void cycles(float Fini, float Ffin, int Steps, int nC, float Dt, bool Mostra, float DtZero){
  unsigned long t0 = micros();
  for (int qC=0; qC<nC; qC++){
    for (int i=0; i<=Steps; i++){
      unsigned long Limite = t0 + ((qC*(Steps+1) + i)*Dt + DtZero)*1000uL;
      float Fatu = Fini + float(i*(Ffin-Fini))/max(Steps,1);
      while( micros()<Limite ){}
      mudaFase( Fatu, Mostra, (i==0)&&(qC==0) );
    }
    float FfinA = Ffin;
    Ffin = Fini;
    Fini = FfinA;
  }
  executandoAcao( false );
}




void mudancasImax(int ImaxIni, int ImaxFin, int deltaImax, float DtWaitImax, bool Mostra, float DtZero){
  int nImax = (ImaxFin-ImaxIni)/deltaImax;
  unsigned long t0 = micros();
  for (int indiceImax=0; indiceImax<=nImax; indiceImax++){
    Imax = ImaxIni + indiceImax*deltaImax;
    unsigned long Limite = t0 + (indiceImax*DtWaitImax + DtZero)*1000uL;
    while( micros()<Limite ) {}
    mudaImax( Imax, faseAtual );
    if (indiceImax==0)
      executandoAcao( true );
    if ( Mostra ){
      Serial.println( Imax );
    }  
  }
  executandoAcao( false );
}




void mostraStatus(){
      Serial.print( F("iMax=") );
      Serial.print( Imax );
      Serial.print( F(" (") );
      Serial.print( 8000.0/(Imax+1) );
      Serial.println( F("kHz)") );
      Serial.print( F("fase=") );
      Serial.println( faseAtual );
      Serial.print( F("fase Real=") );
      Serial.println( faseAtualReal );
      Serial.print( F("DelayT=") );
      Serial.println( DelayT );
      Serial.print( F("Pot=") );
      Serial.println( potAtual );
      Serial.print( F("EnA:") );
      Serial.print( pinEnA );
      Serial.print( F(" EnB:") );
      Serial.print( pinEnB );
      //Serial.print( F(" Acao:") );
      //Serial.print( pinExecutandoAcao );
      Serial.print( F(" A:") );
      Serial.print( pinControleA );
      Serial.print( F(" B:") );
      Serial.print( pinControleB );
      Serial.print( F(" valor A=") );
      Serial.print( valorControleA );
      Serial.print( F(" B=") );
      Serial.println( valorControleB );
      
      //Serial.print( F("micros/1e6=") );
      //Serial.println( micros()/1e6 );
}



void mostraHelpComandos(){
  mostraHelpComandos( false );
}
void mostraHelpComandos(bool DETALHADO){
  Serial.println( F("Comandos disponiveis:") );
  Serial.println( F("?  -> status [ ? ]") );
  Serial.println( F("w,W > wait [ W DelayT ]") );
  Serial.println( F("p,P > set Pot [ P newP ]") );
  Serial.println( F("f,F > set Fase [ F newF ]") );
  if (DETALHADO){
    Serial.println( F("+/- > increase/decrease Imax") );
    Serial.println( F("i,I > define Imax [ i Imax ]") );
    Serial.println( F("k,K > scan in Imax [ k ImaxIni ImaxFin Dt ]") );
  }
  Serial.println( F("o,O > on-off cycles [ o DtOff, Dt, nCycles ]") );
  Serial.println( F("j,J > jump [ j DeltaF, Dt, nJumps ]") );
  Serial.println( F("u,U > up (360 degrees) [ u Steps, Dt ]") );
  Serial.println( F("d,D > down (360 degrees) [ d Steps, Dt ]") );
  Serial.println( F("c,C > cycles (up + down + up ...) [ C Steps, Dt, nCycles ]") );
  Serial.println( F("s,S > switch (in steps) [ s Fini, Ffin, Steps, Dt ]") );
  Serial.println( F("h  -> help [ h ]=help simples [h1]=help detalhado") );
  if (DETALHADO){
    Serial.println( F(" OBS1: em 'u', 'd', 'c' e 's', Steps pode ser '#' ou '$':") );
    Serial.println( F("  '#'->2*(iMax+1) a cada 360º e '$'->(iMax+1) a cada 360º") );
    Serial.println( F(" OBS2: em 's', Fini pode ser '*'=>fase atual") );
    Serial.println( F("       e '*'+DF em Ffin faz Ffin=fase atual+DF") );
  }
}
