#include <U8glib.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <swRTC.h>

//RTC
swRTC mRTC;

//BIOS
bool mBootComplete = false;
short mBootAnimationStatus = 0;
const short BOOT_ANIMATION_MAX = 600;

//Bluetooth
SoftwareSerial mySerial(2, 3);
bool mBluetoothConnected = false;

//Push Button
const short PIN_BTN_POSITIVE = 7;
const short PIN_BTN_NEGATIVE = 6;

//OLED
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

//Messaging
const short MESSAGE_SIZE = 20;
char mMessageDesc[MESSAGE_SIZE] = "\0";
char mMessageTitle[MESSAGE_SIZE] = "\0";
short mMessageType = -1;
bool mIsReceivingBegan = false;

//Message Type
const short ICON_TYPE_DATE = 0;
const short ICON_TYPE_TIME = 1;
const short ICON_TYPE_CALL = 2;
const short ICON_TYPE_SMS = 3;
const short ICON_TYPE_OTHER = 4;
const short ICON_TYPE_DELETE = 5;
const short ICON_TYPE_MESSAGE = 6;
const short ICON_TYPE_VIDEO = 7;
const short ICON_TYPE_PICTURE = 8;
const short ICON_TYPE_DOWNLOAD = 9;
const short ICON_TYPE_UPLOAD = 10;
const short ICON_TYPE_CALENDAR = 11;

//Time
bool mTime24 = true; //24시간 단위계
bool mTimeReceived = false;
short mTimeHour = 0;
short mTImeMinute = 30;
short mTimeSecond = 0;
short mTimeNextCheckHour = 0;
short mTimeNextCheckMinute = 0;
const short TIME_NEXT_CHECK_COUNT = 30;

//Date
bool mDateReceived = false;
short mDateYear = 1970;
short mDateMonth = 1;
short mDateDate = 1;

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

//Setup
void setup() {
    //Button
    pinMode(PIN_BTN_POSITIVE, INPUT);
    pinMode(PIN_BTN_NEGATIVE, INPUT);

    //Serial
    Serial.begin(9600);
    mySerial.begin(9600);
}

//Loop
void loop() {
    //Button
    String line;
    //Positive Button
    if (digitalRead(PIN_BTN_POSITIVE) == LOW) {
        //Wait for Long Click
        delay(300);
        if (digitalRead(PIN_BTN_POSITIVE) == LOW) {
            line = "BTN_POS_EX";
        } else {
            line = "BTN_POS_NONE";
        }
        mySerial.println(line);
    }
    //Negative Button
    if (digitalRead(PIN_BTN_NEGATIVE) == LOW) {
        //Wait for Long Click
        delay(300);
        if (digitalRead(PIN_BTN_NEGATIVE) == LOW) {
            line = "BTN_NEG_EX";
        } else {
            lien = "BTN_NEG_NONE";
        }
    }

    //Receive Message with Bluetooth
    char received_message[MESSAGE_SIZE] = '\0';
    short i = 0;
    while(mySerial.available()) {
        //Set Flag to Receiving Begin 
        if (!mIsReceivingBegan) {
            mIsReceivingBegan = true;
        }

        //Receive String
        char myChar = (char) mySerial.read();
        if (myChar != '\n') {
            received_message[i]=myChar;
            i++;
            delay(5); 
        }
    }

    //End Receiving
    mIsReceivingBegan = false;

    //Handling Message
    short type = findMessageType(received_message);
    if (type == ICON_TYPE_DATE) {

    } else if (type == ICON_TYPE_TIME) {

    } else if (type >= 0) {
        mMessageType = type;
    } else if (type == -1) {
        mMessageDesc = received_message;
    } else if (type == -2) {
        mMessageTitle = received_message;
    }

    //Check Blueototh Connection
    mBluetoothConnected = checkConnection();

    //Draw System UI
    u8g.firstPage();
    do {
        //Booting Animation
        if (mBootComplete) {
            
        }
    }

}

//Output Message Type at String
short findMessageType(char str[MESSAGE_SIZE]) {
    if (str[0] == '&' ) {
        switch (str[1]) {
            case 'C':
                return ICON_TYPE_CALL;
            case '&':
                return -2;//Title
        }
    }
    return -1;//Desc
}

bool checkConnection() {
  //연결 확인
  return (rtc.getHours()*100+rtc.getMinutes() < mTimeNextCheckHour*100+mTimeNextCheckMinute);
}