// Levitacao_2Timers [controle da fase usando ICR1 ou Timers 1 e 2]
// Gera sinais com defasagem escolhida
// TIPOTimer=1  -> de 20 a 80kHz entre os pinos 9 e 10
// TIPOTimer=12 -> de 32.15 a 80kHz entre (9,10) e (11,3)
//                 [10 = not 9 ] e [3 = not 11]
//                 para nao precisar de inversor externo

// versao de 01/01/2023 (usando Strings)
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
float DelayTRep = 1000.0;

bool MostraTudo = true;
float DtZeroPadrao = 0.01;

byte pinEnA = 5;
byte pinEnB = 6;
byte pinControle[] = {A0, A1, A2, A3, A4, A5};
bool valorControle[] = {LOW, LOW, LOW, LOW, LOW, LOW};
bool modoControle[] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT};
const int nControles = sizeof(pinControle)/sizeof(pinControle[0]);


String inStr = "";
bool strOk = false;
char endChar = 13;
String endStr = String(endChar);

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
  Serial.println( F("1 janeiro 2023") );
  Serial.print( F("usando TIPOTimer = ") );
  Serial.println( TIPOTimer );
  Serial.println( F("digite h para help") );
  mostraImax();
  mostraFase();
  mostraPot();

  inStr.reserve(200);
}
// ---------- end of SETUP ---------------- end of SETUP ---------------



