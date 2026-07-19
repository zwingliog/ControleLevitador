// B1_InStr2Comandos (funcoes para transformar a String de entrada em COMANDOs)

void InStr2Comandos(){
  
  byte Sp = removeEspaco();
  if ( (Sp==10) || (Sp==13) || (Sp==',') || (Sp==';') ){
    removeEspaco(Sp);

  }else if( (Sp=='w') || (Sp=='W') ){ // w,W - wait
    // w -> wait short (ms) : w [Dt]
    // W -> Wait long (s) : W [DtRep]
    if (tLAST==0){
      tLAST = micros();
    }
    myRemove();
    float newDelay = 0.;
    if (!jaTerminou()){
      newDelay = myParseFloat();
      if (Sp=='W'){
        newDelay = 1000.0*newDelay;
      }
    }
    if (newDelay<=0.){
      if (Sp=='w'){
        newDelay = DelayT;
      }else if (Sp=='W'){
        newDelay = DelayTRep;
      }
    }
    if (newDelay>0.){
      addCOMANDOs_l1('w', (1000uL*newDelay) );
      if (DEBUG){
        Serial.print( F("wait ") );
        Serial.print( newDelay );
        Serial.println( F("ms" ) );
      }
    }

  }else if( (Sp=='t') || (Sp=='T') ){ // t, T -> set Delay
    // t newDelayT
    // t?
    // T newDelayTRep
    // T?
    myRemove();
    if( myPeek()=='?' ){
      removeEspaco('?');
      //mostraDelayT();
      addCOMANDOs_s1( 'T', Sp );
    }else if( !jaTerminou() ){
      char tipo = '=';
      if( myPeek()=='*' ){
        removeEspaco( '*' );
        tipo = '+';
      }
      if (contaDigitosStr()>0){
        float newDelay = myParseFloat();
        if (newDelay!=0.){
          addCOMANDOs_f1s2( 't', newDelay, Sp, tipo );
          if (ESCREVE){ addCOMANDOs_s1( 'T', Sp ); }
        }
      }
    }


    
  }else if(Sp=='?'){ // ?-> status
    myRemove();
    mostraStatus();
    


  }else if(Sp=='p'){
    // p VALOR
    // p* (switch)
    // p?
    myRemove();
    if (myPeek()=='?'){
      addCOMANDOs_0('P');
      // VIROU_COMANDOs P
      myRemove();
    }else if (myPeek()=='*'){
      removeEspaco('*');
      addCOMANDOs_s1('p', '*');
    }else if ((!jaTerminou())&&(contaDigitosStr()>0) ){
      int newP = myParseInt();
      if ( (newP>=-2)&&(newP<=3) ){
        addCOMANDOs_I('p', newP);
        // VIROU_COMANDOs p
      }
    }


  }else if(Sp=='f'){
    // f newF
    // f * newDF
    // f?
    myRemove();
    if (myPeek()=='?'){
      addCOMANDOs_0('F');
      // VIROU_COMANDOs F
      myRemove();
    }else if (!jaTerminou()){
      int faseRelativa = 0;
      if ( removeEspaco()=='*' ){
        removeEspaco('*');
        faseRelativa = 1;
      }
      if (contaDigitosStr()>0){
        float newDF = myParseFloat();
        addCOMANDOs_If1('f', faseRelativa, newDF);
        // VIROU_COMANDOs f
      }
    }


  }else if (Sp=='i'){
    // i newImax
    // i?
    myRemove();
    if (myPeek()=='?'){
      addCOMANDOs_0('I');
      // VIROU_COMANDOs I
      myRemove();
    }else if (!jaTerminou()){
      bool mudouImax = false;
      int newImax = myParseInt();
      if ( (newImax>=ImaxMIN) && (newImax<=ImaxMAX) ){
        mudouImax = true;
      }
      if ( mudouImax ){
        addCOMANDOs_I( 'i', newImax );
        // VIROU_COMANDOs i
      }
    }



  }else if( (Sp=='o') || (Sp=='c') ){ 
    // o-> off : o DtOff
    // c-> on-off cycles : c DtOff, DtRep, nCiclos
    myRemove();
    float DtOff = 0;
    char tipoOff = '=';
    if (removeEspaco()=='*'){
      removeEspaco( '*' );
      DtOff = DelayT;
    }else if (myPeek()=='t'){
      removeEspaco( 't' );
      tipoOff = 't';
      DtOff = DelayT;
    }else if (myPeek()=='T'){
      removeEspaco( 'T' );
      tipoOff = 'T';
      DtOff = DelayTRep;
    }else{
      DtOff = myParseFloat();
    }
    float DtRep = 0;
    char tipoRep = '=';
    unsigned long nCiclos = 1;
    int tempo2Ciclos = 0;
    float valorCiclos = 0.;
    if ( Sp=='c' ){
      if (removeEspaco(',')=='*'){
        removeEspaco( '*' );
        DtRep = DelayTRep;
      }else if (myPeek()=='t'){
        removeEspaco( 't' );
        tipoRep = 't';
        DtRep = DelayT;
      }else if (myPeek()=='T'){
        removeEspaco( 'T' );
        tipoRep = 'T';
        DtRep = DelayTRep;
      }else{
        DtRep = myParseFloat();
      }
      if (removeEspaco(',')=='*'){
        // nCiclos = 1;
        removeEspaco( '*' );
      }else{
        valorCiclos = myParseFloat();
        //Serial.println( valorCiclos );
        if (valorCiclos>=0){
          nCiclos = valorCiclos;
        }else{
          nCiclos = (-valorCiclos + 0.5*DtRep)/DtRep;
          tempo2Ciclos = 1;
        }
      }
    }
    if ( (DtRep==0.) && (tipoRep=='=') && (nCiclos>0) ){
      nCiclos = 1;
    }
    //if (ESCREVE){ Serial.println( char(Sp) ); }
    if (Sp=='o'){
      addCOMANDOs_l1s1( 'o', (DtOff*1000uL), tipoOff );
    }else if ((DtOff>0.) && (nCiclos>0)){
      if (DtRep<DtOff){
        DtRep=DtOff;
      }
      addCOMANDOs_Il3f1s2( 'c', tempo2Ciclos, (DtOff*1000uL), (DtRep*1000uL), nCiclos, 
                            valorCiclos, tipoOff, tipoRep  );
    }
    // VIROU_COMANDOs o, c
    

  }else if(Sp=='j'){ // j-> jump : j DF, DtRep, nJumps
    myRemove();
    if (!jaTerminou()){
      float DF = myParseFloat();
      float DtRep = 0;
      char tipoRep = '=';
      unsigned long nJumps = 2;
      int tempo2Jumps = 0;
      float valorJumps;
      removeEspaco(',');
      if (myPeek()=='*'){
        removeEspaco('*');
        DtRep = DelayTRep;
      }else if( myPeek()=='t' ){
        removeEspaco('t');
        DtRep = DelayT;
        tipoRep = 't';
      }else if( myPeek()=='T' ){
        removeEspaco('T');
        DtRep = DelayTRep;
        tipoRep = 'T';
      }else{
        DtRep = myParseFloat();
      }
      removeEspaco(',');
      if (!jaTerminou()){
        valorJumps = myParseFloat();
        if (valorJumps>=0){
          if (valorJumps<2){
            valorJumps = 2;
          }
          nJumps = 2*(((unsigned long)valorJumps+1)/2); //must be even
        }else{
          nJumps = ( ((unsigned long)(-valorJumps) + DtRep)/(2*DtRep) ); //must be even
          nJumps = 2*nJumps;
          tempo2Jumps = 1;
        }
      }
      if ( (DF!=0.) && (DtRep>0.) && (nJumps>0) ){
        //if (ESCREVE){ Serial.println( char(Sp) ); }
        addCOMANDOs_Il2f2s1( 'j', tempo2Jumps, (DtRep*1000uL), nJumps, DF, valorJumps, tipoRep );
        // VIROU_COMANDOs j
        // fazJumps( DF, DtRep_uL, nJumps);
      }
    }
    

  }else if( (Sp=='s') || (Sp=='S') || (Sp=='u') || (Sp=='d') ){ 
    // s -> steps: s DF, Dt
    // S -> steps: S DF, Steps, Dt
    // u -> up:    u Dt
    // d -> down:  d Dt
    myRemove();
    float DF;
    if (Sp=='u'){ // u -> up : u Dt
      DF = +360.0;          
    }else if (Sp=='d'){ // d -> down : d Dt
      DF = -360.0;        
    }else if ( (Sp=='s') || (Sp=='S') ){ // s,S
        // s -> steps: s DF, Dt
        // S -> steps: S DF, Steps, Dt
      DF = myParseFloat();
    }
    unsigned long Steps= (Imax+1)*(abs(DF)/360.0);
    if ( (Sp=='S') && (!jaTerminou()) ){
      char nextChar = removeEspaco(',');
      if (nextChar=='!'){
          removeEspaco('!');
          Steps = 2*(Imax+1)*(abs(DF)/360.0);
      }else if (nextChar=='$'){
          removeEspaco('$');
          Steps = ((Imax+1)*(abs(DF)/360.0))/2;
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
        if (Dt==0.){
          Dt = DelayT;
        }
      }
    }
    if ( (Steps>0) && (Dt>0.) ){
      addCOMANDOs_l2f1( 's', Steps, (Dt*1000uL), DF );
      // VIROU_COMANDOs s
    }


  }else if(Sp=='h'){
    // h
    // h1
    myRemove();
    int Nivel=0;
    if (jaTerminou()){
      removeEspaco(';');
    }else{
      Nivel = myParseInt();
    }
    mostraHelpComandos( Nivel!=0 );  


  }else if(Sp=='A'){
    // A QUAL MODO
    // A ?
    myRemove();
    if (myPeek()=='?'){
      myRemove();
      addCOMANDOs_0('A');
      // VIROU_COMANDOs a (->A em 05/07/26)
    }else if ( !jaTerminou() ){
      int qualControle=100;
      int newQual = myParseInt();
      if ( !ehControleValido(newQual) ){
        erroNoComando = true;
      }else{
        qualControle = newQual;
        Sp = removeEspaco();
        if (Sp=='?'){
          myRemove();
          addCOMANDOs_Is2('a', qualControle, '?', ' ');
        }else{
          bool mudouValue = false;
          bool mudouModo = false;
          char strModo = ' ';
          char strValue = ' ';
          if (Sp=='L'){
            strValue = 'L';
            mudouValue = true;
          }else if (Sp=='H'){
            strValue = 'H';
            mudouValue = true;
          }else if (Sp=='I'){
            strModo = 'I';
            mudouModo = true;
          }else if (Sp=='O'){
            strModo = 'O';
            mudouModo = true;
          }else if (Sp=='P'){
            strModo = 'I';
            strValue = 'H';
            mudouModo = true;
            mudouValue = true;
          }else if (Sp=='*'){ //para trocar estado (LOW->HIGH, H->L)
            strValue = '*';
            mudouValue = true;
          }else if (Sp=='#'){ //para trocar tipo (INPUT->OUTPUT, O->I)
            strModo = '#';
            mudouModo = true;
          }
          if ( (mudouValue)&&(!mudouModo) ){
            mudouModo = primeiroUsoControle[newQual];
            if (mudouModo){ strModo = 'O'; }
          }
          primeiroUsoControle[newQual] = false;
          removeEspaco(Sp,';');
          if ( (mudouValue) || (mudouModo) ){
            addCOMANDOs_Is2('a', qualControle, strModo, strValue);
            // VIROU_COMANDOs A  (->a em 05/07/26)
          }
        }
      }
    }



  }else if (Sp=='B'){
    // B QUAL > COMANDO
    // B QUAL ?
    // B ?
    myRemove();
    if (myPeek()=='?'){
      myRemove();
      mostraBotoes();
    }else{
      int numBotao = myParseInt();
      if (ehBotaoValido(numBotao)){
        if (myPeek()=='>'){
          myRemove();
          int nextEnd = inStr.indexOf(endChar);
          if (nextEnd>=0){
            botoes[numBotao-1] = inStr.substring(0,nextEnd);
            inStr.remove(0,nextEnd+1);
            if (ESCREVE){ mostraBotoes(numBotao); }
          }
        }else if (myPeek()=='?'){
          myRemove();
          mostraBotoes(numBotao);
        }
      }
    }
    


  }else if (Sp=='M'){
    myRemove();
    if (myPeek()=='?'){
      myRemove();
      mostraMemos();
    }else{
      int numMemo = myParseInt();
      if ( ehMemoValido(numMemo) ){
        if (myPeek()=='>'){ // padrao novo antigo ("M#>@")
          myRemove();
          int nextEnd = inStr.indexOf(endChar);
          if (nextEnd>=0){
            memos[numMemo-1] = inStr.substring(0,nextEnd);
            inStr.remove(0,nextEnd+1);
            if (ESCREVE){ mostraMemos(numMemo); }
          }
        }else{
          if ((inStr.charAt(0)=='?')){
            myRemove();
            mostraMemos(numMemo); 
          }else{
            String oldStr = inStr;
            inStr = memos[numMemo-1];
            inStr += oldStr;
          }
        }
      }
    }  



  }else if (Sp=='R'){
    myRemove();
    bool repeticaoValida = true;
    if (myRead()!='('){
      repeticaoValida = false;
      erroNoComando = true;
    }
    if ( (repeticaoValida) && (inStr.length()>=(1+2+1)) ){
      long nLoop = myParseInt();
      if ( inStr.startsWith(String("){")) ){
        inStr.remove(0,2);
        //int endAt = inStr.indexOf('}');
        int endAt = ondeFecha( '}', '{', 1 );
        if (endAt>0){
          String strARepetir = inStr.substring(0,endAt);
          inStr.remove(0,endAt+1);
          String novoLoop = "";
          if (nLoop==0){
            strARepetir = "";
          }else if (nLoop>1){
            novoLoop = String("l") + String(nLoop-1) +
                        String(" ") + String(nCOMANDOs) +
                        String(";");
          }
          String oldStr = inStr;
          inStr = strARepetir; 
          inStr += novoLoop;
          inStr += oldStr;
        } else {
          inStr.remove( 0, inStr.length() );
          erroNoComando = true;
        }
      }
    }



  }else if (Sp=='l'){
    myRemove();
    unsigned long nLoop = myParseInt();
    int nCOMANDOInicioLoop = myParseInt();
    addCOMANDOs_Il2( 'L', nCOMANDOInicioLoop, nLoop, nLoop );



  }else if(Sp=='_'){ // '_' define variavel global
    myRemove();
    if (inStr.startsWith("SAVE_CONFIG")){
      inStr.remove(0,11);
      String nomeConfig = leAspas();
      escreveEEPROM( nomeConfig );
    }else if (inStr.startsWith("LOAD_CONFIG")){
      inStr.remove(0,11);
      leEEPROM(true);
    }else if (inStr.startsWith("CLEAR_CONFIG")){
      inStr.remove(0,12);
      apagaCabecalhoEEPROM();
      if (ESCREVE || AVISATERMINOU ){
        Serial.println( F("apagou EEPROM.") );
      }
    }else if (defineVariavelGlobalBool("DEBUG", DEBUG)){
    }else if (defineVariavelGlobalBool("ESCREVE", ESCREVE)){
    }else if (defineVariavelGlobalBool("REVERSO", REVERSO)){
    }else if (defineVariavelGlobalBool("COMENTA", COMENTA)){
    }else if (defineVariavelGlobalBool("AVISATERMINOU", AVISATERMINOU)){
    }else if (inStr.startsWith("DtZeroPadrao")){
      inStr.remove(0,12);
      if (myPeek()=='>'){
        myRemove();
        float newDtZero = myParseFloat();
        if (newDtZero>0.){
          DtZeroPadrao = newDtZero;
          DtZero_uL = newDtZero*1000uL;
        }
      }else if (myPeek()=='?'){
        myRemove();
        mostraDtZero();
      }
    }else if (inStr.startsWith("DtZero_uL")){
      inStr.remove(0,9);
      if (myPeek()=='>'){
        myRemove();
        unsigned long newDtZero_uL = myParseInt();
        if (newDtZero_uL>0){
          DtZero_uL = newDtZero_uL;
          DtZeroPadrao = newDtZero_uL/1000.0;
        }
      }else if (myPeek()=='?'){
        myRemove();
        mostraDtZero();
      }
    }



  }else if(Sp==39){ // 39 eh aspas simples
    //myRemove(); // o leAspas ja verifica o incio e remove as aspas
    String TextoEntreAspas = leAspas();
    if (COMENTA || DEBUG || ESCREVE){
      Serial.println( TextoEntreAspas );
    }



  }else if(Sp=='['){
    // [ porta, timeMax ]
    // [ ^ porta, timeMax ]
    // [ ~ porta, timeMax ]
    // -> (aguarda pino A_porta ficar {LOW, HIGH ou SWITCH} por até timeMax ms)
    //myRemove();
    removeEspaco(Sp);
    int ondeTermina = ondeFecha( ']', 0, 1 );
    char tipo = 'L';
    int porta = 0;
    float timeMax = 0.;
    if (ondeTermina>0){
      if (myPeek()=='^'){
        tipo = 'H';
        removeEspaco('^');
      }else if(myPeek()=='~'){
        tipo = '~';
        removeEspaco('~');
      }
      if (myPeek()!=','){
        porta = myParseInt();
      }
      removeEspaco(',');
      timeMax = myParseFloat();
      removeEspaco(']',';');
      if (timeMax>0.){
        addCOMANDOs_Il1s1('[', porta, timeMax*1000uL, tipo );
      }else{
        erroNoComando = true;
      }
    }else{
      erroNoComando = true;
    }



  }else if(Sp=='#'){
    // # porta: comando = # L porta: comando
    // # H porta: comando
    // -> só executa o comando se A_porta estiver {LOW ou HIGH}
    //myRemove();
    removeEspaco(Sp);
    char tipo = 'L';
    int porta = 0;
    if (myPeek()=='^'){
      tipo = 'H';
      removeEspaco('^');
    }
    if (myPeek()!=':'){
      porta = myParseInt();
    }
    if (myPeek()==':'){
      removeEspaco(':');
      if ((porta>=0) && (porta<=5)){
        addCOMANDOs_Is1('#', porta, tipo );
      }else{
        erroNoComando = true;
      }
    }else{
      erroNoComando = true;        
    }



  }else{ // Se nao for nenhum comando conhecido
    erroNoComando = true;
  }

  if (erroNoComando){
    if (DEBUG || ESCREVE){
      Serial.print( F("skipped: ") );
      Serial.print( inStr[0] );
      Serial.println( inStr );     
    }
    inStr = "";
    apagaCOMANDOs();
    erroNoComando = false;
  }

}



