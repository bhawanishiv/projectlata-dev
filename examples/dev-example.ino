#include <SoftwareSerial.h>
#include "lib/ACS712.h"
ACS712 currentSensor(ACS712_30A);
SoftwareSerial logSerial(2, 3); // RX, TX
void reqDefs();
void reqCmds();
void sendReadings();
void processSendReading(const int &pinNo, const String &pinFor);
bool processDefs(const String &_defs);
bool processCmds(const String &_cmds, const String &_defs);
void saveDefs(const String &_defs);
void saveCmds(const String &_cmds);
char getReqType(const String &parsedReq);
String parseReq(const String &req);
int delaySendReading{1000 * 5};
const String instanceId {
  "-MAc6e2zOVGHyao7aHrw"
};
int nPins = 20;
String cmds;
String defs;
void setup()
{
  Serial.begin(9600);
  logSerial.begin(9600);
}

void loop()
{
  if (Serial.available() == 0)
  {
    if (defs.length() < (nPins + 1)) reqDefs();
    else {
      if (defs.length() == (nPins + 1)  && cmds.length() == (nPins + 1)) sendReadings();
      reqCmds();
    }
    delay(3 * 1000);
  }
  if (Serial.available() > 0)
  {
    String request = parseReq(Serial.readString());
    if (!(request == "empty")  || !(request == "NaVR") || !(request == "error") || !(request == "success") ) {
      char reqType = getReqType(request);
      if (reqType == 'C') {
        saveCmds(request);
        processCmds(cmds, defs);
      }
      else if (reqType == 'D') {
        saveDefs(request);
        processDefs(defs);
      }
    } else logSerial.println("SERVER RESPONDED ->" + request);
  }
}

void reqCmds()
{
  //{\"instanceId\":\"-MAc6e2zOVGHyao7aHrw\",\"reqType\":\"get-cmds\"}
  String req = "{\"instanceId\":\"";
  req += instanceId + "\",\"reqType\":\"get-cmds\"}";
  Serial.println(req);
  Serial.flush();
  logSerial.println("REQUESTING CMDS ->" + req);

}
void reqDefs()
{
  //{\"instanceId\":\"-MAc6e2zOVGHyao7aHrw\",\"reqType\":\"get-defs\"}
  String req = "{\"instanceId\":\"";
  req += instanceId + "\",\"reqType\":\"get-defs\"}";
  Serial.println(req);
  Serial.flush();
  logSerial.println("REQUESTING DEFINITIONS ->" + req);
}

void sendReadings()
{
  String newDefs = defs.substring(1);
  if (newDefs.length() == nPins) {
    for (int i = 0; i < newDefs.length(); i++) {
      char def = newDefs.charAt(i);
      if (def == '1') { //For all inputs to be read;
        if (i > 13) {
          String pinFor{"UNKNOWN PIN"};
          if (i == 14) pinFor = "4";
          else if (i == 15) pinFor = "5";
          else if (i == 16) pinFor = "6";
          else if (i == 17) pinFor = "7";
          else if (i == 18) pinFor = "8";
          else if (i == 19) pinFor = "9";
          processSendReading(i, pinFor);
          delay(delaySendReading);
        }
      }

    }
  }
}

void processSendReading(const int &pinNo, const String &pinFor) {
  currentSensor.setPin(pinNo);
  currentSensor.calibrate();
  int value = currentSensor.getCurrentAC();
  //  {\"instanceId\":\"sfd\", \"reqType\":\"set-readings\", \"pinNo\":\"5\",\"value\":6}
  String res = "{\"instanceId\":\"";
  res += instanceId + "\", \"reqType\":\"set-readings\", \"pinNo\":\"";
  res += pinFor + "\",\"value\":";
  res += String(value) + "}";
  Serial.println(res);
  Serial.flush();
  logSerial.println("SENDING READING FOR: " + pinFor + "->" + res);
}

bool processDefs(const String &_defs) {
  String newDefs = _defs.substring(1);
  if (newDefs.length() < nPins) return false;
  logSerial.println("PROCESSING DEFINITIONS");
  for (int i = 0; i < newDefs.length(); i++) {
    char def = newDefs.charAt(i);
    if (def == '1') pinMode(i, INPUT);
    else if (def == '0') pinMode(i, OUTPUT);
  }
  return true;
}

bool processCmds(const String &_cmds, const String &_defs) {
  String newCmd = _cmds.substring(1);
  if (newCmd.length() < nPins) return false;
  if (_defs.length() < (nPins + 1)) return false;
  logSerial.println("PROCESSING CMDS");
  for (int i = 0; i < newCmd.length(); i++) {
    char def = _defs.charAt(i);
    if (def == '0') { //For outputs type only

      if (i > 13) { //For Analog outputs
        String analogValue = newCmd.substring(i, i + 1);
        analogWrite(i, analogValue.toInt());
      }
      else { //For digital outputs
        char value = newCmd.charAt(i);
        if (value == '1') digitalWrite(i, HIGH);
        else if (value == '0') digitalWrite(i, LOW);
      }
    }
  }
  return true;
}
void saveDefs(const String &_defs) {
  logSerial.println("SAVING DEFINITIONS");
  defs = _defs;
}

void saveCmds(const String &_cmds) {
  logSerial.println("SAVING CMDS");
  cmds = _cmds;
}

char getReqType(const String &parsedReq) {
  return parsedReq.charAt(0);
}

String parseReq(const String &req) {
  //req=""| {D120012000012}| {C12121200012012};
  int firstBracketOccurance;
  int lastBracketOccurance;
  if (req == "") return "empty";
  if (req.indexOf("error") > 0) return "error";
  else if (req.indexOf("success") > 0) return "success";
  else if (req.length() < (nPins + 3)) return "NaVR";
  else {
    firstBracketOccurance = req.indexOf('{');
    if (firstBracketOccurance < 0) return "NavR";  //Not a valid request
    lastBracketOccurance = req.indexOf('}');
    if (firstBracketOccurance < 0) return "NavR";
    int parsedReqLen = lastBracketOccurance - firstBracketOccurance + 1;
    if (parsedReqLen == (nPins + 3)) return req.substring(firstBracketOccurance + 1, lastBracketOccurance);
  }
}
