#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>

// משתני הרשת שנמצאים בשימוש (מבוסס על קובץ ה-WiFi שלך)
extern const char* home_ssid;
extern const char* home_password;
extern WebServer server;
extern WebSocketsServer webSocket;

// ==========================================
// פונקציית התחברות ל-WiFi
// ==========================================
void connectToWifi() {
  Serial.print("Connecting to Home WiFi: ");
  Serial.println(home_ssid);

  WiFi.begin(home_ssid, home_password);

  // לולאת המקבילה שממתינה עד שהחיבור יושלם בהצלחה
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n--- WiFi Connected Successfully! ---");
  Serial.print("ESP32 IP Address (Type this in your Quest 3): http://");
  Serial.println(WiFi.localIP()); // הדפסת ה-IP המקומי עבור המשקפיים
}

// ==========================================
// פונקציית עזר לשרת ה-HTTP (הגשת קבצים מ-LittleFS)
// ==========================================
bool handleFileRead(String path) {
  Serial.println("HTTP Request for: " + path);
  
  // אם גולשים לנתיב הראשי, נגיש את קובץ ה-index.html
  if (path.endsWith("/")) path += "index.html";
  
  String contentType = "text/html";
  if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".css")) contentType = "text/css";
  
  // בדיקה אם הקובץ קיים פיזית בזיכרון ה-LittleFS של הצ'יפ
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  
  Serial.println("File not found in LittleFS: " + path);
  return false; 
}
