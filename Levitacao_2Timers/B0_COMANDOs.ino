// B0_COMANDOs (funcoes para gerenciar os COMANDOs)




void apaga1COMANDO(int QUAL){
  // COMANDOs, long1COMANDOs, long2COMANDOs, long3COMANDOs, 
  // float1COMANDOs, float2COMANDOs, str1COMANDOs, str2COMANDOs, intCOMANDOs 
  COMANDOs[QUAL] = ' ';
  long1COMANDOs[QUAL] = 0L;
  long2COMANDOs[QUAL] = 0L;
  long3COMANDOs[QUAL] = 0L;
  float1COMANDOs[QUAL] = 0.;
  float2COMANDOs[QUAL] = 0.;
  str1COMANDOs[QUAL] = ' ';
  str2COMANDOs[QUAL] = ' ';
  intCOMANDOs[QUAL] = 0;
}

void apagaCOMANDOs(){
  for (int i=0; i<nCOMANDOsMax; i++){
    apaga1COMANDO(i);
  }
  nCOMANDOs = 0;
  COMANDOsAtual = -1;
}



void addCOMANDOs_0(char newC){
  addCOMANDOs( newC, 0L, 0L, 0L, 0., 0., ' ', ' ', 0 );
}

void addCOMANDOs_I( char newC, int Int ){
  addCOMANDOs( newC, 0L, 0L, 0L, 0., 0., ' ', ' ', Int );
}

void addCOMANDOs_Il1s1( char newC, int Int, unsigned long long1, char str1){
  addCOMANDOs( newC, long1, 0L, 0L, 0., 0., str1, ' ', Int );
}

void addCOMANDOs_Il2( char newC, int Int, unsigned long long1, unsigned long long2 ){
  addCOMANDOs( newC, long1, long2, 0L, 0., 0., ' ', ' ', Int );
}

void addCOMANDOs_Il2f2s1( char newC, int Int, unsigned long long1, unsigned long long2, 
                 float float1, float float2, char str1){
  addCOMANDOs( newC, long1, long2, 0L, float1, float2, str1, ' ', Int );
}

void addCOMANDOs_Il3f1s2( char newC, int Int, unsigned long long1, unsigned long long2, unsigned long long3, 
                 float float1, char str1, char str2){
  addCOMANDOs( newC, long1, long2, long3, float1, 0., str1, str2, Int );
}

void addCOMANDOs_If1( char newC, int Int, float float1){
  addCOMANDOs( newC, 0L, 0L, 0L, float1, 0., ' ', ' ', Int );
}

void addCOMANDOs_Is1(char newC, int Int, char str1){
  addCOMANDOs( newC, 0L, 0L, 0L, 0., 0., str1, ' ', Int );
}

void addCOMANDOs_Is2(char newC, int Int, char str1, char str2){
  addCOMANDOs( newC, 0L, 0L, 0L, 0., 0., str1, str2, Int );
}

void addCOMANDOs_l1( char newC, unsigned long long1 ){
  addCOMANDOs( newC, long1, 0L, 0L, 0., 0., ' ', ' ', 0 );
}

void addCOMANDOs_l1s1( char newC, unsigned long long1, char str1){
  addCOMANDOs( newC, long1, 0L, 0L, 0., 0., str1, ' ', 0 );
}

void addCOMANDOs_l2f1( char newC, unsigned long long1, unsigned long long2, float float1 ){
  addCOMANDOs( newC, long1, long2, 0L, float1, 0., ' ', ' ', 0 );
}

void addCOMANDOs_l3( char newC, unsigned long long1, unsigned long long2, unsigned long long3 ){
  addCOMANDOs( newC, long1, long2, long3, 0., 0., ' ', ' ', 0 );
}

void addCOMANDOs_f1s2(char newC, float float1, char str1, char str2){
  addCOMANDOs( newC, 0L, 0L, 0L, float1, 0., str1, str2, 0 );
}

void addCOMANDOs_s1(char newC, char str1){
  addCOMANDOs( newC, 0L, 0L, 0L, 0., 0., str1, ' ', 0 );
}

void addCOMANDOs_s2(char newC, char str1, char str2){
  addCOMANDOs( newC, 0L, 0L, 0L, 0., 0., str1, str2, 0 );
}

void addCOMANDOs(char newC, unsigned long long1, unsigned long long2, unsigned long long3, 
                 float float1, float float2, char str1, char str2, int int1){
  if (nCOMANDOs<nCOMANDOsMax){
    int QUAL = nCOMANDOs;
    COMANDOs[QUAL] = newC;
    long1COMANDOs[QUAL] = long1;
    long2COMANDOs[QUAL] = long2;
    long3COMANDOs[QUAL] = long3;
    float1COMANDOs[QUAL] = float1;
    float2COMANDOs[QUAL] = float2;
    str1COMANDOs[QUAL] = str1;
    str2COMANDOs[QUAL] = str2;
    intCOMANDOs[QUAL] = int1;
    nCOMANDOs += 1;
  }
}
