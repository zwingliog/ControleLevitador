// Precisa das variaveis globais:
//   pinEnA -> byte
//   pinEnB -> byte
//   potAtual -> bool
//
// Usa as funcoes:
//


void mudaFase( float newFase ){
  mudaFase( newFase, false );
}
void mudaFase( float newFase, bool mostra ){
  if (TIPOTimer==12){
    mudaFase12( newFase, mostra );
  }
  else if (TIPOTimer==1){
    mudaFase1( newFase, mostra );
  }
}

void mudaImax( int newImax, float oldFase ){
  if (TIPOTimer==12){
    mudaImax12(newImax, oldFase );
  }
  else if (TIPOTimer==1){
    mudaImax1(newImax, oldFase );
  }
}

void configuraTimer(){
  if (TIPOTimer==12){
    configuraTimer12();
  }
  else if (TIPOTimer==1){
    configuraTimer1();
  }
}


void mudaPot( int newPot ){
    if ( (pinEnA==5) && (pinEnB==6) ){
      if (newPot==1)
        PORTD = PORTD | B01100000;
      else if ((newPot==2)||(newPot==-1)){
        PORTD = PORTD | B00100000;
        PORTD = PORTD & B10111111;
      }else if ((newPot==3)||(newPot==-2)){
        PORTD = PORTD | B01000000;
        PORTD = PORTD & B11011111;
      }else if(newPot==0)
        PORTD = PORTD & B10011111;
    }else{
      digitalWrite( pinEnA, (newPot==1)||(newPot==2)||(newPot==-1) );
      if (pinEnB!=pinEnA)
        digitalWrite( pinEnB, (newPot==1)||(newPot==3)||(newPot==-2) );
    }
    potAtual = newPot;
}
