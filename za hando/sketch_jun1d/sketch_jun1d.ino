#include <WiFi.h>
#include <WebSocketsServer.h> // ספריה לתקשורת מהירה מול האתר בגיטהאב

// אובייקט שרת ה-WebSockets שיקשיב בפורט 81 הסטנדרטי
WebSocketsServer webSocket = WebSocketsServer(81); 

// משתנה גלובלי לספירת הבלונים שהתפוצצו
int popCounter = 0;

// הצהרה על פונקציית ה-WiFi שנמצאת בלשונית השנייה שלך (functionsWiFi.ino)
void connectToWifi();

// פונקציית הטיפול באירועי ה-WebSocket (חיבורים, ניתוקים והודעות נכנסות)
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Quest 3 Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Quest 3 Connected from IP: %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      webSocket.sendTXT(num, "Hello from ESP32!");
      break;
    }
    
    case WStype_TEXT:
      // הפיכת ה-payload (מערך הבייטים שנתקבל) למחרוזת טקסט של C++
      String message = String((char*)(payload));
      
      // בדיקה האם המשקפיים דיווחו על פיצוץ בלון
      if (message == "BALLOON_POPPED") {
        popCounter++; // קידום המונה ב-1
        Serial.println("========================================");
        Serial.printf("🎈 BOOM! Balloon Popped! Total Score: %d\n", popCounter);
        Serial.println("========================================");
      }
      break;
  }
}

void setup() {
  // אתחול ערוץ התקשורת הטורית בקצב של 115200 baud
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- Starting ESP32 Score Counter System ---");

  // הפעלת ה-Wi-Fi והתחברות לנקודה החמה של הטלפון או הראוטר [cite: 4]
  connectToWifi(); 

  // אתחול שרת ה-WebSockets וקישורו לפונקציית הניהול
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Server Started on Port 81.");

  // הדפסת מדריך ברור למסך הסיריאל [cite: 5]
  Serial.println("\n========================================"); 
  Serial.print("ESP32 Local IP Address: "); 
  Serial.println(WiFi.localIP());  // הדפסת ה-IP שקיבלנו מהראוטר [cite: 7]
  Serial.println("========================================\n"); 
}

void loop() {
  // החזקת שרת ה-WebSockets פעיל ומאזין ברקע
  webSocket.loop();
  delay(1); // השהייה מינימלית ליציבות המעבד [cite: 9]
}