// EEPROM - (rotinas para usar a memoria nao volatil)
#include <EEPROM.h>

char CABECALHO_EEPROM[] = "Lev_26a:";
const byte lenCABECALHO_EEPROM = sizeof(CABECALHO_EEPROM)/sizeof(CABECALHO_EEPROM[0]);

void iniciaEEPROM(){
  if (digitalRead( pinEEPROM )==HIGH){
    leEEPROM();
  }
}

bool existeEEPROM(bool so2pontos){
  for (int i=0; i<lenCABECALHO_EEPROM; i++){
    byte cr;
    EEPROM.get(i, cr);
    if ( cr!=CABECALHO_EEPROM[i] ){
      if ( (i==(lenCABECALHO_EEPROM-1)) || (!so2pontos) ){
        return false;
      }
    }
  }
  return true;
}

void apagaCabecalhoEEPROM(){
  for (int i=0; i<max(lenCABECALHO_EEPROM,1); i++){
    EEPROM.update(i,'\0');
  }
}

/*
 * CABECALHO_EEPROM (lenCABECALHO_EEPROM)
 * Imax (2)           (int)
 * potAtual (2)       (int)
 * faseAtual (4)      (float)
 * faseAtualReal (4)  (float)
 * DelayT (4)         (unsigned long)
 * DelayTRep (4)      (unsigned long)
 * byteConfig (1)     (byte)
 * DtZeroPadrao (4)   (float)
 * DtZero_uL (4)      (unsigned long)
 * 
 * byteConfig (1)     (byte)
 * 
 * memos (?)          (array de Strings)
 * botoes (?)         (array de Strings)
 */

void escreveEEPROM(String NomeEEPROM){
  EEPROM.put( 0, CABECALHO_EEPROM );  
  int posicaoEEPROM = lenCABECALHO_EEPROM;

  EEPROM.put(posicaoEEPROM, Imax);
  posicaoEEPROM += sizeof(Imax);

  EEPROM.put(posicaoEEPROM, potAtual);
  posicaoEEPROM += sizeof(potAtual);
  
  EEPROM.put(posicaoEEPROM, faseAtual);
  posicaoEEPROM += sizeof(faseAtual);

  EEPROM.put(posicaoEEPROM, faseAtualReal);
  posicaoEEPROM += sizeof(faseAtualReal);

  EEPROM.put(posicaoEEPROM, DelayT);
  posicaoEEPROM += sizeof(DelayT);

  EEPROM.put(posicaoEEPROM, DelayTRep);
  posicaoEEPROM += sizeof(DelayTRep);

  EEPROM.put(posicaoEEPROM, DtZeroPadrao);
  posicaoEEPROM += sizeof(DtZeroPadrao);

  EEPROM.put(posicaoEEPROM, DtZero_uL);
  posicaoEEPROM += sizeof(DtZero_uL);

  byte byteConfig = criaByteConfig();
  EEPROM.update(posicaoEEPROM, byteConfig);
  posicaoEEPROM += 1;
  
  posicaoEEPROM = gravaBlocoStringEEPROM( memos, nMemos, posicaoEEPROM );
  
  posicaoEEPROM = gravaBlocoStringEEPROM( botoes, nBotoes, posicaoEEPROM );

  posicaoEEPROM = gravaStringEEPROM( NomeEEPROM, posicaoEEPROM );
  
  if ( ESCREVE || AVISATERMINOU ){
    Serial.print( F("Atualizou EEPROM") );
    if (NomeEEPROM.length()>0){
      Serial.print( " '" );
      Serial.print( NomeEEPROM );
      Serial.print( "'" );
    }
    Serial.println( '.' );
  }
}

