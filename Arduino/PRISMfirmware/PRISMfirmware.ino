#include <U8glib.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <swRTC.h>

//RTC
swRTC rtc;

//BIOS
bool BOOT_COMPLETE = false;
short BOOT_ANIMATION_STATUS = 0;
short BOOT_ANIMATION_MAX = 600;

//블루투스 연결
SoftwareSerial mySerial(2, 3); //블루투스의 Tx, Rx핀을 2번, 3번핀으로 설정
boolean BLUETOOTH_CONNECTED = false;

//Button
const short PIN_BTN_POSITIVE = 7;
const short PIN_BTN_NEGATIVE = 6;

//OLED import
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 

//메시지 변수
const int MESSAGE_SIZE = 20;
char mReceivedMessage[MESSAGE_SIZE]="\0";
short MESSAGE_TYPE = -1;
bool mIsReceivingBegin = false;

//메시지 타입
const int TEXT_TYPE_NULL = -1;
const int TEXT_TYPE_DATE = 0;
const int TEXT_TYPE_TIME = 1;
const int TEXT_TYPE_CALL = 2;
const int TEXT_TYPE_SMS = 3;
const int TEXT_TYPE_OTHER = 4;
const int TEXT_TYPE_DELETE = 5;
const int TEXT_TYPE_MESSAGE = 6;
const int TEXT_TYPE_VIDEO = 7;
const int TEXT_TYPE_PICTURE = 8;
const int TEXT_TYPE_DOWNLOAD = 9;
const int TEXT_TYPE_UPLOAD = 10;
const int TEXT_TYPE_CALENDAR = 11;

//시간
bool TIME_24 = true;
bool TIME_RECEIVED = false;
short TIME_HOUR = 2;
short TIME_MINUTE = 30;
short TIME_SECOND = 0;
short TIME_NEXT_CHECK_HOUR = 0;
short TIME_NEXT_CHECK_MINUTE = 0;//다음 연결 확인할 시간
short TIME_NEXT_CHECK_COUNT = 30;
short TIME_MILISECOND =0;

//날짜
bool DATE_RECEIVED = false;
short DATE_YEAR = 1970;
short DATE_MONTH = 1;
short DATE_DATE = 1;

//XBM Icon BEGIN
static unsigned char IconConnected_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x0a, 0x00,
   0x00, 0x0a, 0x00, 0x80, 0x0a, 0x00, 0x80, 0x0a, 0x00, 0xa0, 0x0a, 0x00 };

static unsigned char IconDisconnected_bits[] = {
   0x00, 0x00, 0x00, 0xa0, 0x08, 0x00, 0x40, 0x08, 0x00, 0xa0, 0x0a, 0x00,
   0x00, 0x0a, 0x00, 0x80, 0x0a, 0x00, 0x80, 0x0a, 0x00, 0xa0, 0x0a, 0x00 };

static unsigned char IconCall_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x20, 0x10, 0x00,
   0xa0, 0x17, 0x00, 0x60, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char IconOther_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00,
   0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00 };

static unsigned char IconSMS_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x20, 0x08, 0x00,
   0x60, 0x0c, 0x00, 0xa0, 0x0b, 0x00, 0x20, 0x08, 0x00, 0xe0, 0x0f, 0x00 };

static unsigned char IconVideo_bits[] = {
   0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x20, 0x08, 0x00, 0xa0, 0x09, 0x00,
   0xa0, 0x0a, 0x00, 0xa0, 0x09, 0x00, 0x20, 0x08, 0x00, 0xe0, 0x0f, 0x00 };

static unsigned char IconPicture_bits[] = {
   0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x20, 0x08, 0x00, 0x20, 0x09, 0x00,
   0xa0, 0x0a, 0x00, 0x60, 0x0a, 0x00, 0x20, 0x0c, 0x00, 0xe0, 0x0f, 0x00 };

static unsigned char IconChat_bits[] = {
   0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x20, 0x08, 0x00, 0x20, 0x08, 0x00,
   0x20, 0x08, 0x00, 0x20, 0x0f, 0x00, 0xa0, 0x00, 0x00, 0x60, 0x00, 0x00 };

