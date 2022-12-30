// Levitacao_2Timers [controle da fase usando ICR1 ou Timers 1 e 2]
// Gera sinais com defasagem escolhida
// TIPOTimer=1  -> de 20 a 80kHz entre os pinos 9 e 10
// TIPOTimer=12 -> de 32.15 a 80kHz entre (9,10) e (11,3)
//                 [10 = not 9 ] e [3 = not 11]
//                 para nao precisar de inversor externo

// versao de 22/12/2022 (inclusao da funcao jaTerminou)
// versao de 03/12/2022 (Controles A e B nas portas A0 e A1)
// versao de 28/11/2022 (ativado a opcao dos controles A e B)
// versao de 30/10/2022 (primeira versao)
//   OBS: nome anterior: "Levitacao_ControleLevitador"

#define TIME_OUT  5
byte TIPOTimer = 1;
int Imax = 199; // calculo preve 199 para 40kHz e 319 para 25kHz
int ImaxMIN = 99;
int ImaxMAX = 399-144*(TIPOTimer==12);
int ImaxMINgeral = 99;
int ImaxMAXgeral = 399-144*(TIPOTimer==12);
int DeltaOCR = 1;
float faseAtual = 0.0;
float faseAtualReal = 0.0;
int potAtual = 1;
bool oldFF = false;
float DelayT = 10.0;

byte pinEnA = 5;
byte pinEnB = 6;
byte pinExecutandoAcao = 4;
byte pinControleA = A0;
byte pinControleB = A1;
bool valorControleA = LOW;
bool valorControleB = LOW;

// ------------- SETUP ---------------- SETUP -------------------
void setup() {

  Serial.begin(250000); // nao usar 230400- usar 250000 ou 115200
  Serial.setTimeout( TIME_OUT );

  configuraTimer();
  pinMode( pinEnA, OUTPUT);
  pinMode( pinEnB, OUTPUT);
  mudaPot( potAtual, false );
  pinMode( pinExecutandoAcao, OUTPUT );
  digitalWrite( pinExecutandoAcao, LOW );
  pinMode( pinControleA, OUTPUT );
  digitalWrite( pinControleA, valorControleA );
  pinMode( pinControleB, OUTPUT );
  digitalWrite( pinControleB, valorControleB );
  
  Serial.println( F("Levitacao 2 Timers") );
  Serial.println( F("22 dezembro 2022") );
  Serial.print( F("usando TIPOTimer = ") );
  Serial.println( TIPOTimer );
  Serial.println( F("digite h para help") );
  Serial.print( Imax );
  Serial.print( F(" (") );
  Serial.print( 8000.0/(Imax+1) );
  Serial.println( F("kHz)") );

}


