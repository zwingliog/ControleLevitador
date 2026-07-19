// B2_ExecutaComandos (funcoes que executam os COMANDOs)

void executaComandos(){
  while( COMANDOsAtual>=0 ){  //adiconado em 19/06/2026 (controla melhor o tempo)

    char newCOMANDO = COMANDOs[COMANDOsAtual];
    bool emLoop = false;
    
    if (newCOMANDO=='w'){
      dtNEXT = dtNEXT + long1COMANDOs[COMANDOsAtual];
      
    }else if (newCOMANDO=='I'){
        mostraImax();
    }else if (newCOMANDO=='i'){
      // addCOMANDOs( 'i', newImax );
      if (dtNEXT){
        while( (micros()-tLAST)<dtNEXT ){}
        tLAST += dtNEXT;
      }else{
        tLAST = micros();
      }          
      mudaImax( intCOMANDOs[COMANDOsAtual], faseAtual );
      dtNEXT = 0;

    }else if( newCOMANDO=='T' ){
      mostraDelayT( str1COMANDOs[COMANDOsAtual] );
    }else if( newCOMANDO=='t' ){
      char QUAL = str1COMANDOs[COMANDOsAtual];
      char OQUE = str2COMANDOs[COMANDOsAtual];
      float newDelay = float1COMANDOs[COMANDOsAtual];
      float lastDelay = (QUAL=='t')? DelayT : DelayTRep;
      if( OQUE=='+' ){
        newDelay = lastDelay + newDelay;
      }
      if( QUAL=='t' ){
        DelayT = newDelay;
      }else if( QUAL=='T' ){
        DelayTRep = newDelay;
      }

    }else if (newCOMANDO=='P'){
        mostraPot();
    }else if (newCOMANDO=='p'){
      // addCOMANDOs( 'p', newP );
      if (dtNEXT){
        while( (micros()-tLAST)<dtNEXT ){}
        tLAST += dtNEXT;
      }else{
        tLAST = micros();
      }
      if (str1COMANDOs[COMANDOsAtual]=='*'){
        mudaPot( 1-potAtual );
      }else{
        mudaPot( intCOMANDOs[COMANDOsAtual] );
      }
      dtNEXT = 0;
    
    }else if (newCOMANDO=='F'){
        mostraFase();
    }else if (newCOMANDO=='f'){
      if (dtNEXT){
        while( (micros()-tLAST)<dtNEXT ){}
        tLAST += dtNEXT;
      }else{
        tLAST = micros();
      }
      float newF = (intCOMANDOs[COMANDOsAtual]==1)? faseAtual:0.;
      newF += float1COMANDOs[COMANDOsAtual];
      mudaFase( newF, true );
      dtNEXT = 0;
      
    }else if (newCOMANDO=='o'){
      // addCOMANDOs( 'o' DtOff_uL )   antes da v2026.5
      // addCOMANDOs_l1s1( 'o', DtOff_uL, tipoOff )
      // fazOff( DtOff_uL )
      unsigned long DtOff_uL = long1COMANDOs[COMANDOsAtual];
      if( str1COMANDOs[COMANDOsAtual]=='t' ){
        DtOff_uL = 1000uL*DelayT;
      }else if( str1COMANDOs[COMANDOsAtual]=='T' ){
        DtOff_uL = 1000uL*DelayTRep;
      }
      fazOff( DtOff_uL );
    
    }else if (newCOMANDO=='c'){
      // addCOMANDOs( 'c', DtOff_uL, DtRep_uL, nCiclos )  antes da v2026.5
      // addCOMANDOs_Il3f1s2( 'c', tempo2Ciclos, DtOff_uL, DtRep_uL, nCiclos, valorCiclos, tipoOff, tipoRep )
      // fazOnOffCycles( DtOff_uL, DtRep_uL, nCiclos )
      unsigned long DtOff_uL = long1COMANDOs[COMANDOsAtual];
      unsigned long DtRep_uL = long2COMANDOs[COMANDOsAtual];
      unsigned long nCiclos = long3COMANDOs[COMANDOsAtual];
      bool precisaRecalcularTempo = false;
      if( str1COMANDOs[COMANDOsAtual]=='t' ){
        DtOff_uL = 1000uL*DelayT;
      }else if( str1COMANDOs[COMANDOsAtual]=='T' ){
        DtOff_uL = 1000uL*DelayTRep;
      }
      if( str2COMANDOs[COMANDOsAtual]=='t' ){
        DtRep_uL = 1000uL*DelayT;
        precisaRecalcularTempo = true;
      }else if( str2COMANDOs[COMANDOsAtual]=='T' ){
        DtRep_uL = 1000uL*DelayTRep;
        precisaRecalcularTempo = true;
      }
      if( precisaRecalcularTempo && (intCOMANDOs[COMANDOsAtual]==1) ){
        nCiclos = (1000uL*(-float1COMANDOs[COMANDOsAtual]) + DtRep_uL/2)/DtRep_uL;
      }
      fazOnOffCycles( DtOff_uL, DtRep_uL, nCiclos );
    
    }else if (newCOMANDO=='j'){
      // addCOMANDOs( 'j', nJumps, DtRep_uL, DF )
      // addCOMANDOs_Il2f2s1( 'j', tempo2Jumps, (DtRep*1000uL), nJumps, DF, valorJumps, tipoRep )
      //fazJumps(DF, DtRep_uL, nJumps)
      float DF = float1COMANDOs[COMANDOsAtual];
      unsigned long DtRep_uL = long1COMANDOs[COMANDOsAtual]; 
      unsigned long nJumps = long2COMANDOs[COMANDOsAtual];
      bool precisaRecalcularTempo = false;
      if( str1COMANDOs[COMANDOsAtual]=='t' ){
        DtRep_uL = 1000uL*DelayT;
        precisaRecalcularTempo = true;
      }else if( str1COMANDOs[COMANDOsAtual]=='T' ){
        DtRep_uL = 1000uL*DelayTRep;
        precisaRecalcularTempo = true;
      }
      if( precisaRecalcularTempo && (intCOMANDOs[COMANDOsAtual]==1) ){
        nJumps = (1000uL*(-float2COMANDOs[COMANDOsAtual]) + DtRep_uL)/(2*DtRep_uL);
        nJumps = 2*nJumps;
      }
      fazJumps( DF, DtRep_uL, nJumps );
    
    }else if (newCOMANDO=='s'){
      // addCOMANDOs( 's', Steps, Dt_uL, DF );
      //fazSteps(DF, Steps, Dt_uL)
      fazSteps(float1COMANDOs[COMANDOsAtual],
                long1COMANDOs[COMANDOsAtual],
                long2COMANDOs[COMANDOsAtual] );
    
    }else if (newCOMANDO=='A'){
      mostraControles();
    }else if (newCOMANDO=='a'){
      // addCOMANDOs('A',qualControle,strModo,strValue);
      char strModo = str1COMANDOs[COMANDOsAtual];
      char strValue = str2COMANDOs[COMANDOsAtual];
      int qualControle = intCOMANDOs[COMANDOsAtual];
      if (strModo=='?'){
        mostraControles( qualControle );
      }else{
        if (dtNEXT){
          while( (micros()-tLAST)<dtNEXT ){}
          tLAST += dtNEXT;
        }else{
          tLAST = micros();
        }
        if (strModo!=' '){
          if (strModo=='#'){
            strModo = 
              (modoControle[qualControle]==INPUT)? 'O':'I';
          }
          modoControle[qualControle] = 
                      (strModo=='I')? INPUT: OUTPUT;
          pinMode( pinControle[qualControle], 
                    modoControle[qualControle] );
        }
        if (strValue!=' '){
          if (strValue=='*'){
            strValue = 
              (valorControle[qualControle]==LOW)? 'H':'L';
          }
          valorControle[qualControle] = 
                        (strValue=='L')? LOW : HIGH;
          digitalWrite( pinControle[qualControle], 
                  valorControle[qualControle] );
        }
        dtNEXT = 0;
        if (DEBUG){ mostraControles( qualControle ); }
      }

    }else if (newCOMANDO=='L'){
      //addCOMANDOs_i2I( 'L', nLoop, nLoop, nCOMANDOInicioLoop );
      long nLoop = long1COMANDOs[COMANDOsAtual];
      if (nLoop>0){
        long1COMANDOs[COMANDOsAtual] = nLoop-1;
        COMANDOsAtual = intCOMANDOs[COMANDOsAtual];
        emLoop = true;
      }else{
        long1COMANDOs[COMANDOsAtual] = long2COMANDOs[COMANDOsAtual];
      }

    }else if (newCOMANDO=='['){
      int porta = A0 + intCOMANDOs[COMANDOsAtual];
      unsigned long timeMax = long1COMANDOs[COMANDOsAtual];
      bool tipoEsperado = LOW;
      if (str1COMANDOs[COMANDOsAtual]=='~'){
        tipoEsperado = !digitalRead(porta);
      }else if (str1COMANDOs[COMANDOsAtual]=='L'){
        tipoEsperado = LOW;
      }else if (str1COMANDOs[COMANDOsAtual]=='H'){
        tipoEsperado = HIGH;
      }
      unsigned long timeIniAtu = micros();
      while( (digitalRead(porta)!=tipoEsperado) && ((micros()-timeIniAtu)<timeMax) ){}

    }else if (newCOMANDO=='#'){
      int porta = A0 + intCOMANDOs[COMANDOsAtual];
      bool tipoEsperado = (str1COMANDOs[COMANDOsAtual]=='L')? LOW : HIGH;
      if (digitalRead(porta)!=tipoEsperado){
        COMANDOsAtual += 1;
      }

    }


    if (COMANDOsAtual<nCOMANDOs){
      if (!emLoop){
        COMANDOsAtual += 1;
      }
    }else{
      COMANDOsAtual = -1;
      apagaCOMANDOs();
      if (ESCREVE || DEBUG || AVISATERMINOU){ Serial.println('.'); }
    }
  }
}