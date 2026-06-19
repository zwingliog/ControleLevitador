// Shows - (rotinas para mostrar valores)

void mostraImax(){
  Serial.print( F("iMax=") );
  Serial.print( Imax );
  Serial.print( F(" (") );
  Serial.print( 8000.0/(Imax+1) );
  Serial.println( F("kHz)") );
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


void mostraBotoes(){
  Serial.print( F("number of botoes: ") );
  Serial.println( nBotoes );
  for (int i=1; i<=nBotoes; i++){  
    mostraBotoes(i);
  }  
}
void mostraBotoes( int QUAL){
  if (ehBotaoValido(QUAL)){
    if (botoes[QUAL-1].length()>0){
      Serial.print( "B" );
      Serial.print( QUAL );
      Serial.print( ":" );
      Serial.println( botoes[QUAL-1] );
    }
  }
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
      Serial.print( "M" );
      Serial.print( QUAL );
      Serial.print( ":" );
      Serial.println( memos[QUAL-1] );
    }
  }
}


void mostraControles(){
  for (int qC=0; qC<nControles; qC++){
    if (qC>0)
      Serial.print( " " );
    mostraControles(qC,false);
  }
  Serial.println();
}
void mostraControles(int QUAL){
  mostraControles(QUAL, true);
}
void mostraControles(int QUAL, bool TERMINA){
  if (ehControleValido(QUAL)){
    Serial.print( "A" );
    Serial.print( QUAL );
    Serial.print( ":" );
    Serial.print( (valorControle[QUAL]==LOW)? "L": "H" );
    Serial.print( "," );
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
