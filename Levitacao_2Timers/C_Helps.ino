// Helps - (ajuda para o usuario)

void mostraHelpComandos(){
  mostraHelpComandos( false );
}
void mostraHelpComandos(bool DETALHADO){
  Serial.println( F("Valid commands:") );
  Serial.println( F("? > status") );
  Serial.println( F("p > set Pot [p#];  p?->show; p*->flip") );
  Serial.println( F("f > set Phase [f#]; f*#->add #;  f?->show") );
  Serial.println( F("o > off [o DtOff]") );
  Serial.println( F("c > on-off cycles [c DtOff, DtR, nC]") );
  Serial.println( F("j > jump [j DF, DtR, nJ]") );
  Serial.println( F("u > up (360 degrees) [u Dt]") );
  Serial.println( F("d > down (360 degrees) [d Dt]") );
  Serial.println( F("s > switch (in steps) [s DF, Dt]") );
  Serial.println( F("S > switch (in steps) [S DF, Steps, Dt]") );
  if (DETALHADO){
    Serial.println( F("w > wait (short) (ms)") );
    Serial.println( F("W > wait (long) (s)") );
    Serial.println( F("t > set DelayT (ms); 't?'->show") );
    Serial.println( F("T > set DelayTRep (s); 'T?'->show") );
    Serial.println( F("i > define Imax;  'i?'->show") );
    Serial.println( F("A > set port A0-A1 'A# H_L_I_O'; 'A?'->show") );
    Serial.println( F("M > Memory 'M#'; 'M#>@'->save @ to #;  'M?'->show" ) );
    //Serial.println( F("> > save to Memory [ > 1-9: @ ] {removido}") );
    Serial.println( F("B > Botoes: 'B#>@'->save @ to Botoes #;  'B?'->show") );
    Serial.println( F("R > Repeat: 'R(#){@}'->repeat @ for # times") );
    Serial.println( F("[ > wait with scape: '[port, timeMax]', wait A(port) for timeMax ms") );
    Serial.println( F("# > IF: '#port:@'-> if A(port), run @ (1 comando)") );
    Serial.println( F("_ > set global variable [_@>VALOR]:") );
    Serial.println( F("    for @=(DEBUG, ESCREVE, AVISATERMINOU, REVERSO)") );
    Serial.println( F("        VALOR=(true, false, flip, ?)") );
    Serial.println( F("    for @=DtZeroPadrao, time in ms (float)") );
    Serial.println( F("    for @=DtZero_uL, time in us") );
    Serial.println( F("_SAVE_CONFIG > save to EEPROM") );
    Serial.println( F("_LOAD_CONFIG > load from EEPROM") );
    Serial.println( F("_CLEAR_CONFIG > clear (erase) EEPROM") );
  }
  Serial.println( F("h > help,  h1->full help") );
  if (DETALHADO){
    Serial.println( F(" OBS1: in 'S': Steps '!'->2*(iMax+1) e '$'->(iMax+1)/2") );
    Serial.println( F(" OBS2: in 'f': '*'+DF=current phase+DF") );
    Serial.println( F(" OBS3: in c,j: nCycles,nJumps <0 means total time in ms (float)") );
  }
}
