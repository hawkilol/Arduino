/*
 * Programa desenvolvido para capturar os dados do sensor e enviar para SGBD tipo MySQL/MariaDB.
*/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

// Número identificador do sensor
int idSensor = 27;

// Variáveis novas para serem pedidas na configuração do Wi-FI
char serverBD[100] = "200.239.66.45";
char portBD[6] = "3306";
char loginBD[20] = "aluno";
char passwdBD[30] = "aula";

// Query para inserir dados no BD
char INSERT_SQL[] = "INSERT INTO topicos.dadosTU(usuario, idSensor, tempCelsius, umidade) VALUES (%s, %d, %.2f, %.2f)";
char query[255];

WiFiClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor *cursor_mysql;

void configWiFi(){
  WiFiManager wm;

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("Topicos"); // anonymous ap
  // res = wm.autoConnect("Topicos","1234"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");
  }
}

void setup() {
  Serial.begin(115200);

  // Configura o Wi-Fi
  configWiFi();

  // Desabilita o SW-WDT para ativar o HW-WDT
  ESP.wdtDisable();
}

void loop(){
  float h = random(100);
  float t = random(50);

  ESP.wdtFeed();
  
  sprintf( query, INSERT_SQL, "Dionne", idSensor, t, h);

  Serial.println("Query a ser enviada ao BD: ");
  Serial.println(query);

  sendQueryToBD( );
  
  ESP.wdtFeed();

  // Delay de 60 segundos com reset do HW-WDT (HardWare-Watch Dog Time)
  for( int i=0; i<12; i++ ) {
    delay(5000);
    ESP.wdtFeed();
  }
}

/*
 * Função que envia a o conteúdo da variável query para o BD.
 */
void sendQueryToBD( ) {
  IPAddress server;
  server.fromString(serverBD);

  int port = atoi(portBD);
  
  if( WiFi.status() == WL_CONNECTED ) {
    Serial.println("Iniciando conexão ao BD");
    delay(500);

    int i = 0;
    while (conn.connect(server, port, loginBD, passwdBD) != true) {
      delay(200);
      Serial.print ( "." );
      
      i++;
      if( i > 10 ) ESP.restart();
    }
  
    Serial.println("Connected to SQL Server!");  
    cursor_mysql = new MySQL_Cursor(&conn);
    if(!cursor_mysql->execute(query)) {
      Serial.println("Query enviada.\n");
    }
    else {
      Serial.println("Problemas ao enviar a query\n");
      ESP.restart();
    }
    delete cursor_mysql;
  }
  else{
    Serial.println("Conexão com o Wi-Fi perdida\n");
    ESP.restart();
  }
}
