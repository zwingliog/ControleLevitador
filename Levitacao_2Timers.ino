// Levitacao_2Timers [controle da fase usando ICR1 ou Timers 1 e 2]
// Gera sinais com defasagem escolhida
// TIPOTimer=1  -> de 20 a 80kHz entre os pinos 9 e 10
// TIPOTimer=12 -> de 32.15 a 80kHz entre (9,10) e (11,3)
//                 [10 = not 9 ] e [3 = not 11]
//                 para nao precisar de inversor externo

// versao de 31/12/2022 (ajuste comandos para diferenciar maiusculas e minusculas)
// versao de 30/12/2022 (diversas mudancas para preparar versao 1.0)
// versao de 22/12/2022 (inclusao da funcao jaTerminou)
// versao de 03/12/2022 (Controles A e B nas portas A0 e A1)
// versao de 28/11/2022 (ativado a opcao dos controles A e B)
// versao de 30/10/2022 (primeira versao)
//   OBS: nome anterior: "Levitacao_ControleLevitador"

#define TIME_OUT  5
byte TIPOTimer = 1;
int Imax = 199; // calculo preve 199 para 40kHz e 319 para 25kHz
int fatorImax = 1; // fator multiplicativo do Imax para Timer2 (8-bits)
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

bool TemINPUT = false;
bool MostraTudo = true;
float DtZeroPadrao = 0.01;

byte pinEnA = 5;
byte pinEnB = 6;
byte pinControle[] = {A0, A1, A2, A3, A4, A5};
bool valorControle[] = {LOW, LOW, LOW, LOW, LOW, LOW};
bool modoControle[] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT};
const int nControles = sizeof(pinControle)/sizeof(pinControle[0]);

// ------------- SETUP ---------------- SETUP -------------------
void setup() {

  Serial.begin(250000); // nao usar 230400- usar 250000 ou 115200
  Serial.setTimeout( TIME_OUT );

  configuraTimer();
  pinMode( pinEnA, OUTPUT);
  pinMode( pinEnB, OUTPUT);
  mudaPot( potAtual );
  for (int qControle=0; qControle<nControles; qControle++){
    pinMode( pinControle[qControle], modoControle[qControle] );
    digitalWrite( pinControle[qControle], valorControle[qControle] );
  }  
  Serial.println( F("Levitacao 2 Timers") );
  Serial.println( F("31 dezembro 2022") );
  Serial.print( F("usando TIPOTimer = ") );
  Serial.println( TIPOTimer );
  Serial.println( F("digite h para help") );
  Serial.print( Imax );
  Serial.print( F(" (") );
  Serial.print( 8000.0/(Imax+1) );
  Serial.println( F("kHz)") );
}
// ---------- end of SETUP ---------------- end of SETUP ---------------



