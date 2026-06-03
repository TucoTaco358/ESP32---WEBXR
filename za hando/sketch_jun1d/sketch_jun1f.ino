//================== functionsWiFi ==================
void connectToWifi() {
  // החלף כאן לשם וסיסמת הרשת הציבורית/הביתית שלך
  //const char* home_ssid = "PET";
  //const char* home_password = "048297100";
  const char* home_ssid = "AndroidAPA861";
  const char* home_password = "iszn2501";

  Serial.print("Connecting to Home WiFi: ");
  Serial.println(home_ssid);

  // התחברות לראוטר הביתי
  WiFi.begin(home_ssid, home_password);

  // לולאת המקבילה שממתינה עד שהחיבור יושלם בהצלחה
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n--- WiFi Connected Successfully! ---");
}
//=================================================