// ------------- LOOP ---------------- LOOP ------------------------
void loop() {
  
   if ( Serial.available()>0 ){
//    byte Sp = Serial.peek();
    byte Sp = removeEspaco();
    if ( (Sp==10)||(Sp==13)||(Sp==',')||(Sp==';') ){
      Serial.read();


      
    }else if((Sp=='w')||(Sp=='W')){ // w -> wait 
      Serial.read();
      float newDelayT = 0;
      if (removeEspaco()=='*'){
        removeEspaco('*',',');
      }else{
        if (jaTerminou()){
          removeEspaco(';');
        }else{
          newDelayT = Serial.parseFloat();
        }
        if ( (Sp=='W') && (newDelayT>0) ){
          DelayT = newDelayT;
          Serial.print( F("new DelayT = ") );
          Serial.println( DelayT );
        }
      }
      if (newDelayT==0)
        newDelayT = DelayT;
      if (newDelayT>0){
        unsigned long t0 = micros();
        unsigned long Limite = t0 + (1000*newDelayT);
        while( micros()<Limite ){}
        Serial.println( (char)Sp);
      }


      
    }else if(Sp=='?'){ // ?-> status
      Serial.read();
      mostraStatus();
      


    }else if((Sp=='P')||(Sp=='p')){
      Serial.read();
      if (!jaTerminou()){
        int newP = Serial.parseInt();
        mudaPot( newP, (Sp=='p'), true );
        executandoAcao( false );
      }else{
        removeEspaco(';');
      }


    }else if((Sp=='F')||(Sp=='f')){
      Serial.read();
      if (!jaTerminou()){
        float newF;
        if ( removeEspaco()=='*' ){
          removeEspaco('*');
          newF = Serial.parseFloat() + faseAtual;
        }else{
          newF = Serial.parseFloat();        
        }
        mudaFase( newF, (Sp=='f'), true );
        executandoAcao( false );
      }else{
        removeEspaco(';');
      }



    }else if((Sp=='o')||(Sp=='O')){ // o-> on-off cycles : o DtOff, Dt, nCiclos      
      Serial.read();
      Serial.println( (char)Sp );
      float DtOff;
      if (jaTerminou()){
        DtOff = 0;
      }else{
        if (removeEspaco()=='*'){
          removeEspaco( '*' );
          DtOff=DelayT;
        }else{
          DtOff = Serial.parseFloat();
        }
      }
      if (DtOff==0)
        DtOff = DelayT;
      float Dt;
      int nCiclos;
      if (jaTerminou()){
        removeEspaco(';');
        Dt = 0;
        nCiclos = 1;
      }else{
        Dt = Serial.parseFloat();
        if (removeEspaco(',')=='*'){
          nCiclos = 1;
          removeEspaco( '*' );
        }else{
          if (jaTerminou()){
            removeEspaco(';');
            nCiclos=1;
          }else{
            nCiclos = Serial.parseInt();
            if (nCiclos==0)
              nCiclos=1;
          }
        }
      }
      onOffCycles(DtOff, Dt, nCiclos, 0.05);
      Serial.println(".");

      

    }else if((Sp=='j')||(Sp=='J')){ // j-> jump : j DF, Dt, nJumps      
      Serial.read();
      if (!jaTerminou()){
        Serial.println( (char)Sp );
        float DF;
        float Dt;
        int nJumps;
        DF = Serial.parseInt();
        if (jaTerminou()){
          removeEspaco(';');
          Dt = 0;
          nJumps = 1;
        }else{
          if (removeEspaco()=='*'){
            Dt = DelayT;
            removeEspaco('*');
          }else{
            Dt = Serial.parseFloat();
            if (Dt==0)
              Dt = DelayT;
          }
          if (jaTerminou()){
            removeEspaco(';');
            nJumps = 1;
          }else{
            nJumps = Serial.parseInt();
            if (nJumps==0)
              nJumps = 1;
          }
        }
        bool Mostra = (nJumps<=50)&&(Sp=='j');
        //jumps(float DF, float Dt, int nJumps, bool Mostra, float DtZero)
        jumps( DF, Dt, nJumps, Mostra, 0.05);
        Serial.println(".");
      }else{
        removeEspaco(';');
      }
      

    }else if((Sp=='s')||(Sp=='S')||(Sp=='u')||(Sp=='U')||
                    (Sp=='d')||(Sp=='D')||(Sp=='c')||(Sp=='C')){ 
      Serial.read();
      if (!jaTerminou()){
        Serial.println( (char)Sp );
        float Fini, Ffin;
        int nC = 1;
        if ((Sp=='s')||(Sp=='S')){ // s -> steps: s,Fini,Ffin,Steps,Dt
          if (removeEspaco()=='*'){
            Fini = faseAtual;
            removeEspaco('*');
          } else {
            Fini = Serial.parseFloat();
          }
          if (removeEspaco(',')=='*'){
            removeEspaco('*');
            Ffin = Serial.parseFloat() + Fini;
          }else{
            Ffin = Serial.parseFloat();          
          }
        }else if ((Sp=='u')||(Sp=='U')){ // u -> up: u,Steps,Dt
          Fini = faseAtual;
          Ffin = faseAtual+360.0;
        }else if ((Sp=='d')||(Sp=='D')){ // d -> down : d,Steps,Dt
          Fini = faseAtual;
          Ffin = faseAtual-360.0;        
        }else if((Sp=='c')||(Sp=='C')){ // c -> cycles : c,Steps,Dt,nC
            Fini = faseAtual;
          if (removeEspaco()=='-'){
            Ffin = faseAtual - 360.0;
            removeEspaco('-');
          }else{
            Ffin = faseAtual + 360.0;
          }
          nC = 0;
        }
      
        int Steps;
        float Dt;
        if (jaTerminou()){
          removeEspaco(';');
          Steps = (Imax+1)*(abs(Ffin-Fini)/360.0);
          Dt = DelayT;
          nC = 2-nC;
        }else{
          if (removeEspaco(',')=='#'){
              removeEspaco('#');
              Steps = 2*(Imax+1)*(abs(Ffin-Fini)/360.0);
          }else if (removeEspaco(',')=='$'){
              removeEspaco('$');
              Steps = (Imax+1)*(abs(Ffin-Fini)/360.0);
          }else{
            Steps = Serial.parseInt();
          }
          if (removeEspaco(',')=='*'){
            Dt = DelayT;
            removeEspaco('*');
          }else{
            if (jaTerminou()){
              removeEspaco(';');
              Dt = DelayT;
              nC = 2-nC;
            }
            Dt = Serial.parseFloat();
            if (Dt==0)
              Dt = DelayT;        
          }
          if (nC==0){
            if (jaTerminou())
              removeEspaco(';');
            else
              nC = Serial.parseInt();
            if (nC==0)
              nC = 2;
          }
        }
        //cycles(float Fini, float Ffin, int Steps, int nC, float Dt, bool Mostra, float DtZero)
        bool Mostra = ( (Steps*nC<=20)&&((Sp=='s')||(Sp=='u')||(Sp=='d')||(Sp=='c')) );
        cycles(Fini, Ffin, Steps, nC, Dt, Mostra, 0.05);
        Serial.println(".");
      }else{
        removeEspaco(';');
      }
  

    }else if ((Sp=='+')||(Sp=='=')||(Sp=='-')||(Sp=='i')||(Sp=='I')||(Sp=='k')||(Sp=='K')){
      Serial.read();
      bool mudouImax = false;
      int ImaxIni = 0;
      int ImaxFin = 0;
      float DtWaitImax = 0;
      if ( (Sp=='+')||(Sp=='=') ){
        mudouImax = true;
        ImaxIni = (Imax<ImaxMAX)? Imax+1 : ImaxMIN;
      }else if(Sp=='-'){
        mudouImax = true;
        ImaxIni = (Imax>ImaxMIN)? Imax-1 : ImaxMAX;
      }else if((Sp=='i')||(Sp=='I')){
        if (jaTerminou()){
          removeEspaco(';');
          mudouImax = false;
        }else{
          int newImax = Serial.parseInt();
          if ((newImax>=ImaxMINgeral)&&(newImax<=ImaxMAXgeral)){
            ImaxIni = newImax;
            mudouImax = true;
          }
        }
      }else if((Sp=='k')||(Sp=='K')){
        //"scan in Imax [ k ImaxIni ImaxFin Dt ]"
        if (jaTerminou()){
          removeEspaco(';');
          mudouImax = false;
        }else{
          if (removeEspaco()=='*'){
            ImaxIni = Imax;
            removeEspaco('*');
          }else{
            ImaxIni = Serial.parseInt();
          }
          if (jaTerminou()){
            removeEspaco(';');
            mudouImax = false;
          }else{
            ImaxFin = Serial.parseInt();
            if (jaTerminou()){
              removeEspaco(';');
              DtWaitImax = DelayT;
            }else{
              if (removeEspaco(',')=='*'){
                DtWaitImax = DelayT;
                removeEspaco('*');
              }else{
                DtWaitImax = Serial.parseFloat();
                if (DtWaitImax==0)
                  DtWaitImax = DelayT;
              }
            }
            if ( (ImaxIni>=ImaxMINgeral) && (ImaxFin>=ImaxMINgeral) &&
                 (ImaxIni<=ImaxMAXgeral) && (ImaxFin<=ImaxMAXgeral) ){ 
              mudouImax = true;
              Serial.println(char(Sp));
            }
          }
        }
      }
      if ( (mudouImax) && (ImaxFin==0) ){
        ImaxFin = ImaxIni;
      }
      int deltaImax = (ImaxFin>=ImaxIni)? (+1): (-1);
      if ( mudouImax ){
        bool Mostra = ((Sp!='I') && (Sp!='K'));
        //mudancasImax(int ImaxIni, int ImaxFin, int deltaImax, float DtWaitImax, bool Mostra, float DtZero)
        mudancasImax(ImaxIni, ImaxFin, deltaImax, DtWaitImax, Mostra, 0.05);
        if ( (Sp!='I') && (Sp!='K') ){
          Serial.print( Imax );
          Serial.print( " (" );
          Serial.print( 8000.0/(Imax+1) );
          Serial.println( "kHz)" );
        }
      }


    }else if(Sp=='h'){
      Serial.read();
      int Nivel=0;
      if (jaTerminou())
        removeEspaco(';');
      else
        Nivel = Serial.parseInt();
      mostraHelpComandos( Nivel!=0 );  


    }else if( (Sp=='A')||(Sp=='a')||(Sp=='B')||(Sp=='b') ){
      Serial.read();
      int newValue;
      if (jaTerminou()){
        removeEspaco(';');
        if ( (Sp=='A')||(Sp=='a') )
          newValue = !valorControleA;
        else if ( (Sp=='B')||(Sp=='b') )
          newValue = !valorControleB;    
      }else
        newValue = Serial.parseInt();
      byte pinControleATU;
      bool valorControleATU = (newValue==1);
      if ((Sp=='A')||(Sp=='a')){
        pinControleATU = pinControleA;
        valorControleA = valorControleATU;
      }else if ((Sp=='B')||(Sp=='b')){
        pinControleATU = pinControleB;
        valorControleB = valorControleATU;
      }
      if ((Sp=='a')||(Sp=='b')){
        Serial.print( (char)Sp );
        Serial.println( valorControleATU );
      }
      mudaControle( pinControleATU, valorControleATU );
    }else{
      Serial.read();
    }
   }
    
}




