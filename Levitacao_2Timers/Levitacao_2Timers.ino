// Levitacao_2Timers [controle da fase usando ICR1 ou Timers 1 e 2]
// Gera sinais com a defasagem escolhida
// TIPOTimer=1  -> entre os pinos 9 e 10 (de 244Hz a 400kHz)
// TIPOTimer=12 -> entre (9,10) e (11,3) (de 31.25kHz a 400kHz)
//                 [10 = not 9 ] e [3 = not 11] para nao usar inversor externo
//#define TIPOTimer 1
int TIPOTimer = 1;

#define VERSAO "v2026.5"
// v2026.5 de 16-/07/2026 (t=DelayT e T=DelayTRep são variaveis para os comandos 'o', 'c' e 'j')
// v2026.4 de 15-/07/2026 (adicionado espera '[...]' e if '#' com modificadores '^'->HIGH e '~'->flip)
// v2026.31 de 13/07/2026 (aumento do intervalo de frequencias geradas, 'o' e 'c' invertem pot)
// v2026.3 de 05-/07/2026 (DtRep do 'c' corrigido, separação em B0_..., B1_... e B2_....)
// v2026.2 de 23-24/06/2026 (controle da EEPROM, forca nJumps a ser par, fReal de [0.0 a 360.0[)
// v2026.1 de 19/06/2026 (editar botoes + Rep<0->tempo + flip + B_...ino->B_,C_,D_)
// v1.0.10 de 25-26/03/2024 (adicionado os botoes)
// v1.0.9 de 02/03/2024 (tNEXT -> tLAST e dtNEXT, para evitar overflow)
// v1.0.8.1 de 02/03/2024 (mudanca inicializacao do pinControle->Input)
// v1.0.8 de 24/12/2023 (mudanca na forma de entrada do texto)
// v1.0.7 de 20/12/2023 (correcao dos tempos dos comandos)
// v1.0.6 de 19/12/2023 (correcao do tempo do comando "o")
// v1.0.5 de 27/10/2023 (pequenas correcoes)
// v1.0.4 de 02/10/2023 (adicao da variavel global REVERSO)
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

//#define TIME_OUT  5
//#define pinEnA 5
//#define pinEnB 6
//#define pinEEPROM  13
#define nCOMANDOsMax 25
char pinEnA = 5;
char pinEnB = 6;
char pinEEPROM = 13;
long baudRate = 250000;
long timeOUT = 5;

unsigned int Imax = 199; // f=16MHz/(2*(Imax+1)) preve 199 para 40kHz e 319 para 25kHz
int fatorImax = 1; // fator multiplicativo do Imax para Timer2 (8-bits) (???)
unsigned int ImaxMIN = 19; // mudado para 19 (400kHz) na v2026.31
unsigned int ImaxMAX = (TIPOTimer==1)? 32767 : 255;// mudado na v2026.31 e na v2026.4
float faseAtual = 0.0;
float faseAtualReal = 0.0;
int potAtual = 1;
bool oldFF = false;
float DelayT = 10.0; // 10ms
float DelayTRep = 1000.0; // 1s (1000ms)

unsigned long tLAST = 0;
unsigned long dtNEXT = 0;

// COMANDOs
bool erroNoComando = false;
int nCOMANDOs = 0;
int COMANDOsAtual = -1;
// COMANDOs, long1COMANDOs, long2COMANDOs, long3COMANDOs, 
// float1COMANDOs, float2COMANDOs, str1COMANDOs, str2COMANDOs, intCOMANDOs
char COMANDOs[nCOMANDOsMax];
unsigned long long1COMANDOs[nCOMANDOsMax];
unsigned long long2COMANDOs[nCOMANDOsMax];
unsigned long long3COMANDOs[nCOMANDOsMax];
float float1COMANDOs[nCOMANDOsMax];
float float2COMANDOs[nCOMANDOsMax];
char str1COMANDOs[nCOMANDOsMax];
char str2COMANDOs[nCOMANDOsMax];
int intCOMANDOs[nCOMANDOsMax];


bool ESCREVE = false;
bool DEBUG = false;
bool COMENTA = true;
bool AVISATERMINOU = true;
bool REVERSO = false; // para indicar se inverte a defasagem
// int PASSO = -1; // para definir o passo padrão:-1="$", -2="!"
float DtZeroPadrao = 0.02;
unsigned long DtZero_uL = 20;

const byte nControles = 2;
byte pinControle[] = {A0, A1};
bool valorControle[] = {LOW, LOW};
bool modoControle[] = {INPUT, INPUT};
bool primeiroUsoControle[] = {true, true};
//const byte nControles = sizeof(pinControle)/sizeof(pinControle[0]);