/* 
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
*/
void myRemove(){
  inStr.remove(0,1);
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
    nextChar = myPeek();
    if ( (nextChar==' ')||(nextChar==oqueMais1)||(nextChar==oqueMais2) ){
      myRemove();
    }else{
      removendo = false;
    }
  }
  if (nextChar==-1)
    nextChar = 0;
  return byte(nextChar);
}

bool jaTerminouSemT(){
  return jaTerminou( 2 );
}
bool jaTerminou(){
  return jaTerminou(0);
}
bool jaTerminou(int ignoraUltimosN){
  byte terminouChar[] = { 'w', 'p', 'f', 'i', 
                          'o', 'c', 'j', 'h',   
                          's', 'S', 'u', 'd',
                          'l', 'R', 
                          'W', 'A', 'M', 'B', 
                          '_', ';',
                          10, 13, 39,
                          't', 'T' };
  int nTerminouChar = sizeof(terminouChar)/sizeof(terminouChar[0]);
  byte pChar = removeEspaco(',');
  bool achou = false;
  for ( int nAtu=0; nAtu<(nTerminouChar-ignoraUltimosN); nAtu++ ){
    achou = (pChar==terminouChar[nAtu]);
    if (achou){
      break;
    }
  }
  return achou;
}


String leAspas(){
  String conteudoAspas = "";
  if (inStr.startsWith("'")) { // 39 eh aspas simples
    myRemove();
    int ondeFechaAspas = inStr.indexOf(39);
    if (ondeFechaAspas==-1){
      ondeFechaAspas = inStr.length();
    }
    conteudoAspas = inStr.substring(0,ondeFechaAspas);
    inStr.remove(0,ondeFechaAspas+1);
  }
  return conteudoAspas;
}