// ------------- LOOP ---------------- LOOP ------------------------
void loop() {
  if (Serial.available()>0){
    TemINPUT = true;
  }else{
    TemINPUT = false;
  }
  if ( TemINPUT ){
    byte Sp = removeEspaco();
    if ( (Sp==10) || (Sp==13) || (Sp==',') || (Sp==';') ){
      removeEspaco(Sp);

    }else if(Sp=='w'){ // w -> wait : w [Dt]
      unsigned long t0 = micros();
      removeEspaco(Sp);
      float newDelayT = 0.;
      if (!jaTerminou()){
        newDelayT = myParseFloat();
      }
      if (newDelayT==0.)
        newDelayT = DelayT;
      if (newDelayT>0.){
        unsigned long Limite = t0 + (1000uL*newDelayT);
        while( micros()<Limite ){}
        if (MostraTudo)
          Serial.println( char(Sp) );
      }

    }else if(Sp=='t'){ // t -> set DelayT : t newDelayT
      removeEspaco(Sp);
      if (!jaTerminou()){
        float newDelayT = myParseFloat();
        if (newDelayT>0.){
          DelayT = newDelayT;
          Serial.print( F("DelayT = ") );
          Serial.print( DelayT );
          Serial.println( F(" ms") );
        }
      }


      
    }else if(Sp=='?'){ // ?-> status
      removeEspaco(Sp);
      mostraStatus();
      


    }else if(Sp=='p'){
      removeEspaco(Sp);
      if (!jaTerminou()){
        int newP = myParseInt();
        mudaPot( newP );
      }


    }else if(Sp=='f'){
      removeEspaco(Sp);
      if (!jaTerminou()){
        float newF = 0.;
        if ( removeEspaco()=='*' ){
          removeEspaco('*');
          newF = faseAtual;
        }
        newF += myParseFloat();
        mudaFase( newF, true );
      }



    }else if( (Sp=='o') || (Sp=='c') ){ 
      // o-> off : o DtOff
      // c-> on-off cycles : c DtOff, Dt, nCiclos
      removeEspaco(Sp);
      Serial.println( char(Sp) );
      float DtOff = 0;
      if (!jaTerminou()){
        if (removeEspaco()=='*'){
          removeEspaco( '*' );
          DtOff = DelayT;
        }else if (removeEspaco()==','){
          removeEspaco( ',' );
          DtOff = DelayT;
        }else{
          DtOff = myParseFloat();
        }
      }
      if (DtOff==0)
        DtOff = DelayT;
      float Dt = 0;
      long nCiclos = 1;
      if ( (Sp=='c') && (!jaTerminou()) ){
        Dt = myParseFloat();
        if (removeEspaco(',')=='*'){
          nCiclos = 1;
          removeEspaco( '*' );
        }else{
          if (jaTerminou()){
            nCiclos = 1;
          }else{
            nCiclos = myParseInt();
            if (nCiclos<=0)
              nCiclos = 1;
          }
        }
      }
      fazOnOffCycles(DtOff, Dt, nCiclos, DtZeroPadrao);
      Serial.println(".");

      

    }else if(Sp=='j'){ // j-> jump : j DF, Dt, nJumps
      removeEspaco(Sp);
      if (!jaTerminou()){
        Serial.println( char(Sp) );
        float DF;
        float Dt = DelayT;
        long nJumps = 1;
        DF = myParseInt();
        if (!jaTerminou()){
          if (removeEspaco()=='*'){
            removeEspaco('*');
          }else{
            Dt = myParseFloat();
            if (Dt==0)
              Dt = DelayT;
          }
          if (!jaTerminou()){
            nJumps = myParseInt();
            if (nJumps<=0)
              nJumps = 1;
          }
        }
        //fazJumps(float DF, float Dt, long nJumps, float DtZero)
        fazJumps( DF, Dt, nJumps, DtZeroPadrao);
        Serial.println(".");
      }
      

    }else if( (Sp=='s') || (Sp=='S') || (Sp=='u') || (Sp=='d') ){ 
      // s -> steps: s Fini, Ffin
      // S -> steps: s Fini, Ffin, Steps, Dt
      // u -> up:    u
      // d -> down:  d
      removeEspaco(Sp);
      if ( (Sp=='u') || (Sp=='d') || (!jaTerminou()) ){
        Serial.println( char(Sp) );
        float Fini = 0.;
        float Ffin = 0.;
        if (Sp=='u'){ // u -> up
          Fini = faseAtual;
          Ffin = faseAtual+360.0;          
        }else if (Sp=='d'){ // d -> down
          Fini = faseAtual;
          Ffin = faseAtual-360.0;        
        }else if ( (Sp=='s') || (Sp=='S') ){ // s,S
           // s -> steps: s Fini, Ffin
           // S -> steps: s Fini, Ffin, Steps, Dt
          if (removeEspaco()=='*'){
            Fini = faseAtual;
            removeEspaco('*');
          } else {
            Fini = myParseFloat();
          }
          if (removeEspaco(',')=='*'){
            removeEspaco('*');
            Ffin = Fini;
          }
          Ffin += myParseFloat();
        }      
        long Steps;
        float Dt = DelayT;
        if ( (Sp!='S') || (jaTerminou()) ){
          Steps = (Imax+1)*(abs(Ffin-Fini)/360.0);
        }else{
          if (removeEspaco(',')=='#'){
              removeEspaco('#');
              Steps = 2*(Imax+1)*(abs(Ffin-Fini)/360.0);
          }else if (removeEspaco(',')=='$'){
              removeEspaco('$');
              Steps = (Imax+1)*(abs(Ffin-Fini)/360.0);
          }else{
            Steps = myParseInt();
          }
          if (removeEspaco(',')=='*'){
            removeEspaco('*');
          }else{
            if (!jaTerminou()){
              Dt = myParseFloat();
              if (Dt==0.)
                Dt = DelayT;
            }
          }
        }
        if ( (Steps>0) && (Dt>0.) ){
          //fazSteps(float Fini, float Ffin, long Steps, float Dt, float DtZero)
          fazSteps(Fini, Ffin, Steps, Dt, DtZeroPadrao);
          Serial.println(".");
        }
      }
  

    }else if (Sp=='i'){
      removeEspaco(Sp);
      bool mudouImax = false;
      int newImax = 0;
      if (jaTerminou()){
        removeEspaco(';');
      }else{
        newImax = myParseInt();
        if ( (newImax>=ImaxMINgeral) && (newImax<=ImaxMAXgeral) ){
          mudouImax = true;
        }
      }
      if ( mudouImax ){
        mudaImax( newImax, faseAtual );
        Serial.print( Imax );
        Serial.print( " (" );
        Serial.print( 8000.0/(Imax+1) );
        Serial.println( "kHz)" );
      }


    }else if(Sp=='h'){
      removeEspaco(Sp);
      int Nivel=0;
      if (jaTerminou())
        removeEspaco(';');
      else
        Nivel = myParseInt();
      mostraHelpComandos( Nivel!=0 );  


    }else if(Sp=='a'){
      removeEspaco(Sp);
      byte qualControle=100;
      bool newValue;
      bool newModo;
      bool mudouValue = false;
      bool mudouModo = false;
      if (jaTerminou()){
        qualControle = 0;
      }else{
        int newQual = myParseInt();
        if ( (newQual>=0) && (newQual<nControles) )
          qualControle = newQual;
      }
      if (jaTerminou()){
        removeEspaco(';');
        newValue = !valorControle[qualControle];
        mudouValue = true;
      }else{
        Sp = removeEspaco();
        if ( (Sp=='L')||(Sp=='0') ){
          newValue = LOW;
          mudouValue = true;
        }else if ( (Sp=='H')||(Sp=='1') ){
          newValue = HIGH;
          mudouValue = true;
        }else if (Sp=='I'){
          newModo = INPUT;
          mudouModo = true;
        }else if (Sp=='O'){
          newModo = OUTPUT;
          mudouModo = true;
        }else if (Sp=='*'){
          newValue = !valorControle[qualControle];
          mudouValue = true;
        }
        removeEspaco(Sp,';');
      }
      if ( (qualControle!=100) && ( (mudouValue) || (mudouModo) ) ){
        if (MostraTudo){
          Serial.print( F("A") );
          Serial.print( qualControle );
          Serial.print( F(":") );
        }
        if (mudouModo){
          modoControle[qualControle] = newModo;
          pinMode( pinControle[qualControle], modoControle[qualControle] );
          if (MostraTudo)
            Serial.println( (newModo==OUTPUT)? "O": "I" );
        }
        if (mudouValue){
          valorControle[qualControle] = newValue;
          digitalWrite( pinControle[qualControle], valorControle[qualControle] );
          if (MostraTudo)
            Serial.println( (newValue==LOW)? "L": "H" );
        }
      }


    }else{ // Se nao for nenhum comando conhecido
      Serial.println( char(Sp) );
      removeEspaco(Sp);
    }
  }

}
// ---------- end of LOOP ---------------- end of LOOP ---------------




int myRead(){
  return Serial.read();
}
int myPeek(){
  return Serial.peek();
}
long myParseInt(){
  return Serial.parseInt();
}
float myParseFloat(){
  return Serial.parseFloat();
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
    unsigned long tEnd = micros() + TIME_OUT*1000uL;
    do{
      nextChar = myPeek();
    }while( (nextChar==-1) && (micros()<tEnd) );
    removendo = false;
    if ( (nextChar==' ')||(nextChar==oqueMais1)||(nextChar==oqueMais2) ){
      removendo = true;
      myRead();
    }
  }
  if (nextChar==-1)
    nextChar = 0;
  return byte(nextChar);
}


bool jaTerminou(){
  byte endChar[] = { ';','w','t','p','f',   'o','c','j','?','h',   
                     'i','s','S','u','d',   'a', 10, 13 };
  int nEndChar = sizeof(endChar)/sizeof(endChar[0]);
  byte pChar = removeEspaco(',');
  bool achou = false;
  for ( int nAtu=0; (!achou)&&(nAtu<nEndChar); nAtu++ ){
    achou = (pChar==endChar[nAtu]);
  }
  return achou;
}