void leEEPROM(){
  leEEPROM(false);
}
void leEEPROM(bool so2pontos){
  if (!existeEEPROM(so2pontos)){
    return;
  }
  int posicaoEEPROM = lenCABECALHO_EEPROM;

  EEPROM.get(posicaoEEPROM, Imax);
  posicaoEEPROM += sizeof(Imax);

  EEPROM.get(posicaoEEPROM, potAtual);
  posicaoEEPROM += sizeof(potAtual);
  
  EEPROM.get(posicaoEEPROM, faseAtual);
  posicaoEEPROM += sizeof(faseAtual);

  EEPROM.get(posicaoEEPROM, faseAtualReal);
  posicaoEEPROM += sizeof(faseAtualReal);

  EEPROM.get(posicaoEEPROM, DelayT);
  posicaoEEPROM += sizeof(DelayT);

  EEPROM.get(posicaoEEPROM, DelayTRep);
  posicaoEEPROM += sizeof(DelayTRep);

  EEPROM.get(posicaoEEPROM, DtZeroPadrao);
  posicaoEEPROM += sizeof(DtZeroPadrao);

  EEPROM.get(posicaoEEPROM, DtZero_uL);
  posicaoEEPROM += sizeof(DtZero_uL);

  byte byteConfig = EEPROM.get(posicaoEEPROM, byteConfig);
  atualizaConfigFromByte( byteConfig );
  posicaoEEPROM += sizeof(byteConfig);
      
  posicaoEEPROM = leBlocoStringEEPROM( memos, nMemos, posicaoEEPROM, 'M' );
  if (posicaoEEPROM==0)
    return;
    
  posicaoEEPROM = leBlocoStringEEPROM( botoes, nBotoes, posicaoEEPROM, 'B' );
  if (posicaoEEPROM==0){
    return;
  }

  String NomeEEPROM;
  posicaoEEPROM = leStringEEPROM( NomeEEPROM, posicaoEEPROM );
  if (posicaoEEPROM==0){
    return;
  }
  
  if ( ESCREVE || AVISATERMINOU ){
    Serial.print( F("Leu EEPROM '") );
    if (NomeEEPROM.length()>0){
      Serial.print( NomeEEPROM );
    }else{
      Serial.print( CABECALHO_EEPROM );      
    }
    Serial.println( F("'.") );
  }
}

int leStringEEPROM( String &qual, int posicaoAtual ){
  return leStringEEPROM( qual, posicaoAtual, 255 );
}
int leStringEEPROM( String &qual, int posicaoAtual, byte maxLen ){
  qual = "";
  char cr;
  for (byte lenAtu=0; lenAtu<maxLen; lenAtu++){
    EEPROM.get( posicaoAtual, cr);
    posicaoAtual += 1;
    if (cr=='\0')
      break;
    qual += cr;
  }
  if (cr!='\0'){
    Serial.println( F("Erro lendo EEPROM.") );
    return 0;
  }
  return posicaoAtual;
}
int leBlocoStringEEPROM( String quem[], byte nQuem, int posicaoAtual, char nomeQuem ){
  byte new_nQuem;
  EEPROM.get(posicaoAtual, new_nQuem);
  if (new_nQuem != nQuem){
    Serial.print( F("Numero de ") );
    Serial.print( nomeQuem );
    Serial.println( F(" diferentes.") );
    return 0;
  }
  posicaoAtual += sizeof(nQuem);

  for (byte i=0; i<nQuem; i++){
    posicaoAtual = leStringEEPROM( quem[i], posicaoAtual );
    if (posicaoAtual==0){
      Serial.print( F("Erro lendo ") );
      Serial.println( nomeQuem );
      return 0;
    }
  }
  return posicaoAtual;
}

int gravaStringEEPROM( String qual, int posicaoAtual){
  for (int j=0; j<qual.length(); j++){
    EEPROM.update(posicaoAtual, qual[j] );
    posicaoAtual += 1;
  }
  EEPROM.update(posicaoAtual, '\0');
  posicaoAtual += 1;
  return posicaoAtual;
}
int gravaBlocoStringEEPROM( String quem[], byte nQuem, int posicaoAtual){
  EEPROM.put(posicaoAtual, nQuem);
  posicaoAtual += sizeof(nQuem);
  for (byte i=0; i<nQuem; i++){
    posicaoAtual = gravaStringEEPROM( quem[i], posicaoAtual );
  }
  return posicaoAtual;
}

/*
bool ESCREVE = false;
bool DEBUG = false;
bool COMENTA = true;
bool AVISATERMINOU = true;
bool REVERSO = false;
 */
byte criaByteConfig(){
  byte byteConfig = 0;
  byte novoValor = 1;
  if (ESCREVE){
    byteConfig += novoValor;
  }
  
  novoValor *= 2;
  if (DEBUG){
    byteConfig += novoValor;
  }
  
  novoValor *= 2;
  if (COMENTA){
    byteConfig += novoValor;
  }
  
  novoValor *= 2;
  if (AVISATERMINOU){
    byteConfig += novoValor;
  }
  
  novoValor *= 2;
  if (REVERSO){
    byteConfig += novoValor;
  }
  
  return byteConfig;
}

void atualizaConfigFromByte( byte byteConfig ){
  ESCREVE = (byteConfig%2==1);
  
  byteConfig /= 2;
  DEBUG = (byteConfig%2==1);
  
  byteConfig /= 2;
  COMENTA = (byteConfig%2==1);
  
  byteConfig /= 2;
  AVISATERMINOU = (byteConfig%2==1);
  
  byteConfig /= 2;  
  REVERSO = (byteConfig%2==1);
}