// ------------- LOOP ---------------- LOOP ------------------------
void loop() {
  while (Serial.available()>0){
    addChar();
  }
  if ( (inStr.length()>0) && (inStr.endsWith(endStr)) ){
    strOk = true;
  }else{
    strOk = false;
  }
  if ( strOk ){
    byte Sp = removeEspaco();
    if ( (Sp==10) || (Sp==13) || (Sp==',') || (Sp==';') ){
      removeEspaco(Sp);

    }else if( (Sp=='w') || (Sp=='W') ){ // w,W - wait
      // w -> wait : w [Dt]
      // W -> wait Rep : W [DtRep]
      unsigned long t0 = micros();
      myRead();
      float newDelay = 0.;
      if (!jaTerminou()){
        newDelay = myParseFloat();
      }
      if (newDelay==0.){
        if (Sp=='w'){
          newDelay = DelayT;
        }else if (Sp=='W'){
          newDelay = DelayTRep;
        }
      }
      if (newDelay>0.){
        unsigned long Limite = t0 + (1000uL*newDelay);
        while( micros()<Limite ){}
        if (MostraTudo)
          Serial.println( char(Sp) );
      }

    }else if( (Sp=='t') || (Sp=='T') ){ // t, T -> set DelayT
      // t -> set DelayT : t newDelayT
      // t -> set DelayTRep : t newDelayTRep
      myRead();
      if (!jaTerminou()){
        float newDelayT = myParseFloat();
        if (newDelayT>0.){
          if (Sp=='t'){
            DelayT = newDelayT;
          }else if (Sp=='T'){
            DelayTRep = newDelayT;
          }
          mostraDelayT();
        }
      }


      
    }else if(Sp=='?'){ // ?-> status
      myRead();
      mostraStatus();
      


    }else if(Sp=='p'){
      myRead();
      if (!jaTerminou()){
        int newP = myParseInt();
        if ( (newP>=0)&&(newP<=3) ){
          mudaPot( newP );
          Serial.print( "p" );
          Serial.println( newP );
        }
      }else{
        mostraPot();
      }


    }else if(Sp=='f'){
      myRead();
      if (!jaTerminou()){
        float newF = 0.;
        if ( removeEspaco()=='*' ){
          removeEspaco('*');
          newF = faseAtual;
        }
        newF += myParseFloat();
        mudaFase( newF, true );
      }else{
        mostraFase();
      }
  

    }else if (Sp=='i'){
      myRead();
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
      }
      mostraImax();



    }else if( (Sp=='o') || (Sp=='c') ){ 
      // o-> off : o DtOff
      // c-> on-off cycles : c DtOff, DtRep, nCiclos
      myRead();
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
      float DtRep = DelayTRep;
      long nCiclos = 1;
      if ( (Sp=='c') && (!jaTerminou()) ){
        if (removeEspaco(',')=='*'){
          removeEspaco( '*' );
        }else{
          DtRep = myParseFloat();
        }
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
      if (DtRep==0.){
        nCiclos = 1;
      }
      Serial.println( char(Sp) );
      fazOnOffCycles(DtOff, DtRep, nCiclos, DtZeroPadrao);
      Serial.println(".");

      

    }else if(Sp=='j'){ // j-> jump : j DF, DtRep, nJumps
      myRead();
      if (!jaTerminou()){
        float DF;
        float DtRep = DelayTRep;
        long nJumps = 1;
        DF = myParseInt();
        if (!jaTerminou()){
          if (removeEspaco()=='*'){
            removeEspaco('*');
          }else{
            DtRep = myParseFloat();
          }
          if (!jaTerminou()){
            nJumps = myParseInt();
            if (nJumps<=0)
              nJumps = 1;
          }
        }
        if (DF!=0.){
          if (DtRep==0.){
            nJumps = 1;
          }
          Serial.println( char(Sp) );
          //fazJumps(float DF, float DtRep, long nJumps, float DtZero)
          fazJumps( DF, DtRep, nJumps, DtZeroPadrao);
          Serial.println(".");
        }
      }
      

    }else if( (Sp=='s') || (Sp=='S') || (Sp=='u') || (Sp=='d') ){ 
      // s -> steps: s Fini, Ffin, Dt
      // S -> steps: s Fini, Ffin, Steps, Dt
      // u -> up:    u Dt
      // d -> down:  d Dt
      myRead();
      float Fini = 0.;
      float Ffin = 0.;
      if (Sp=='u'){ // u -> up : u Dt
        Fini = faseAtual;
        Ffin = faseAtual+360.0;          
      }else if (Sp=='d'){ // d -> down : d Dt
        Fini = faseAtual;
        Ffin = faseAtual-360.0;        
      }else if ( (Sp=='s') || (Sp=='S') ){ // s,S
          // s -> steps: s Fini, Ffin, Dt
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
      long Steps= (Imax+1)*(abs(Ffin-Fini)/360.0);
      if ( (Sp=='S') && (!jaTerminou()) ){
        if (removeEspaco(',')=='#'){
            removeEspaco('#');
            Steps = 2*(Imax+1)*(abs(Ffin-Fini)/360.0);
        }else if (removeEspaco(',')=='$'){
            removeEspaco('$');
            Steps = (Imax+1)*(abs(Ffin-Fini)/360.0);
        }else{
          Steps = myParseInt();
        }
      }
      float Dt = DelayT;
      if (!jaTerminou()){
        if (removeEspaco(',')=='*'){
          removeEspaco('*');
        }else{
          Dt = myParseFloat();
          Serial.println( Dt );
          if (Dt==0.)
            Dt = DelayT;
        }
      }
      if ( (Steps>0) && (Dt>0.) ){
        Serial.println( char(Sp) );
        //fazSteps(float Fini, float Ffin, long Steps, float Dt, float DtZero)
        fazSteps(Fini, Ffin, Steps, Dt, DtZeroPadrao);
        Serial.println(".");
      }


    }else if(Sp=='h'){
      myRead();
      int Nivel=0;
      if (jaTerminou())
        removeEspaco(';');
      else
        Nivel = myParseInt();
      mostraHelpComandos( Nivel!=0 );  


    }else if(Sp=='a'){
      myRead();
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
      Serial.print( F("skipped: ") );
      Serial.println( char(Sp) );
      myRead();
    }
  }

}
// ---------- end of LOOP ---------------- end of LOOP ---------------



void addChar(){
  char inChar = char( Serial.read() );
  if ( (inChar==10) || (inChar==13) ){
    inChar = endChar;
  }
  if (inChar>0){
    if (inChar==endChar){
      if ( (inStr.length()>0) && (!inStr.endsWith(endStr)) ){
        inStr += inChar;
      }
    }else{
      inStr += inChar;
    }
  }
}

int myRead(){
  int answer = -1;
  if (inStr.length()>0){
    answer = inStr.charAt(0);
    inStr.remove(0,1);
  }
  return answer;
}

int myPeek(){
  int answer = -1;
  if (inStr.length()>0){
    answer = inStr.charAt(0);
  }
  return answer;
}

long myParseInt(){
  return (digitosStr()).toInt();
}

float myParseFloat(){
  return (digitosStr()).toFloat();
}

String digitosStr(){
  bool lastOk = true;
  int nDigOk = -1;
  int nDig;
  bool passouSeparador = false;
  for (nDig=0; 
      (nDig<inStr.length())&&(lastOk); nDig++){
    char char_At = inStr.charAt(nDig);
    lastOk = false;
    if ( (char_At>='0')&&(char_At<='9') ){
      lastOk = true;
      if (nDigOk<0)
        nDigOk = 0;
      nDigOk++;
    }else if ((!passouSeparador)&&(char_At=='.')){
      passouSeparador = true;
      lastOk = true;
      if (nDigOk<0)
        nDigOk = 0;
      nDigOk++;
    }else if (nDigOk<0){
      if (char_At==' '){
        lastOk = true;
      }else if( (char_At=='+')||(char_At=='-') ){
        lastOk = true;
        nDigOk = 0;        
      }
    }
  }
  if (nDigOk<=0){
    return String("");
  }else{
    nDig = nDig-1;
    String answer = (inStr.substring(0,nDig));
    inStr.remove(0,nDig);
    return answer;
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
  byte terminouChar[] = { ';','w','t','p','f',   'o','c','j','?','h',   
                     'i','s','S','u','d',   'a', 10, 13 };
  int nTerminouChar = sizeof(terminouChar)/sizeof(terminouChar[0]);
  byte pChar = removeEspaco(',');
  bool achou = false;
  for ( int nAtu=0; (!achou)&&(nAtu<nTerminouChar); nAtu++ ){
    achou = (pChar==terminouChar[nAtu]);
  }
  return achou;
}
