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
const char* symbol = "MLNBUSD"; // The trading symbol to use
const double margin = 0.1;  // The margin to use for placing orders
const double quantity_sell = 0.001; // The quantity of the asset to sell
const char* side_sell = "SELL";  // The side of the sell order to place
const double quantity_buy = 0.001; // The quantity of the asset to buy
const char* side_buy = "BUY";  // The side of the buy order to place
const char* type = "MARKET";  // The type of order to place
const char* timeInForce = "GTC";  // The time in force for the order
const unsigned long recvWindow = 5000;  // The receive window for the order

// Trading state variables
double last_price = 0.0;
unsigned long lastUpdateTime = 0;

// WiFi network credentials
const char* ssid = "xx";
const char* password = "xxxx";

// Get the current price for the specified symbol from Binance
float get_current_price(const char* symbol) {
  String url = String(binance_endpoint_price) + String(symbol);
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  double price = 0.0;
  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument json_document(1024);
    deserializeJson(json_document, payload);
    price = json_document["price"];
  }
  http.end();
  return price;
}

float getBinancePrice(String symbol) {
  HTTPClient http;
  http.begin("https://api.binance.com/api/v3/ticker/price?symbol=" + symbol);
  int httpCode = http.GET();
  float price = 0.0;
  if (httpCode == HTTP_CODE_OK) {
    DynamicJsonDocument json_document(1024);
    DeserializationError error = deserializeJson(json_document, http.getString());
    if (!error) {
      price = json_document["price"].as<float>();
    }
  }
  http.end();
  return price;
}


void setup() {
  Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

  display.init();
  display.clear();
  

   // draw some text
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Fetching MLN!");
  display.display();

  // Initialize the serial port for debugging
  //Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateTime >= 1000) {
    lastUpdateTime = currentMillis;
    double price = getBinancePrice(symbol);
    Serial.print("Price: ");
    Serial.println(price);
    prices[currentIndex] = price;
    currentIndex = (currentIndex + 1) % numSamples;
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, String("      MLNBUSD="));
    display.setFont(ArialMT_Plain_16);
    display.drawString(88, 0, String(price, 2));
    // Draw the price curve
    for (int i = 0; i < numSamples - 1; i++) {
      int x1 = map(i, 0, numSamples - 1, 0, display.getWidth())-1;
      int x2 = map(i + 1, 0, numSamples - 1, 0, display.getWidth()-1);
      int y1 = map(prices[(currentIndex + i) % numSamples], 0, 50, display.getHeight()-1, 0);
      int y2 = map(prices[(currentIndex + i + 1) % numSamples], 0, 50, display.getHeight()-1, 0);
      display.drawLine(x1, y1, x2, y2);
    }
    
    // Draw the x and y axes
    display.setFont(ArialMT_Plain_10);
    display.drawLine(0, display.getHeight()-5, display.getWidth(), display.getHeight()-5); // x axis
    display.drawLine(5, 0, 5, display.getHeight()); // y axis

    //draw axes and scale info
    // Pfeil y axis
    display.drawLine(2, 8, 5, 0);
    display.drawLine(8, 8, 5, 0);
    // Pfeil x axis
    display.drawLine(0, display.getHeight()-5, +8, display.getHeight()-8);
    display.drawLine(0, display.getHeight()-5, +8, display.getHeight()-2);
    
    // Skalar x axis
    display.drawLine(35, display.getHeight()-8, 35, display.getHeight()-2);
    display.drawLine(65, display.getHeight()-8, 65, display.getHeight()-2);
    display.drawLine(95, display.getHeight()-8, 95, display.getHeight()-2);
    display.drawLine(125, display.getHeight()-8, 125, display.getHeight()-2);
    display.drawString(28,display.getHeight()-20, String("1,5m"));
    display.drawString(60,display.getHeight()-20, String("1m"));
    display.drawString(90,display.getHeight()-20, String("30s"));
    display.drawString(123,display.getHeight()-20, String("0"));
    

    // Skalar y axis
    display.drawLine(2, display.getHeight()-15, 8, display.getHeight()-15);
    display.drawLine(2, display.getHeight()-25, 8, display.getHeight()-25);
    display.drawLine(2, display.getHeight()-35, 8, display.getHeight()-35);
    display.drawLine(2, display.getHeight()-45, 8, display.getHeight()-45);
    
    display.drawString(10,display.getHeight()-21, String("10"));
    display.drawString(10,display.getHeight()-31, String("20"));
    display.drawString(10,display.getHeight()-41, String("30"));
    display.drawString(10,display.getHeight()-51, String("40"));
    display.display();
  }
 
  // Wait for 1 second before fetching the next price
 // delay(1000);
}

