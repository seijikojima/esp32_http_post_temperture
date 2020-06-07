#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// https://forum.arduino.cc/index.php?topic=624828.0
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include <DHT.h>
const int PIN_DHT = 4;
DHT dht(PIN_DHT,DHT11);


const char* ssid = "";
const char* password = "";
WebServer server(80);
IPAddress ipadr;

const char* serverName = "";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // 5 second

void handleRoot() {
  String style = "<style>body { background-color : white;}";
  style += "button { font-size : 2.0em;}";
  style += "div {border: 3px solid #aaa; margin: 100px 10% 0% 10%; padding: 3% 20% 3% 30%; }</style>";

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  String text = "<div>";
  text += "<h3>" + String(h) + " %" + "</h3>";
  text += "<h3>" + String(t) + " degree" + "</h3>";
  text += "</div>";
  text = style + text;
  server.send(200, "text/HTML", text);
}

void setup() {

    // wifi接続
    Serial.begin(115200);  // シリアルモニタを115200bpsで初期化
    WiFi.mode(WIFI_STA);  // Wi-Fiのモードを設定
    WiFi.begin(ssid, password);  // 接続するSSID　キーを指定してWi-Fiを開始
    Serial.println("");         // シリアルプリントの出力改行のみ
    while (WiFi.status() != WL_CONNECTED) {  // 接続が完了するまで待つ
      delay(500);  // 各回500msの時間待つ
      Serial.print(".");// 待ち時間500msごとに１ドット表示　接続が完了すると抜ける
    }

    // wifi設定表示
    Serial.println("");
    Serial.print("Connected to ");  // Wi-Fi接続を表示
    Serial.println(ssid);  // SSID表示
    Serial.print("IP address: ");
    ipadr = WiFi.localIP();
    Serial.println(ipadr);  // シリアルモニタにIPアドレスを表示

    // endpoint
    server.on("/", handleRoot);  // IPアドレスのみのアクセス時はhandleRoot()を起動
        
    server.begin();        // サーバを開始
    Serial.println("HTTP server started");

    dht.begin();
}

void print_humidity_and_temperature(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    return;
  } else {
    Serial.print("Humidity:");
    Serial.print(h);
    Serial.print("%\t");
    Serial.print("Temperature:");
    Serial.print(t);
    Serial.println("*C");
  }  
}

void loop(void) {

  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {

    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    String jsonData = "{";
    jsonData += "\"temperature\":";
    jsonData += String(t);
    jsonData += ",\"humidity\":";
    jsonData += String(h);
    jsonData += "}"; 
    String httpRequestData = "value1=" + String(72);
    Serial.print("httpRequestData: ");
    Serial.println(jsonData);

    http.addHeader("Content-Type", "application/json" , "Content-Length", jsonData.length());
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(jsonData);

    lastTime = millis();
  }
  
  server.handleClient();
}
