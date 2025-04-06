#include <WiFi.h>
#include <esp_heap_caps.h>
#include <ESPAsyncWebServer.h>

#include "camera.h"
#include "oled_display.h"


// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid     = "speleo2";
const char* password = "***";

AsyncWebServer server(80); // Tworzenie serwera na porcie 80

// Funkcja, która obsługuje obraz kamery

// curl http://192.168.0.14/jpg --output obraz1.jpg

void startCameraServer() {
  server.on("/jpg", HTTP_GET, [](AsyncWebServerRequest *request){

    // Ta petla for jest po to, zeby przed GET zrobic kilka zdjec testowych zeby kamera sie ustabilizowala itp
    for (int i = 0; i < 5; i++){
      camera_fb_t *fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
    }
    camera_fb_t *fb = esp_camera_fb_get();
    delay(200);
    if (!fb) {
        request->send(500, "text/plain", "Camera error");
        return;
    }
    // Ustaw nagłówek Content-Length
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/jpeg", fb->buf, fb->len);
    response->addHeader("Content-Type", "image/jpeg");
    response->addHeader("Content-Length", String(fb->len));
    response->addHeader("Cache-Control", "no-store");
    response->addHeader("Connection", "close");
    request->send(response);
    esp_camera_fb_return(fb);
});

  server.begin();
}

void setup() {
  Serial.begin(115200); 
  Serial.setDebugOutput(true);
  Serial.println();


  // CAMERA
  camera_config_t camera_config;
  setup_camera(camera_config);

  if(psramFound()){
    Serial.println("PSRAM Found: ");
    Serial.println(ESP.getFreePsram());
  } else {
    // Limit the frame size when PSRAM is not available
    Serial.println("PSRAM NOT Found");
    camera_config.frame_size = FRAMESIZE_HVGA;
    camera_config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // OLED I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!oled_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADRESS)) {
    Serial.println(F("OLED not found!"));
  }
  setup_oled(1, SSD1306_WHITE, 0, 0);


  // BUTTON
  pinMode(3, INPUT_PULLUP);
  



  // camera init
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 1); // flip it back
  s->set_brightness(s, 2); // up the brightness just a bit
  s->set_saturation(s, -1); // lower the saturation
  s->set_contrast(s, 2);
  s->set_hmirror(s, 1);
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(ssid);

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

}

void loop() {
  //Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  //delay(50);
  if(digitalRead(3) == LOW)
  {
    Serial.println("CWEL");
  }
}