#include <Wii.h>
#include <usbhub.h>

#define  R_INR  3
#define  R_IN1  1
#define  R_IN2  2
#define  L_INR  6
#define  L_IN1  4
#define  L_IN2  5

USB Usb;
USBHub Hub1(&Usb);
BTD Btd(&Usb);
WII Wii(&Btd,PAIR);

bool printAngle;

bool isRunning;

void setup() {
  Serial.begin(115200);
  
  // Wii リモコンとの接続待ち
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while(1); //halt
  }
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
  
  // Arduino UNO の出力ピンを設定
  pinMode(R_INR, OUTPUT);
  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
  pinMode(L_INR, OUTPUT);
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  isRunning = 0;
}

void loop() {
  Usb.Task();
  
  // Wii リモコンとの接続が切れたら終了
  if(!Wii.wiimoteConnected) { return; }
  if(Wii.getButtonClick(HOME)) {
    Serial.println("CLOSE");
    Wii.disconnect();
    return;
  }
  
  // Wii リモコン横持ちでの左右回転を 0～360 で取得
  int pitch = Wii.getPitch();
  int right = 0;
  int left = 0;
  if( 0 <= pitch && pitch <= 90 ){
    right = constrain( map(pitch, 0, 45, 0, 255), 0, 255 );
  }else if( -90 <= pitch && pitch <= 0 ){
    left = constrain( map(pitch, 0,-45, 0, 255), 0, 255 );
  }else if( 270 <= pitch ){
    left = constrain( map(pitch, 360, 315, 0, 255), 0, 255 );
  }
  
  if(Wii.getButtonPress(UP)){
    // Wii リモコンの上ボタンが押されたら両方のモーターを正転
    digitalWrite(R_IN1, HIGH);
    digitalWrite(R_IN2, LOW);
    digitalWrite(L_IN1, HIGH);
    digitalWrite(L_IN2, LOW);

    // Wii リモコンが右に傾いている時は右を減速
    // 左に傾いている時は左を減速
    analogWrite(R_INR,255 - right);
    analogWrite(L_INR,255 - left);
  }else if(Wii.getButtonPress(DOWN)){
    // Wii リモコンの下ボタンが押されたら両方のモーターを反転
    digitalWrite(R_IN1, LOW);
    digitalWrite(R_IN2, HIGH);
    digitalWrite(L_IN1, LOW);
    digitalWrite(L_IN2, HIGH);

    // Wii リモコンが右に傾いている時は右を減速
    // 左に傾いている時は左を減速
    analogWrite(R_INR,255 - right);
    analogWrite(L_INR,255 - left);
  }else if(Wii.getButtonPress(RIGHT)){
    // Wii リモコンの右ボタンが押されたら右モーターだけを反転
    digitalWrite(R_IN1, LOW);
    digitalWrite(R_IN2, HIGH);
    digitalWrite(L_IN1, HIGH);
    digitalWrite(L_IN2, LOW);
    analogWrite(R_INR,255);
    analogWrite(L_INR,255);
  }else if(Wii.getButtonPress(LEFT)){
    // Wii リモコンの左ボタンが押されたら左モーターだけを反転
    digitalWrite(R_IN1, HIGH);
    digitalWrite(R_IN2, LOW);
    digitalWrite(L_IN1, LOW);
    digitalWrite(L_IN2, HIGH);
    analogWrite(R_INR, 255);
    analogWrite(L_INR, 255);
  } else {
    // ボタンから手を放したら停止
    digitalWrite(R_IN1, LOW);
    digitalWrite(R_IN2, LOW);
    digitalWrite(L_IN1, LOW);
    digitalWrite(L_IN2, LOW);
    analogWrite(R_INR, 0);
    analogWrite(L_INR, 0);
  }
}