static unsigned char IconDownload_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
   0x40, 0x05, 0x00, 0x80, 0x03, 0x00, 0x00, 0x01, 0x00, 0xe0, 0x0f, 0x00 };

static unsigned char IconUpload_bits[] = {
   0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0x01, 0x00, 0x80, 0x03, 0x00,
   0x40, 0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00 };

static unsigned char IconCalendar_bits[] = {
   0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x20, 0x08, 0x00, 0xe0, 0x0f, 0x00,
   0x20, 0x08, 0x00, 0xa0, 0x0a, 0x00, 0x20, 0x08, 0x00, 0xe0, 0x0f, 0x00 };

//XBM Icon END

void setup() {

  //Button
  pinMode(PIN_BTN_POSITIVE, INPUT);
  pinMode(PIN_BTN_NEGATIVE, INPUT);
  
  //시리얼 통신 시작
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {

  //Button
  String line = "";
  if (digitalRead(PIN_BTN_POSITIVE) == LOW) {
    delay(300);
    if (digitalRead(PIN_BTN_POSITIVE) == LOW) {//Double or Long Click
      line = "BTN_POS_EX";
    } else {
      line = "BTN_POS_NONE";
    }
    mySerial.println(line);
    Serial.println(line);
  }
  if (digitalRead(PIN_BTN_NEGATIVE) == LOW) {
    delay(300);
    if (digitalRead(PIN_BTN_NEGATIVE) == LOW) {//Double or Long Click
      line = "BTN_NEG_EX";
    } else {
      line = "BTN_NEG_NONE";
    }
    mySerial.println(line);
    Serial.println(line);
  }
  
  
  //Bluetooth 로 메시지 수신
  int i=0;
  while(mySerial.available()) {
    //문자열 초기화
    if (!mIsReceivingBegin) {
      mIsReceivingBegin = true;
      clearMessage();
    }

    //문자열 받아오기
    char myChar = (char) mySerial.read();//한 글자씩 받아옴
    if (myChar != '\n') {
      mReceivedMessage[i]=myChar;//문자열에 추가
      i++;
      delay(5);//끊김 방지
    }
  }//while END

  Serial.println(mReceivedMessage);
  //문자열 받아오기 flag false
  mIsReceivingBegin = false;
  
  //메시지 핸들
  MESSAGE_TYPE =getMessageType(mReceivedMessage);
  controlMessage(MESSAGE_TYPE, mReceivedMessage);

  //연결 확인
  BLUETOOTH_CONNECTED=checkConnection();

  //시스템그리기
  u8g.firstPage();
  do {
    if (BOOT_COMPLETE) {
      drawUI();
      drawMessage();
      drawInfo(MESSAGE_TYPE);
    } else {
      BOOT_ANIMATION_STATUS++;
      drawBootAnimation();

      if (BOOT_ANIMATION_STATUS >= BOOT_ANIMATION_MAX+100) {
        BOOT_COMPLETE=true;
      }
    }
    
  } while(u8g.nextPage());  
}

//부팅 애니메이션
void drawBootAnimation() {
  short a = BOOT_ANIMATION_STATUS, m = BOOT_ANIMATION_MAX;
  float am = (float) a/m;
  if (am > 1) {
    am=1.0;
  }
  //I
  u8g.drawFrame(17,22,am*2,am*21);
  //N
  u8g.drawFrame(20,22,am*2,am*21);
  u8g.drawLine(20,22,20+am*15,22+am*21);
  u8g.drawFrame(35-am*2,43-am*21,am*2,am*21);//35,43
  //O
  u8g.drawFrame(37,22,am*18, am*21);//55,43
  //R
  u8g.drawFrame(58,22,am*12,am*11);//70,33
  u8g.drawFrame(58,43-am*9,am*2,am*9);//58,34|59,43
  u8g.drawLine(72-am*6,43-am*9,72,43);//66,34|72,43
  //O
  u8g.drawFrame(73,22,am*18, am*21);//55,43
  //I
  u8g.drawFrame(93,22,am*2,am*21);
  //D
  u8g.drawFrame(96,22,am*8,am*2);
  u8g.drawLine(104,23,104+am*5,23+am*2);
  u8g.drawFrame(110,26,am*2,am*11);
  u8g.drawLine(110,38,110-am*4,38-am*4);
  u8g.drawFrame(104-am*8,42-am*2,am*8,am*2);
  u8g.drawFrame(97-am*2,43-am*21,am*2,am*21);
}

void drawMessage() {
  //수신 메시지 표시
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0,40);
  u8g.print(simplifyMessage() );
}

