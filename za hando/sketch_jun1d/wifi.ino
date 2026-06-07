#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>

// ==========================================
// הגדרות רשת וחומרה 
// ==========================================
//const char* home_ssid = "HOTWiFi-154F";     // שם הרשת מהקובץ שלך
//const char* home_password = "14X1Q89TB5GJ";     // סיסמת הרשת מהקובץ שלך
const char* home_ssid = "AndroidAPA861";    
const char* home_password = "iszn2501"; 
#define LED_PIN 2 // הגדרת הלד המובנה של הכרטיס לאינדיקציה

// הגדרת שרתי הרשת
WebServer server(80);          // שרת ה-HTTP שיגיש את ה-HTML וה-JS (פורט 80)
WebSocketsServer webSocket(81); // שרת ה-WebSocket שיקשיב לפיצוצים מהמשקפיים (פורט 81)

// הצהרה על פונקציות שנמצאות בכרטיסיית ה-WiFi כדי שה-main יכיר אותן
void connectToWifi();
bool handleFileRead(String path);

// ==========================================
// פונקציית הטיפול באירועי WebSocket (קבלת הפיצוץ מהמשקפיים)
// ==========================================
// פונקציית ה-Callback המתוקנת עם אותיות קטנות/גדולות נכונות (WStype_t)
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    
    // שינוי ל-WStype עם t קטנה
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Quest 3 Disconnected! 🔴\n", num);
      break;
      
    // שינוי ל-WStype עם t קטנה
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Quest 3 Connected from %d.%d.%d.%d 🟢\n", num, ip[0], ip[1], ip[2], ip[3]);
      break;
    }
      
    // שינוי ל-WStype עם t קטנה
    case WStype_TEXT: {
      // המרה של המידע שהגיע לטקסט (String)
      String message = String((char*)(payload)).substring(0, length);
      
      // בדיקה אם הגיע האות המדויק של הפיצוץ המוגדר ב-index.html
      if (message == "BALLOON_POPPED") {
        Serial.println("Boom! Balloon popped by user hands 🎈💥");
        
        // הדלקת הלד המובנה לשנייה כאינדיקציה חומרתית
        digitalWrite(LED_PIN, HIGH);
        delay(200); 
        digitalWrite(LED_PIN, LOW);
      }
      break;
    }
  }
}

// ==========================================
// Setup הראשי
// ==========================================
void setup() {
  // הפעלת התקשורת הטורית בקצב הקבוע שלך
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 1. אתחול מערכת הקבצים הפנימית LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed! ❌ Check your upload.");
    return;
  }
  Serial.println("LittleFS Mounted Successfully. 📂");

  // 2. הפעלת חיבור ה-WiFi (קורא לפונקציה מכרטיסיית ה-WiFi)
  connectToWifi();

  // 3. הגדרת שרת ה-HTTP להגשת קבצי ה-AR מתוך הזיכרון הפנימי
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  server.begin();
  Serial.println("HTTP Server Started.");

  // 4. אתחול שרת ה-WebSocket לפורט 81
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("WebSocket Server Started on port 81.");
}

// ==========================================
// Loop הראשי
// ==========================================
void loop() {
  // הרצה רציפה של שרתי הרשת בכל מחזור עבודה
  server.handleClient();
  webSocket.loop();
}