String inStr = "";
bool strOk = false;
char endChar = 13;
String endStr = String(endChar);

const byte nMemos = 9;
String memos[] = {"'b';A0H;w10;A0L;", 
                  "'q';w300;p0;w700;p1", 
                  "M1 M2",
                  "_ESCREVE>flip", 
                  "_DEBUG>flip", 
                  "'up';u20", 
                  "'down';d20",
                  "'o10ms';w500;o10", 
                  "'q';w300;p0;w700;p1" };
//const byte nMemos = sizeof(memos)/sizeof(memos[0]);

const byte nBotoes = 4;
byte pinBotoes[] = {A2, A3, A4, A5};
byte estadoBotoes[]={0, 0, 0, 0};
String botoes[] = {"'up';u20",
                   "'down';d20",
                   "'o10ms';w500;o10", 
                   "'q';w300;p0;w700;p1" };
//const byte nBotoes = sizeof(pinBotoes)/sizeof(pinBotoes[0]);


// ------------- SETUP ---------------- SETUP -------------------
void setup() {

  pinMode( pinEEPROM, INPUT_PULLUP );
  iniciaEEPROM(); // necessario para alterar valores padroes

  Serial.begin(baudRate); // nao usar 230400- usar 250000 ou 115200
  Serial.setTimeout( timeOUT );
  Serial.println( F("Levitacao 2 Timers") );
  Serial.println( VERSAO );
  Serial.print( F("TIPOTimer:") );
  Serial.println( TIPOTimer );
  Serial.println( F("h->help; ?->status") );

  
  configuraTimer();
  pinMode( pinEnA, OUTPUT );
  pinMode( pinEnB, OUTPUT );
  mudaPot( potAtual );
  for (int qControle=0; qControle<nControles; qControle++){
    pinMode( pinControle[qControle], modoControle[qControle] );
    digitalWrite( pinControle[qControle], valorControle[qControle] );
  }
  mostraImax();
  //mostraFase();
  //mostraPot();
  //mostraMemos();
  //mostraBotoes();

  inStr.reserve(150);
  apagaCOMANDOs();
  for (byte b=0; b<nBotoes; b++){
    pinMode( pinBotoes[b], INPUT_PULLUP );
  }
}
// ---------- end of SETUP ---------------- end of SETUP ---------------
// ---------- end of SETUP ---------------- end of SETUP ---------------
// ---------- end of SETUP ---------------- end of SETUP ---------------




// ------------- LOOP ---------------- LOOP ------------------------
// ------------- LOOP ---------------- LOOP ------------------------
// ------------- LOOP ---------------- LOOP ------------------------
void loop() {
  if (COMANDOsAtual==-1){
    if ((inStr.length()==0)&&(nCOMANDOs==0)){
      for (byte b=0; b<nBotoes; b++){
        if (digitalRead(pinBotoes[b])==LOW){
          if (estadoBotoes[b]==0){
            inStr += botoes[b];
            inStr += endChar;
            if (DEBUG){
              Serial.println( inStr );
              Serial.println( nCOMANDOs );
              Serial.println( COMANDOsAtual );
            }
            strOk = true;
            estadoBotoes[b] = 255;
            break;
          }else{
            estadoBotoes[b] = 255;
          }
        }else{
          if (estadoBotoes[b]>0){
            estadoBotoes[b] -= 1;
          }
        }
      }
    }    
    while (Serial.available()>0){
      addChar();
    }
    if ( (inStr.length()>0) && (inStr.endsWith(endStr)) ){
      strOk = true;
    }else{
      strOk = false;
    }
  } else {
    if( COMANDOsAtual>=0 ){
      if (erroNoComando == false){
        executaComandos();
      }else{
        apagaCOMANDOs();
        erroNoComando = false;
      }
    }
  }
  if (dtNEXT){
    if ((micros()-tLAST)>dtNEXT){
      tLAST = 0;
      dtNEXT = 0;
    }
  }
  if ( strOk ){
    InStr2Comandos();
  }else if ((COMANDOsAtual==-1)&&(nCOMANDOs>0)){
    COMANDOsAtual = 0;
    if (ESCREVE){
      for ( int i=0; i<nCOMANDOs; i++ ){
        Serial.print( COMANDOs[i] );
      }
      Serial.println("");
    }
    if (dtNEXT==0){
      tLAST = micros();
    }
  }

}
// ---------- end of LOOP ---------------- end of LOOP ---------------
// ---------- end of LOOP ---------------- end of LOOP ---------------
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



bool ehControleValido(int n){
  return ((n>=0)&&(n<nControles));
}

bool ehMemoValido(int n){
  return ((n>=1)&&(n<=nMemos));
}

bool ehBotaoValido(int n){
  return ((n>=1)&&(n<=nBotoes));
}