void drawInfo(int type) {
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(30,25);
  switch(type) {
    case TEXT_TYPE_CALL://Call
      u8g.drawXBM(5,17,18,8,IconCall_bits);
      u8g.print("Call" );
      break;
    case TEXT_TYPE_SMS://SMS
      u8g.drawXBM(5,17,18,8,IconSMS_bits);
      u8g.print("New Message");
      break; 
    case TEXT_TYPE_OTHER://Other
      u8g.drawXBM(5,17,18,8,IconOther_bits);
      u8g.print("New Notice");
      break;  
    case TEXT_TYPE_MESSAGE://Message
      u8g.drawXBM(5,17,18,8,IconChat_bits);
      u8g.print("New Chat");
      break;  
    case TEXT_TYPE_VIDEO://Video
      u8g.drawXBM(5,17,18,8,IconVideo_bits);
      u8g.print("New Video");
      break;    
    case TEXT_TYPE_PICTURE://Picture
      u8g.drawXBM(5,17,18,8,IconPicture_bits);
      u8g.print("New Picture");
      break;    
    case TEXT_TYPE_DOWNLOAD://Download
      u8g.drawXBM(5,17,18,8,IconDownload_bits);
      u8g.print("Downloading");
      break;    
    case TEXT_TYPE_UPLOAD://Upload
      u8g.drawXBM(5,17,18,8,IconUpload_bits);
      u8g.print("Uploading");
      break;    
    case TEXT_TYPE_CALENDAR://Calendar
      u8g.drawXBM(5,17,18,8,IconCalendar_bits);
      u8g.print("Schedule");
      break;  
  }
}

void drawUI() {
  //날짜 표시
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0,64);
  u8g.print( getDate() );

  //시간 표시
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(64,64);
  u8g.print( getTime() );

  //연결 정보
  if (!BLUETOOTH_CONNECTED) {
    u8g.drawXBM(0,0,18,8,IconDisconnected_bits);
  } else {
    u8g.drawXBM(0,0,18,8,IconConnected_bits);
  } 
}

void setRTC() {
  if (TIME_RECEIVED && DATE_RECEIVED) {
    rtc.stopRTC();
    rtc.setTime(TIME_HOUR, TIME_MINUTE, TIME_SECOND);//시, 분, 초
    rtc.setDate(DATE_DATE, DATE_MONTH, DATE_YEAR);//일, 월, 년
    rtc.startRTC();
  }
}

void clearMessage() {
  mReceivedMessage[0]='\0';
  Serial.print("CLEAR");
  Serial.println(mReceivedMessage);
  for(int i=0; i < MESSAGE_SIZE; i++) {
    mReceivedMessage[i]=' ';
  }
}

void extNextCheck() {
  //블루투스로 수신시 발동
  TIME_NEXT_CHECK_MINUTE = TIME_MINUTE+TIME_NEXT_CHECK_COUNT;
  TIME_NEXT_CHECK_HOUR = rtc.getHours();
  if (TIME_NEXT_CHECK_MINUTE >= 60) {
    TIME_NEXT_CHECK_HOUR++;
    TIME_NEXT_CHECK_MINUTE-=60;
  }
}

String simplifyMessage() {
  char ret_call[20];
      for(int i=0; i < 16; i++) {
        ret_call[i]=mReceivedMessage[i+2];  
      }
      ret_call[16]='\0';
      ret_call[17]='\0';
      ret_call[18]='\0';
      ret_call[19]='\0';
      return ret_call;
}

