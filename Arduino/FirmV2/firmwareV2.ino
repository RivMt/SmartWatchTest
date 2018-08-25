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

//Message Data Type
const short DATA_TYPE_SYNC = 0;

//Message Icon Type
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
bool mTime24 = false; //24시간 단위계
short mTimeForm = 0;//시간 표시 스타일
short mTimeNextCheckHour = 0;
short mTimeNextCheckMinute = 0;
const short TIME_NEXT_CHECK_COUNT = 30;

//Date
short mDateForm = 0;//날짜 표시 스타일

//XBM Icon BEGIN
const short mIconWidth = 18;
const short mIconHeight = 8;
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
            //Delay Auto Disconnection Time
            mTimeNextCheckMinute = rtc.getMinutes()+TIME_NEXT_CHECK_COUNT;
            mTimeNextCheckHour = rtc.getHours();
            if (mTimeNextCheckMinute >= 60) {
                mTimeNextCheckHour++;
                mTimeNextCheckMinute-=60;
            }
        }

        //Receive String
        char myChar = (char) mySerial.read();
        if (myChar != '\n') {
            received_message[i]=myChar;
            i++;
            //Delay for maintanance connection
            delay(5); 
        }
    }
    //End Receiving
    mIsReceivingBegan = false;

    //Handling Message
    short type = findMessageType(received_message);
    if (type == ICON_TYPE_DATE) {

    } else if (type == DATA_TYPE_SYNC) {
        setSync();
    } else if (type > 0) {
        mMessageType = type;
    } else if (type == -1) {
        mMessageDesc = received_message;
    } else if (type == -2) {
        mMessageTitle = received_message;
    }

    //Draw System UI
    u8g.firstPage();
    do {
        //When booting is in progress, show animation
        if (mBootComplete) {
            //When device has not been connected, do not show watch face
            if (mBluetoothConnected) {
                //Draw Icon
                u8g.drawXBM(2,2,mIconWidth, mIconHeight, getMessageIcon(mMessageType));
                //Reset Font
                u8g.setFont(u8g_font_unifont);
                //Draw Title
                u8g.setPrintPos(2+mIconWidth, 2);
                u8g.print(mMessageTitle);
                //Draw Description
                u8g.setPrintPos(2, 6+mIconHeight);
                u8g.print(mMessageDesc);
            }
        } else {
            //When status is bigger than max, complete animation
            if (mBootAnimationStatus >= BOOT_ANIMATION_MAX) {
                mBootComplete = true;
            }

            //Booting is in progress
            mBootAnimationStatus++;
        }
    } while(u8g.nextPage());

    //Check Blueototh Connection
    mBluetoothConnected = (rtc.getHours()*100+rtc.getMinutes() < mTimeNextCheckHour*100+mTimeNextCheckMinute);

}

//Output Message Type at String
short findMessageType(char str[MESSAGE_SIZE]) {
    if (str[0] == '&' ) {
        switch (str[1]) {
            case 'C':
                return ICON_TYPE_CALL;
            case 'S':
                return ICON_TYPE_SMS;
            case 'O':
                return ICON_TYPE_OTHER;
            case 'X':
                return ICON_TYPE_DELETE;
            case 'M':
                return ICON_TYPE_MESSAGE;
            case 'V':
                return ICON_TYPE_VIDEO;
            case 'P':
                return ICON_TYPE_PICTURE;
            case 'U':
                return ICON_TYPE_UPLOAD;
            case 'W':
                return ICON_TYPE_DOWNLOAD;
            case 'L':
                return ICON_TYPE_CALENDAR;  
            case 'Y':
                return DATA_TYPE_SYNC;       
            case '&':
                return -2;//Title
        }
    }
    return -1;//Desc
}

unsigned char getMessageIcon(short type) {
    switch(type) {
        case ICON_TYPE_CALL:
            return IconCall_bits;
    }
}

void setSync(char str[MESSAGE_SIZE]) {
    //Var
    int date_year, date_month, date_day,
        time_hour, time_minute, time_second;

    //Save Date
    date_year = (str[3]-'0')*10+(str[4]-'0');
    date_month = (str[5]-'0')*10+(str[6]-'0');
    date_day = (str[7]-'0')*10+(str[8]-'0');

    //Save Time
    time_hour = (str[9]-'0')*10+(str[10]-'0'); 
    time_minute = (str[11]-'0')*10+(str[12]-'0');
    time_second = (str[13]-'0')*10+(str[14]-'0');

    //Set Time Form
    switch(str[2]) {
        case '0'://24
            mTime24 = true;
            break;
        default:// AM/PM
            mTime24 = false;
            break;
    }

    //Set RTC and Start RTC
    rtc.stopRTC();
    rtc.setTime(time_hour, time_minute, time_second);//hhmmss
    rtc.setDate(date_day, date_month, date_year);//ddMMyy
    rtc.startRTC();
}

String getDate() {
    //년, 월, 일 자릿수별로 ASCII 코드값 변수 하나씩
    int dy1, dy2, dm1, dm2, dd1, dd2;
    dy1 = rtc.getYear()/10+48;
    dy2 = rtc.getYear()%10+48;
    dm1 = rtc.getMonth()/10+48;
    dm2 = rtc.getMonth()%10+48;
    dd1 = rtc.getDate()/10+48;
    dd2 = rtc.getDate()%10+48;

    //char형으로 변환
    switch(mDateForm) {
        case 0://Normal
            char ret0[5];
            ret0[0] = (char) dm1;
            ret0[1] = (char) dm2;
            ret0[2] = '/';
            ret0[3] = (char) dd1;
            ret0[4] = (char) dd2;
            return ret0;
    }
}

String getTime() {
    //시, 분, 초 자릿수별로 ASCII 코드값 하나씩
    int th1, th2, tm1, tm2, ts1, ts2;
    th1 = rtc.getHours()/10+48;
    th2 = rtc.getHours()%10+48;
    tm1 = rtc.getMinutes()/10+48;
    tm2 = rtc.getMinutes()%10+48;
    ts1 = rtc.getSeconds()/10+48;
    ts2 = rtc.getSeconds()%10+48;

    //char형으로 변환
    switch(mTimeForm) {
        case 0://Normal
            char ret0[8];
            ret0[0]=' ';
            ret0[1]=' ';
            ret0[2]=' ';
            ret0[3]=(char) th1;
            ret0[4]=(char) th2;
            ret0[5]=':';
            ret0[6]=(char) tm1;
            ret0[7]=(char) tm2;

            //시간 형식
            if (!mTime24) {
                ret0[0]='A';
                ret0[1]='M';

                short resT = (th1-48)*10+(th2-60);
                if ( resT > 0 ) {
                    ret0[0]='P';

                    ret0[3]=(char) resT/10+48;
                    ret0[4]=(char) resT%10+48; 
                }
            }

            return ret0;
    }
}