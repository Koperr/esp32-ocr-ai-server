#include <WiFi.h>
#include <esp_heap_caps.h>
#include <ESPAsyncWebServer.h>

#include "camera.h"
#include "oled_display.h"



// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid     = "****";
const char* password = "****";

AsyncWebServer server(80); // Tworzenie serwera na porcie 80

// Funkcja, która obsługuje obraz kamery

void startCameraServer() {
  server.on("/jpg", HTTP_GET, [](AsyncWebServerRequest *request){

    // Ta petla for jest po to, zeby przed GET zrobic kilka zdjec testowych zeby kamera sie ustabilizowala itp
    for (int i = 0; i < 5; i++){
      camera_fb_t *fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        request->send(500, "text/plain", "Camera error");
        return;
    }
    // Ustaw nagłówek Content-Length
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/jpeg", fb->buf, fb->len);
    response->addHeader("Content-Length", String(fb->len));
    request->send(response);
    esp_camera_fb_return(fb);
});

  server.begin();
}

void setup() {
  Serial.begin(115200); 
  Serial.setDebugOutput(true);
  Serial.println();


  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!oled_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADRESS)) {
    Serial.println(F("OLED not found!"));
  }
  setup_oled(1, SSD1306_WHITE, 0, 0);


  // BUTTON
  pinMode(3, INPUT_PULLUP);
  


  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA; // SVGA
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // FRAMESIZE_QVGA 320x240
  // FRAMESIZE_VGA 640x480
  // FRAMESIZE_SVGA 800x600
  // FRAMESIZE_XGA 1024x768
  // FRAMESIZE_UXGA 1600x1200
  // FRAMESIZE_SXGA 1280x1024
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
    Serial.println("PSRAM Found: ");
    Serial.println(ESP.getFreePsram());
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 4;
    config.fb_count = 1;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  } else {
    // Limit the frame size when PSRAM is not available
    Serial.println("PSRAM NOT Found");
    config.frame_size = FRAMESIZE_HVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
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
  Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  delay(50);
  if(digitalRead(3) == LOW)
  {
    Serial.println("CWEL");
  }
}