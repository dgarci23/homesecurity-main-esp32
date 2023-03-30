#include <api.h> 

//Your Domain name with URL path or IP address with path
String serverName = "https://aapqa4qfkg.execute-api.us-east-1.amazonaws.com/dev";

void configureWiFi(const char* ssid) {
    WiFi.begin(ssid);
    WiFi.setSleep(WIFI_PS_NONE);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

String connectApi(String path, const char* method, String sensorId) {
    String payload;
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + path;
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      http.addHeader("sensorId", sensorId);
      // Send HTTP GET request
      int httpResponseCode = http.sendRequest(method,"");
      //int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        payload = http.getString();
        //Serial.println(payload);
      }
      else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
        payload = "Error";
      }
      // Free resources
      http.end();
    }
    else {
      //Serial.println("WiFi Disconnected");
        payload = "WiFi Disconnected";
    }
    return payload;
}

String getUser(String userId){
    return connectApi("/user/"+userId, "GET");
}

String getSensors(String userId){
    return connectApi("/sensor/"+userId, "GET");
}

void triggerSensor(String userId, String sensorId){
    connectApi("/hub/sensor/"+userId, "PUT", sensorId);
}

String batterySensor(String userId, String sensorId, String batteryStatus){
  String payload;
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;

    String serverPath = serverName + "/battery/" + userId;
    http.begin(serverPath.c_str());
    //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
    
    http.addHeader("sensorId", sensorId);
    http.addHeader("battery", batteryStatus);
    // Send HTTP GET request
    int httpResponseCode = http.sendRequest("PUT","");
    
    if (httpResponseCode>0) {
      payload = http.getString();
    }
    else {
      payload = "Error";
    }
    // Free resources
    http.end();
  }
  else {
      payload = "WiFi Disconnected";
  }
  return payload;
}