void controlMessage(int type, char str[MESSAGE_SIZE]) {
  switch(type) {
    case TEXT_TYPE_DATE:
      //날짜 저장
      Serial.println("Date Received");
      DATE_YEAR = (str[2]-'0')*10+(str[3]-'0');
      DATE_MONTH = (str[4]-'0')*10+(str[5]-'0');
      DATE_DATE = (str[6]-'0')*10+(str[7]-'0');
      DATE_RECEIVED = true;
      setRTC();
      clearMessage();
      break;
    case TEXT_TYPE_TIME:
      //시간 저장
      Serial.println("Time Received");
      TIME_HOUR = (str[3]-'0')*10+(str[4]-'0');
      TIME_MINUTE = (str[5]-'0')*10+(str[6]-'0');
      TIME_SECOND = (str[7]='0')*10+(str[8]='0');
      TIME_RECEIVED = true;
      switch(str[2]) {
        case '0'://24
          TIME_24 = true;
          break;
        default:
          TIME_24 = false;  
      }
      setRTC();
      clearMessage();
      extNextCheck();
      break;
    case TEXT_TYPE_DELETE:
      clearMessage();
      break;  
  }
}

boolean checkConnection() {
  //연결 확인
  return (rtc.getHours()*100+rtc.getMinutes() < TIME_NEXT_CHECK_HOUR*100+TIME_NEXT_CHECK_MINUTE);
}

int getMessageType(char str[MESSAGE_SIZE]) {
  if (str[0] == '&') {
    switch(str[1]) {
      case 'D'://Date
        return TEXT_TYPE_DATE;
      case 'T'://Time
        return TEXT_TYPE_TIME;  
      case 'C'://Call
        return TEXT_TYPE_CALL;
      case 'S'://SMS
        return TEXT_TYPE_SMS;
      case 'O'://Other
        return TEXT_TYPE_OTHER;  
      case 'X'://Delete
        return TEXT_TYPE_DELETE;  
      case 'M'://Message
        return TEXT_TYPE_MESSAGE;  
      case 'V'://Video
        return TEXT_TYPE_VIDEO;  
      case 'P'://Picture
        return TEXT_TYPE_PICTURE;  
      case 'U'://Upload
        return TEXT_TYPE_UPLOAD;  
      case 'W'://Download
        return TEXT_TYPE_DOWNLOAD;  
      case 'L'://Calendar
        return TEXT_TYPE_CALENDAR;            
      default://&이 제일 앞에 있는 평문의 경우 &을 두 번 쓴다.
        return TEXT_TYPE_NULL;  
    }
  }
  return TEXT_TYPE_NULL;
}

String getDate() {
  int dm1, dm2, dd1, dd2;
  dm1 = rtc.getMonth()/10+48;
  dm2 = rtc.getMonth()%10+48;
  dd1 = rtc.getDay()/10+48;
  dd2 = rtc.getDay()%10+48;

  char ret[5];
  ret[0]=(char) dm1;
  ret[1]=(char) dm2;
  ret[2]='/';
  ret[3]=(char) dd1;
  ret[4]=(char) dd2;

  return ret;
}

String getTime() {
  int th1, th2, tm1, tm2;
  th1 = rtc.getHours()/10+48;
  th2 = rtc.getHours()%10+48;
  tm1 = rtc.getMinutes()/10+48;
  tm2 = rtc.getMinutes()%10+48;

  char ret[8];
  ret[0]=' ';
  ret[1]= ' ';
  ret[2]=' ';
  ret[3]= (char) th1;
  ret[4]= (char) th2;
  ret[5]= ':';
  ret[6]= (char) tm1;
  ret[7]= (char) tm2;

  if (!TIME_24) {
    ret[0]='A';
    ret[1]='M';

    int resT=(th1-48)*10+(th2-48)-12;
    if (resT > 0) {
      ret[0]='P';
      ret[1]='M';
      ret[3]=(char) resT/10+48;
      ret[4]=(char) resT%10+48;
      
    }
  }
  return ret;
}