bool defineVariavelGlobalBool(String nomeVar, bool &VarBool){
  if (inStr.startsWith(nomeVar)){
    inStr.remove(0,nomeVar.length());
    if (myPeek()=='>'){
      myRemove();
      if (inStr.startsWith("true")){
        inStr.remove(0,4);
        VarBool = true;
      }else if (inStr.startsWith("false")){
        inStr.remove(0,5);
        VarBool = false;
      }else if (inStr.startsWith("flip")){
        inStr.remove(0,4);
        VarBool = !VarBool;
      }
    }else if (inStr.startsWith("?")){
      myRemove();
      mostraVar( nomeVar, VarBool );
    }
    return true;
  }
  return false;
}


int ondeFecha( char strFecha, char strAbre, int quantasAbriu ){
  int posicaoAtual = 0;
  bool temAbre = (strAbre>0);
  while( posicaoAtual<inStr.length() ){
    if (inStr[posicaoAtual]==strFecha){
      quantasAbriu -= 1;
      if (quantasAbriu==0){
        break;
      }
    }else if( temAbre && (inStr[posicaoAtual]==strAbre) ){
      quantasAbriu+= 1;
    }
    posicaoAtual += 1;
  }
  if (quantasAbriu==0){
    return posicaoAtual;
  }else{
    return -1;
  }
}
