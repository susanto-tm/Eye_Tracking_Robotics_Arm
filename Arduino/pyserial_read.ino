const byte numChar = 12;
char xCoord[numChar];
char yCoord[numChar];

boolean newData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ready");
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  receiveStartEndMarkers();
  showData();
}

void receiveStartEndMarkers() {
  static byte ndx = 0;
  static byte ndy = 0;
  char startMarker = '<';
  char chgCoordinateMarker = ',';
  char endMarker = '>';
  char rc;
  boolean progressX = false;
  boolean progressY = false;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    // x coordinate parser
    if (progressX == true && rc != chgCoordinateMarker) {
      Serial.println("ProgressX");
      xCoord[ndx] = rc;
      ndx++;
    }
    else if (rc == startMarker && progressX == false) {
      Serial.println("ChangedX");
      progressX = true;
    }
    else if (rc == chgCoordinateMarker) {
      progressX = false;
      Serial.println("ChangedY");
      progressY = true;
      Serial.println("EndX");
    }
    else if (progressY == true && rc != endMarker) {
      Serial.println("ProgressY");
      yCoord[ndy] = rc;
      ndy++;
    }
    else if (rc == endMarker) {
      progressY = false;
      newData = true;
      Serial.println("Parse Ended");
    }

    Serial.print("X is: ");
    Serial.println(xCoord);

    Serial.print("Y is: ");
    Serial.println(yCoord);
    
//    if (recvInProgress == true) {
//      Serial.println("Start process");
//      if (rc != endMarker) {
//        receivedChars[ndx] = rc;
//        Serial.println("ndx + 1");
//        ndx++;
//        if (ndx >= numChar) {
//          ndx = numChar - 1;
//        }
//      }
//      else {
//        receivedChars[ndx] = '\0'; // terminate the string
//        recvInProgress = false;
//        ndx = 0;
//        newData = true;
//      }
//    }
//    else if (rc == startMarker) {
//      Serial.println("received");
//      recvInProgress == true;
//    }
//  }
  
}
}

void showData() {
  if (newData == true) {
    Serial.println("Final: ");

    Serial.print("X is: ");
    Serial.println(xCoord);

    Serial.print("Y is: ");
    Serial.println(yCoord);
    newData = false;
  }
}
