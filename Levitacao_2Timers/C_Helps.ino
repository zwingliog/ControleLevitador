// Helps - (ajuda para o usuario)

void mostraHelpComandos(){
  mostraHelpComandos( false );
}
void mostraHelpComandos(bool DETALHADO){
  Serial.println( F("List of valid commands:") );
  Serial.println( F("? > status [ ? ]") );
  Serial.println( F("w > wait (short) [ w DelayT ] (ms)") );
  Serial.println( F("W > wait (long) [ W DelayTRep ] (s)") );
  Serial.println( F("t > set DelayT [ t newDelayT ]  (ms)") );
  Serial.println( F("T > set DelayTRep [ t newDelayTRep ] (s)") );
  Serial.println( F("p > set Pot [ p newP ]  p?->mostra Pot") );
  Serial.println( F("f > set Phase [ f newF ]  f?->mostra Phase") );
  Serial.println( F("o > off [ o DtOff ]") );
  Serial.println( F("c > on-off cycles [ c DtOff, DtRep, nCycles ]") );
  Serial.println( F("j > jump [ j DeltaF, DtRep, nJumps ]") );
  Serial.println( F("u > up (360 degrees) [ u Dt ]") );
  Serial.println( F("d > down (360 degrees) [ d Dt ]") );
  Serial.println( F("s > switch (in steps) [ s DF, Dt ]") );
  Serial.println( F("S > switch (in steps) [ S DF, Steps, Dt ]") );
  if (DETALHADO){
    Serial.println( F("i > define Imax [ i Imax ];  i?->show IMax") );
    Serial.println( F("A > set port A0-A5 [ A 0-5, H_L_I_O ]; *A?->show port") );
    Serial.println( F("M > run Memory [ M> 1-9: @ ];  M?->show Memos" ) );
    Serial.println( F("> > save to Memory [ > 1-9: @ ] {legacy}") );
    Serial.println( F("B > Botoes: B>=save to [ B> 1-4: @ ];  B?=show Botoes") );
    Serial.println( F("R > Repeat [ R(#){@} ]") );
    Serial.println( F("_ > set global variable [_@_VALOR]:") );
    Serial.println( F("    for @=(DEBUG, ESCREVE, AVISATERMINOU, REVERSO)") );
    Serial.println( F("        VALOR=(true, false, flip)") );
    Serial.println( F("    for @=(DtZeroPadrao) VALOR = time in us") );
  }
  Serial.println( F("h > help [ h ]=simple help,  [h1]=full help") );
  if (DETALHADO){
    Serial.println( F(" OBS1: in 'S': Steps may be '!' ou '$':") );
    Serial.println( F("              '!'->2*(iMax+1) e '$'->(iMax+1)/2 per 360 degrees") );
    Serial.println( F(" OBS2: in 'f': '*'+DF=current phase+DF") );
  }
}
