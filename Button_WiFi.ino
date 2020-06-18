#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

unsigned int setTime = 20000;
unsigned long int lastTime = 0;
const char* host = "api.seniverse.com"; // 网络服务器地址
const int httpPort = 80;              // http端口80

// 设置wifi接入信息(请根据您的WiFi信息进行修改)
const char* ssid = "ESP12E";
const char* password = "12345678";

String  key = "Snb_abxVo_6Mgl369"; //心知密钥
String location = "hangzhou";   //你所在的城市

void setup() {
  //初始化串口设置
  Serial.begin(9600);
  Serial.println("");

  //设置ESP8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);

  //开始连接wifi
  WiFi.begin(ssid, password);

  //等待WiFi连接,连接成功打印IP
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");

  wifiClientRequest();
}

void loop() 
{
 unsigned long int nowTime = millis();
 
 if(nowTime-lastTime>setTime) //限制每20秒访问一次
 {
    lastTime = nowTime;  
    wifiClientRequest();
 }
 if(nowTime-lastTime<0)
    lastTime = nowTime;
}


void wifiClientRequest() // 向服务器发送HTTP请求
{
  
  WiFiClient client; // 建立WiFi客户端对象，对象名称client
  String url = "/v3/weather/now.json?key=" + key + "&location=" + location + "&language=zh-Hans&unit=c";
  
  // 建立字符串，用于HTTP请求
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";

  // 通过串口输出连接服务器名称以便查阅连接服务器的网址
  Serial.print("Connecting to ");
  Serial.print(host);

  // 连接网络服务器
  if (client.connect(host, httpPort)) 
  {
    Serial.println(" Success!");        // 连接成功后串口输出“Success”信息

    client.print(httpRequest);          // 向服务器发送HTTP请求
    Serial.println("Sending request: ");// 通过串口输出HTTP请求信息内容以便查阅
    Serial.println(httpRequest);

    // 通过串口输出网络服务器响应信息， 以下段落中的示例程序为本程序重点2
  
    Serial.println("Web Server Response:");
    Serial.println();
    Serial.println();
    while (client.connected() || client.available()) 
    {
      if (client.available()) {

        if (client.find("\r\n\r\n"))
        {
          String line = client.readStringUntil('\n');
          Serial.println(line);
          jsonParser(line);
        }
      }
    }

    client.stop();                      // 断开与服务器的连接
    Serial.print("Disconnected from "); // 并且通过串口输出断开连接信息
    Serial.print(host);

  } else {   // 如果连接不成功则通过串口输出“连接失败”信息
    Serial.println(" connection failed!");
    client.stop();
  }
}

void jsonParser(String line) //解析JSON数据
{
  //使用ArduinoJSON官网生成的代码，很方便
  //ArduinoJson地址
  //https://arduinojson.org/
  
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 210;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, line);
  DeserializationError error = deserializeJson(doc, line);
  if (error) {                                         
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  JsonObject results_0 = doc["results"][0];

  JsonObject results_0_location = results_0["location"];
  const char* results_0_location_id = results_0_location["id"];
  const char* results_0_location_name = results_0_location["name"];
  const char* results_0_location_country = results_0_location["country"];
  const char* results_0_location_path = results_0_location["path"];
  const char* results_0_location_timezone = results_0_location["timezone"];
  const char* results_0_location_timezone_offset = results_0_location["timezone_offset"];

  JsonObject results_0_now = results_0["now"];
  const char* results_0_now_text = results_0_now["text"]; 
  const char* results_0_now_code = results_0_now["code"]; 
  const char* results_0_now_temperature = results_0_now["temperature"]; 

  const char* results_0_last_update = results_0["last_update"]; 
  Serial.print(results_0_location_name);
  Serial.print(": ");
  Serial.println(results_0_now_text);
  Serial.print("温度: ");
  Serial.println(results_0_now_temperature);

}
