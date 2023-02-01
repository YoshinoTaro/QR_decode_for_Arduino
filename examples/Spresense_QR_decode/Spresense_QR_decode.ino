/**
 * QR decode example using quirc library for Spresense
 * LGPL version 2.1 Copyright 2023 Yoshino Taro
 */
#include <Camera.h>
#include "quirc.h"
#define USE_LCD

#ifdef USE_LCD
#include "Adafruit_ILI9341.h"
#define TFT_DC 9
#define TFT_CS -1
#define TFT_RST 8
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);
#endif

struct quirc* qr;
struct quirc_code code;
struct quirc_data data;
uint8_t *image;
int w, h;
  
void CamCB(CamImage img) {
  if (!img.isAvailable()) return;

#ifdef USE_LCD
  tft.drawRGBBitmap(0, 0, (uint16_t *)img.getImgBuff(), 320, 240); 
#endif
   
  image = quirc_begin(qr, &w, &h);  
  if (w != CAM_IMGSIZE_QVGA_H || h != CAM_IMGSIZE_QVGA_V) {
    Serial.println("configration error");
    while(1); // fatal error to enter the infinite loop (stop process)
  }
  // copy gray scale image
  uint16_t* rgb_image = (uint16_t*)img.getImgBuff();
  for (int n = 0; n < w*h; ++n) {
    uint16_t pix = rgb_image[n];
    image[n] = (pix & 0x7E0) >> 5; // extract g image
  }
  quirc_end(qr);
  
  int num_codes;
  int i;
  
  num_codes = quirc_count(qr);
  Serial.println("num codes: " + String(num_codes));
  for (int i = 0; i < num_codes; i++) {
    quirc_decode_error_t err;
    quirc_extract(qr, i, &code);
    err = quirc_decode(&code, &data);
    if (err)
      printf("DECODE FAILED: %s\n", quirc_strerror(err));
    else
      printf("Data: %s\n", data.payload);
  }
}

void setup() {
  Serial.begin(115200);
  
  String ver  = quirc_version();
  Serial.println("quirc version: " + ver);

#ifdef USE_LCD
  tft.begin();
  tft.setRotation(3);
#endif

  theCamera.begin(1, CAM_VIDEO_FPS_15, 
    CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V, CAM_IMAGE_PIX_FMT_RGB565);
  theCamera.startStreaming(true, CamCB);
  
  qr = quirc_new();
  if (qr == NULL) {
    Serial.println("can't create quirc object");
    return;
  }  
  if (quirc_resize(qr, 320, 240) < 0) {
    Serial.println("Failed to allocate video memory");
    return;
  }
}

void loop() { }
