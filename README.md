# QR_decode_for_Arduino
QR decode for Arduino Library including Spresense examples. This library uses "quirc" made by Daniel Beer. <br/>
For the detail about quirc, please see the github repository below.

https://github.com/dlbeer/quirc

And I also refer the alvarowolfx's repository to improve the performance.

https://github.com/alvarowolfx/ESP32QRCodeReader

Here's the example sketch for Spresense Arduino using the camera. 

```cpp
#include <Camera.h>
#include "quirc.h"

struct quirc* qr;
struct quirc_code code;
struct quirc_data data;
uint8_t *image;
int w, h;
  
void CamCB(CamImage img) {
  if (!img.isAvailable()) return;
   
  image = quirc_begin(qr, &w, &h);  
  uint16_t* rgb_image = (uint16_t*)img.getImgBuff();
  for (int n = 0; n < w*h; ++n) {
    uint16_t pix = rgb_image[n];
    image[n] = (pix & 0x7E0) >> 5; // extract g image
  }
  quirc_end(qr);
  
  int num_codes = quirc_count(qr);
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
```
