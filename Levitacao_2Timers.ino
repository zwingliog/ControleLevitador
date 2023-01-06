// Levitacao_2Timers [controle da fase usando ICR1 ou Timers 1 e 2]
// Gera sinais com defasagem escolhida
// TIPOTimer=1  -> de 20 a 80kHz entre os pinos 9 e 10
// TIPOTimer=12 -> de 32.15 a 80kHz entre (9,10) e (11,3)
//                 [10 = not 9 ] e [3 = not 11]
//                 para nao precisar de inversor externo

// v1.0.3 de 05/01/2023 (adicao de comentarios+mudanca no 's')
// v1.0.2 de 05/01/2023 (pequenas correcoes)
// v1.0.1 de 03/01/2023 (controle dos pinos usa 'A' ao inves de 'a')
// v1.0.0 de 03/01/2023 (adicionado funcao '_' para variaveis globais)
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
float DelayT = 10.0; // 10ms
float DelayTRep = 1000.0; // 1s (1000ms)

bool ESCREVE = true;
bool DEBUG = false;
bool COMENTA = true;
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

String memos[] = {"'botao',A0H;w10;A0L;", 
                  "'ciclo',u;d;", 
                  "d;u;", 
                  "R(2){[3]}",
                  "'queda',w300;p0;w700;", 
                  "M8 M1 M5 M1 M9 p1", 
                  "'ativa DEBUG',_DEBUG_true", 
                  "_ESCREVE_false", 
                  "_ESCREVE_true" };
