/* Class ardcli
 * A simple and easy to use console command line interface for the Arduino serial connection, loosly inspired by Cisco IOS.
 * The begin() function will call your own Exec after pressing <enter>, or Help function when pressing <tab> or <?>. These
 * functions require a char[] paramater to be able to pass the active command. 
 * To exit the interface, call the exit() function from one of these functions.  
 *
 * Usage:
 * ardcli ardCli;
 * void functionExec(char data[]) { };
 * void functionHelp(char data[]) { };
 * ardCli.begin(functionExec, functionHelp);
 *
 * See the demo below for a fully working example
*/
class ardcli {  
  public:   
    ardcli();    
    void begin(void(*onExecPtr)(char data[]), void(*onHelpPtr)(char data[]));
    void exit();
  private:    
    bool _exit;
    void (*onExec)(char data[]);
    void (*onHelp)(char data[]);    
};
ardcli::ardcli() {
  _exit = false;
}
void ardcli::exit() {
  _exit = true;
}
void ardcli::begin(void(*onExecPtr)(char data[]), void(*onHelpPtr)(char data[])) {  
  this->onExec = onExecPtr;
  this->onHelp = onHelpPtr;
  char buff[20] = "";
  byte blen = 0;  
  byte incomingByte = 0;   
  Serial.print("\r\n> ");   
  while (!_exit) {
    if (Serial.available() > 0) {
      incomingByte = Serial.read();
      if ((incomingByte == 10) || (incomingByte == 13)) {           // <enter>      
        Serial.println("");        
        onExec(buff);         
        Serial.print("> ");
        for (int i=0; i<20; i++)
          buff[i] = 0;
        blen = 0;
      } else if ((incomingByte == 9) || (incomingByte == 63)) {     // <tab>, <?>         
        Serial.println("");        
        onHelp(buff);         
        Serial.print("> ");
        Serial.print(buff);         
      } else if (incomingByte == 127) {                             // <backspace>                  
        if (blen > 0) {
          Serial.print((char) incomingByte);
          blen--;
          buff[blen] = 0;         
        }
      } else if ((incomingByte >=32) && (incomingByte <=159)) {     // <a> to <Z> (ASCII)        
        Serial.print((char) incomingByte);
        buff[blen] = (char) incomingByte;
        blen++;        
      }
    }      
  }
  Serial.println("\r\n % terminated.");
}

/*
 * -= Full Demo =-
 */

ardcli ardCli;

char** strSplit(char str[], char delimiter) {  
  char* prev = str-1;
  char* pcur = strchr(str,delimiter);
  char* pend = strchr(str,NULL);
  char** result = new char*[10];
  byte i = 0;
  while (pcur!=NULL)
  {    
    char dest[10] = "";
    strncpy(dest, prev+1,pcur-prev-1);    
    prev = pcur;
    pcur = strchr(pcur+1,delimiter);
    result[i] = new char[10];
    strcpy(result[i],dest);    
    i++;
  }  
  char dest[10] = "";
  strncpy(dest, prev+1,pend-prev-1);
  result[i] = new char[10];
  strcpy(result[i],dest);
  for (byte j=i+1; j<10; j++) {
    result[j] = new char[10];
    strcpy(result[j],"");
  }  
  return result;
}

void acExec(char data[]) { 
  char** tdata = strSplit(data, " :");  
  if (strcmp(tdata[0], "help") == 0) {
    acHelp("help");
  } else if (strcmp(tdata[0], "test") == 0) {
    for (int i = 0; i < 5; i++) {    
      Serial.print(" ");
      Serial.print(tdata[1]); 
    }
    Serial.println();
  } else if (strcmp(tdata[0], "exit") == 0) {
    ardCli.exit();
  } else if (strcmp(tdata[0], "") == 0) {
    acHelp("help");
  } else {
    Serial.println(" % unknown command");
  }
}

void acHelp(char data[]) {  
  char** tdata = strSplit(data, " :");  
  if (strcmp(tdata[0], "help") == 0) {
    Serial.println("Exec commands:");
    Serial.println("  help    Command overview");
    Serial.println("  test    Print word");    
    Serial.println("  exit    Stop terminal access"); 
    Serial.println();
  } else if (strcmp(tdata[0], "test") == 0) {          
    Serial.println("  test [word]     Print [word] 5 times"); 
  } else if (strcmp(tdata[0], "") == 0) {
    acHelp("help");
  } else {
    Serial.println(" % unknown command");
  }  
}

void setup() {  
  Serial.begin(9600);
  Serial.println("\r\nARD Serial CLI Demo v0.02");
  ardCli.begin(acExec, acHelp);  
}

void loop() { }