byte removeEspaco(){
  return removeEspaco(' ',' ');
}
byte removeEspaco(byte oqueMais1){
  return removeEspaco(oqueMais1,' ');  
}
byte removeEspaco(byte oqueMais1, byte oqueMais2){
  int nextChar;
  bool removendo = true;
  while(removendo){
    unsigned long t0 = micros();
    do{
      nextChar = Serial.peek();
    }while( (nextChar==-1)&&( (micros()-t0)<(TIME_OUT*1000uL) ) );
//    Serial.println( nextChar );
    removendo = false;
    if ( (nextChar==' ')||(nextChar==oqueMais1)||(nextChar==oqueMais2) ){
      removendo = true;
      Serial.read();
    }
  }
  if (nextChar==-1)
    nextChar = 0;
  return byte(nextChar);
}

bool jaTerminou(){
  byte pChar = removeEspaco(',');
  return ( (pChar==';')||(pChar=='=')||(pChar=='+')||(pChar=='-')||
       (pChar=='w')||(pChar=='W')||(pChar=='p')||(pChar=='P')||
       (pChar=='f')||(pChar=='F')||(pChar=='o')||(pChar=='O')||
       (pChar=='j')||(pChar=='J')||(pChar=='s')||(pChar=='S')||
       (pChar=='u')||(pChar=='u')||(pChar=='d')||(pChar=='d')||
       (pChar=='c')||(pChar=='C')||(pChar=='h')||(pChar=='H')||
       (pChar=='i')||(pChar=='I')||(pChar=='k')||(pChar=='K')||
       (pChar=='a')||(pChar=='A')||(pChar=='b')||(pChar=='B')||
       (pChar==10)||(pChar==13) );
}



void executandoAcao(bool Ativar){
  if (pinExecutandoAcao==4){
    if (Ativar)
      PORTD = PORTD | B00010000;
    else
      PORTD = PORTD & B11101111;
  }else{
    digitalWrite(pinExecutandoAcao, Ativar);
  }
}



void mudaControle( byte pinControleATU, bool Ativar ){
  if (pinControleATU==A0){
    if (Ativar)
      PORTC = PORTC | B00000001;
    else
      PORTC = PORTC & B11111110;
  }else if (pinControleATU==A1){
    if (Ativar)
      PORTC = PORTC | B00000010;
    else
      PORTC = PORTC & B11111101;
  }else{
    digitalWrite(pinControleATU, Ativar);
  }
}