const int  nMemos = sizeof(memos)/sizeof(memos[0]);

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
  Serial.println( F("v1.0.3") );
  Serial.print( F("usando TIPOTimer = ") );
  Serial.println( TIPOTimer );
  Serial.println( F("digite h para help") );
  Serial.println( F("digite ? para status") );
  mostraImax();
  //mostraFase();
  //mostraPot();
  //mostraMemos();

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
    
    verificaColchetes();
    
    byte Sp = removeEspaco();
    if ( (Sp==10) || (Sp==13) || (Sp==',') || (Sp==';') ){
      removeEspaco(Sp);

    }else if( (Sp=='w') || (Sp=='W') ){ // w,W - wait
      // w -> wait short (ms) : w [Dt]
      // W -> Wait long (s) : W [DtRep]
      unsigned long t0 = micros();
      myRead();
      float newDelay = 0.;
      if (!jaTerminou()){
        newDelay = myParseFloat();
        if (Sp=='W'){
          newDelay = 1000.0*newDelay;
        }
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
        if (ESCREVE)
          Serial.println( char(Sp) );
        if (DEBUG){
          Serial.print( F("Wait for ") );
          Serial.print( newDelay );
          Serial.println( F(" ms" ) );
        }
      }

    }else if( (Sp=='t') || (Sp=='T') ){ // t, T -> set DelayT
      // t -> set DelayT : t newDelayT
      // t -> set DelayTRep : t newDelayTRep
      myRead();
      if ((!jaTerminou()) && (contaDigitosStr()>0)){
        float newDelayT = myParseFloat();
        if (newDelayT>0.){
          if (Sp=='t'){
            DelayT = newDelayT;
          }else if (Sp=='T'){
            DelayTRep = 1000.0*newDelayT;
          }
          if (ESCREVE)
            mostraDelayT();
        }
      }


      
    }else if(Sp=='?'){ // ?-> status
      myRead();
      mostraStatus();
      


    }else if(Sp=='p'){
      myRead();
      if (myPeek()=='?'){
        mostraPot();
        myRead();
      }else if ((!jaTerminou())&&(contaDigitosStr()>0) ){
        int newP = myParseInt();
        if ( (newP>=0)&&(newP<=3) ){
          mudaPot( newP );
          if (ESCREVE){
            Serial.print( "p" );
            Serial.println( newP );
          }
        }
      }


    }else if(Sp=='f'){
      myRead();
      if (myPeek()=='?'){
        mostraFase();
        myRead();
      }else if (!jaTerminou()){
        float newF = 0.;
        if ( removeEspaco()=='*' ){
          removeEspaco('*');
          newF = faseAtual;
        }
        if (contaDigitosStr()>0){
          newF += myParseFloat();
          mudaFase( newF, true );
        }
      }
  

    }else if (Sp=='i'){
      myRead();
      if (myPeek()=='?'){
        mostraPot();
        myRead();
      }else if (!jaTerminou()){
        bool mudouImax = false;
        int newImax = myParseInt();
        if ( (newImax>=ImaxMINgeral) && (newImax<=ImaxMAXgeral) ){
          mudouImax = true;
        }
        if ( mudouImax ){
          mudaImax( newImax, faseAtual );
        }
        if (ESCREVE)
          mostraImax();
      }



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
      if (ESCREVE)
        Serial.println( char(Sp) );
      fazOnOffCycles(DtOff, DtRep, nCiclos, DtZeroPadrao);
      if (ESCREVE)
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
          if (ESCREVE)
            Serial.println( char(Sp) );
          //fazJumps(float DF, float DtRep, long nJumps, float DtZero)
          fazJumps( DF, DtRep, nJumps, DtZeroPadrao);
          if (ESCREVE)
            Serial.println(".");
        }
      }
      

    }else if( (Sp=='s') || (Sp=='S') || (Sp=='u') || (Sp=='d') ){ 
      // s -> steps: s DF, Dt
      // S -> steps: S DF, Steps, Dt
      // u -> up:    u Dt
      // d -> down:  d Dt
      myRead();
      float DF = 0.;
      if (Sp=='u'){ // u -> up : u Dt
        DF = +360.0;          
      }else if (Sp=='d'){ // d -> down : d Dt
        DF = -360.0;        
      }else if ( (Sp=='s') || (Sp=='S') ){ // s,S
          // s -> steps: s DF, Dt
          // S -> steps: S DF, Steps, Dt
        DF = myParseFloat();
      }
      long Steps= (Imax+1)*(abs(DF)/360.0);
      if ( (Sp=='S') && (!jaTerminou()) ){
        if (removeEspaco(',')=='!'){
            removeEspaco('!');
            Steps = 2*(Imax+1)*(abs(DF)/360.0);
        }else if (removeEspaco(',')=='$'){
            removeEspaco('$');
            Steps = (Imax+1)*(abs(DF)/360.0);
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
          if (Dt==0.)
            Dt = DelayT;
        }
      }
      if ( (Steps>0) && (Dt>0.) ){
        if (ESCREVE)
          Serial.println( char(Sp) );
        //fazSteps(float DF, long Steps, float Dt, float DtZero)
        fazSteps(DF, Steps, Dt, DtZeroPadrao);
        if (ESCREVE)
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


    }else if(Sp=='A'){
      myRead();
      if (myPeek()=='?'){
        mostraControles();
        myRead();
      }else if (!jaTerminou()){
        byte qualControle=100;
        bool newValue;
        bool newModo;
        bool mudouValue = false;
        bool mudouModo = false;
        int newQual = myParseInt();
        if (ehControleValido(newQual<nControles))
          qualControle = newQual;
        Sp = removeEspaco();
        if (Sp=='L'){
          newValue = LOW;
          mudouValue = true;
        }else if (Sp=='H'){
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
        if ( ehControleValido(qualControle) && ( (mudouValue) || (mudouModo) ) ){
          if (mudouModo){
            modoControle[qualControle] = newModo;
            pinMode( pinControle[qualControle], modoControle[qualControle] );
          }
          if (mudouValue){
            valorControle[qualControle] = newValue;
            digitalWrite( pinControle[qualControle], valorControle[qualControle] );
          }
          if (ESCREVE)
            mostraControles( qualControle );
        }
      }




    }else if (Sp=='M'){
      myRead();
      if (myPeek()=='?'){
        mostraMemos();
        myRead();
      }else{
        int numMemo = myParseInt();
        if ((numMemo>=1) && (numMemo<=nMemos)){
          inStr = memos[numMemo-1] + inStr;
        }
      }  



    }else if (Sp=='>'){
      myRead();
      int numMemo = myParseInt();
      if ((numMemo>=1) && (numMemo<=nMemos) && (inStr.charAt(0)==':') ){
        myRead();
        int nextEnd = inStr.indexOf(endChar);
        if (nextEnd>=0){
          memos[numMemo-1] = inStr.substring(0,nextEnd);
          inStr.remove(0,nextEnd+1);
          if (ESCREVE)
            mostraMemos(numMemo);
        }
      }



    }else if (Sp=='R'){
      myRead();
      bool repeticaoValida = true;
      if (myRead()!='('){
        repeticaoValida = false;
      }
      if ((repeticaoValida)&&(inStr.length()>=1+2+1)){
        long nLoop = myParseInt();
        if ( inStr.startsWith(String("){")) ){
          inStr.remove(0,2);
          int endAt = inStr.indexOf('}');
          if (endAt==-1)
            endAt = inStr.length()-1;
          String strARepetir = inStr.substring(0,endAt);
          inStr.remove(0,endAt+1);
          String novoLoop = "";
          if (nLoop>1){
            novoLoop = String("R(") + String(nLoop-1) + 
                       String("){") + strARepetir + 
                       String("}");
          }
          inStr = strARepetir + novoLoop + inStr;
        }
      }




    }else if(Sp=='_'){ // '_' define variavel global
      myRead();
      if (inStr.startsWith(String("DEBUG_"))){
        inStr.remove(0,6);
        if (inStr.startsWith(String("true"))){
          inStr.remove(0,4);
          DEBUG = true;
        }else if (inStr.startsWith(String("false"))){
          inStr.remove(0,5);
          DEBUG = false;
        }
      }else if (inStr.startsWith(String("ESCREVE_"))){
        inStr.remove(0,8);
        if (inStr.startsWith(String("true"))){
          inStr.remove(0,4);
          ESCREVE = true;
        }else if (inStr.startsWith(String("false"))){
          inStr.remove(0,5);
          ESCREVE = false;
        }
      }else if (inStr.startsWith(String("COMENTA_"))){
        inStr.remove(0,8);
        if (inStr.startsWith(String("true"))){
          inStr.remove(0,4);
          COMENTA = true;
        }else if (inStr.startsWith(String("false"))){
          inStr.remove(0,5);
          COMENTA = false;
        }
      }else if (inStr.startsWith(String("DtZeroPadrao_"))){
        inStr.remove(0,13);
        float newDtZero = myParseFloat();
        if (newDtZero>0.)
          DtZeroPadrao = newDtZero;
      }



    }else if(Sp==39){ // 39 eh aspas simples
      myRead();
      int ondeFechaAspas = inStr.indexOf(39);
      if (ondeFechaAspas==-1)
        ondeFechaAspas = inStr.length();
      if (COMENTA)
        Serial.println( inStr.substring(0,ondeFechaAspas) );
      inStr.remove(0,ondeFechaAspas+1);



    }else{ // Se nao for nenhum comando conhecido
      if (ESCREVE){
        Serial.print( F("skipped: ") );
        Serial.println( char(Sp) );
      }
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

int contaDigitosStr(){
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
    return -nDig;
  }else{
    return nDig;
  }
}
String digitosStr(){
  int nDig = contaDigitosStr();    
  if (nDig<=0){
    return String("");
  }else{
    String answer = (inStr.substring(0,nDig-1));
    inStr.remove(0,nDig-1);
    return answer;
  }
}



byte removeEspaco(){
  return removeEspaco(' ',' ');
}
byte removeEspaco(byte oqueMais){
  return removeEspaco(oqueMais,' ');  
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
  byte terminouChar[] = { ';','w','t','p','f',
                          'o','c','j','?','h',   
                          'i','s','S','u','d',
                          'W','T', 10, 13,'A',
                          'M','>','R',39,'[' };
  int nTerminouChar = sizeof(terminouChar)/sizeof(terminouChar[0]);
  byte pChar = removeEspaco(',');
  bool achou = false;
  for ( int nAtu=0; (!achou)&&(nAtu<nTerminouChar); nAtu++ ){
    achou = (pChar==terminouChar[nAtu]);
  }
  return achou;
}


bool ehControleValido(int n){
  return ((n>=0)&&(n<nControles));
}

bool ehMemoValido(int n){
  return ((n>=1)&&(n<=nMemos));
}

void verificaColchetes(){
  int abreColchetes = inStr.indexOf('[');
  while (abreColchetes>=0){
    int ondeSeta = inStr.indexOf('>');
    if ( (ondeSeta==-1) || (ondeSeta>abreColchetes) ){
      int fechaColchetes = inStr.indexOf(']');
      if (fechaColchetes>abreColchetes){
        int numeroMemoColchete = inStr.substring(
            abreColchetes+1,fechaColchetes).toInt();
        if (ehMemoValido(numeroMemoColchete)){
          inStr = inStr.substring(0,abreColchetes) +
                  memos[numeroMemoColchete-1] +
                  inStr.substring(fechaColchetes+1);
        }
      }
      abreColchetes = inStr.indexOf('[');
    }else{
      abreColchetes = -1;
    }
  }
}
