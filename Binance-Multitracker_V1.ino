#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SSD1306Wire.h>


#define OLED_RESET 4
SSD1306Wire display(0x3c, 5, 4);

const int numSamples = 60;
double prices[numSamples];
int currentIndex = 0;

// Binance API credentials
const char* binance_api_key = "YOUR_API_KEY";
const char* binance_api_secret = "YOUR_API_SECRET";

// Binance API endpoints
const char* binance_endpoint_price = "https://api.binance.com/api/v3/ticker/price?symbol=";
const char* binance_endpoint_order = "https://api.binance.com/api/v3/order";

// Trading parameters
const char* symbol1 = "MLNBUSD"; // The trading symbol to use
const char* symbol2 = "BTCBUSD"; // The trading symbol to use
const char* symbol3 = "MATICBUSD"; // The trading symbol to use
const char* symbol4 = "BNBBUSD"; // The trading symbol to use
const char* symbol5 = "ADABUSD"; // The trading symbol to use

const double margin = 0.1;  // The margin to use for placing orders
const double quantity_sell = 0.001; // The quantity of the asset to sell
const char* side_sell = "SELL";  // The side of the sell order to place
const double quantity_buy = 0.001; // The quantity of the asset to buy
const char* side_buy = "BUY";  // The side of the buy order to place
const char* type = "MARKET";  // The type of order to place
const char* timeInForce = "GTC";  // The time in force for the order
const unsigned long recvWindow = 5000;  // The receive window for the order

unsigned long lastUpdateTime = 0;

// WiFi network credentials
#define WIFI_SSID_2 "xxxx"
#define WIFI_PASSWORD_2 "xxxxx"
#define WIFI_SSID_1 "xxxx"
#define WIFI_PASSWORD_1 "xxxxx"



float getBinancePrice(String symbol) {
  
  // ["BTCUSDT","BNBUSDT"]
  
  HTTPClient http;
  http.begin("https://api.binance.com/api/v3/ticker/price?symbol=" + symbol);
  
  int httpCode = http.GET();
  float price = 0.0;
  if (httpCode == HTTP_CODE_OK) {
    DynamicJsonDocument json_document(100);
    DeserializationError error = deserializeJson(json_document, http.getString());
    if (!error) {
      price = json_document["price"].as<float>();
    }
  }
  http.end() ;
  return price ;
}


void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID_1, WIFI_PASSWORD_1);
  // WiFi.setTimeout(10000);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID_1);
  Serial.print("...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECT_FAILED) {
      WiFi.begin(WIFI_SSID_2, WIFI_PASSWORD_2);
      Serial.println();
      Serial.print("Failed to connect to ");
      Serial.print(WIFI_SSID_1);
      Serial.print(". Trying ");
      Serial.print(WIFI_SSID_2);
      Serial.println("...");
    }
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());

  display.init();
  display.clear();
  

   // draw some text
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Fetching Crypto!");
  display.display();
  // Initialize the serial port for debugging
  
}

void loop() {
  
  // code to be executed repeatedly
  double pricemln = getBinancePrice(symbol1);
  double pricebtc = getBinancePrice(symbol2);
  double pricematic = getBinancePrice(symbol3);
  double pricebnb = getBinancePrice(symbol4);
  // double priceada = getBinancePrice(symbol5);

  
  Serial.print("MLN: ");
  Serial.println(pricemln);
  Serial.print("BTC: ");
  Serial.println(pricebtc);
  Serial.print("Matic: ");
  Serial.println(pricematic);
  Serial.print("BNB: ");
  Serial.println(pricebnb);
  Serial.print("ADA: ");
  Serial.println(priceada);
  display.clear();
  
  display.drawString(0, 0, String("MLN:  ")    + String(pricemln, 2)   +" $");
  display.drawString(0, 16, String("BTC:  ")   + String(pricebtc, 1)   +" $");
  display.drawString(0, 32, String("Matic: ") + String(pricematic, 4) +" $");
  display.drawString(0, 48, String("BNB:  ")   +  String(pricebnb, 1)  +" $");
  display.display();
  // display.display();  
  
      
}

