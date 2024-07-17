#include <ThingSpeak.h>
#include <WiFi.h>
#include "ESP32_MailClient.h"

const char *ssid = "Aki";
const char *pass = "03979tASB";
int parkingRupee = 0;
int countcar = 0;
#define RXp2 16
#define TXp2 17

const char* emailSenderAccount = "akshajsingh.bisht2021@vitstudent.ac.in";
const char* emailSenderPassword = "qctj dkxo swxq wzcg";
const char* emailRecipient = "armaano.ajay2021@vitstudent.ac.in";
const char* smtpServer = "smtp.gmail.com";
const int smtpServerPort = 465;
const char* emailSubject = "Parking Information";

SMTPData smtpData;

WiFiClient client;

long myChannelNumber = 2521047;
const char myWriteAPIKey[] = "5GI45ZLYXQANIVNP";

long myChannelNumber1 = 2521040;
const char myWriteAPIKey1[] = "2OWZERPFOX7CE4VQ";

void sendCallback(SendStatus info);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);

  // Email setup
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  smtpData.setSender("ESP32", emailSenderAccount);
  smtpData.setPriority("High");
  smtpData.setSubject(emailSubject);
  smtpData.setSendCallback(sendCallback);
}

void loop() {
  // Check if there's any data available to read from Serial2
  if (Serial2.available() > 0) {
    // Read the incoming data and store it in a string
    String receivedData = Serial2.readString();

    // Check if the received message contains "Elapsed Time:"
    if (receivedData.indexOf("Elapsed Time:") != -1) {
      // Extract the substring containing the number of seconds
      int startIndex = receivedData.indexOf(":") + 1;
      int endIndex = receivedData.indexOf("seconds") - 1;
      String secondsString = receivedData.substring(startIndex, endIndex);

      // Convert the substring to an integer
      int seconds = secondsString.toInt();

      int amt = calculateCost(seconds);
      countcar += 1;
      parkingRupee += amt;
      // Print the integer value of the seconds
      Serial.print("Elapsed Time (seconds): ");
      Serial.println(seconds);
      Serial.print("Cost (Rupees): ");
      Serial.println(amt);
      Serial.print("Total Cost in Parking (Rupees): ");
      Serial.println(parkingRupee);
      Serial.println("-------------------");

      // Send email
      smtpData.setMessage("Elapsed Time: " + secondsString + " seconds\n"
                          "Cost: " + String(amt) + " Rupees\n"
                          "Total Cost in Parking: " + String(parkingRupee) + " Rupees", true);
      smtpData.addRecipient(emailRecipient);

      if (!MailClient.sendMail(smtpData))
        Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

      //smtpData.empty();

      ThingSpeak.setField(1, amt);
      ThingSpeak.setField(2, seconds);
      ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);

      ThingSpeak.setField(1, amt);
      ThingSpeak.setField(2, seconds);
      ThingSpeak.setField(3, parkingRupee);
      ThingSpeak.setField(4, countcar);
      ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    }
  }
}

int calculateCost(int time) {
  if (time <= 5) {
    return 5; // Cost for the first 5 seconds
  } else {
    return 5 + 2 * (time - 5); // Cost for the remaining seconds
  }
}

void sendCallback(SendStatus msg) {
  Serial.println(msg.info());

  if (msg.success()) {
    Serial.println("----------------");
  }
}