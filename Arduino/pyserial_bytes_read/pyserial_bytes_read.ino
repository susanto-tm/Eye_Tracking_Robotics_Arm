const byte numChars = 12;
char receivedChars[numChars];
char tempChars[numChars];

// variable to store parsed data
int xData = 0;
int yData = 0;

boolean newData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ready");

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  receiveStartEndMarker();
  if (newData == true) {
    strcpy(tempChars, receivedChars); // creates a copy of receivedChars since strtok() will replace commas with \0
    parseData();
    showData();
    newData = false;
  }
}

void receiveStartEndMarker() {
  static boolean receiveProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    Serial.println(rc);

    if (receiveProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate string with null operator
        Serial.println(receivedChars);
        receiveProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      receiveProgress = true;
    }
  }
}

void parseData() {
  // split the data into its parts
  char* strtokIndx; // strtokIndx pointer to char

  strtokIndx = strtok(tempChars, ","); // takes tempChars as strings then searches for the string before it reaches the comma
  xData = atoi(strtokIndx); // takes strings up to the comma and changes it into int for first coordinate split

  strtokIndx = strtok(NULL, ","); // continues from previous token and finds for next split
  yData = atoi(strtokIndx); // takes the continued string and changes it into int for second coordinate split
  
}

void showData() {
  Serial.print("X Coordinate: ");
  Serial.println(xData);

  Serial.print("Y Coordinate: ");
  Serial.println(yData);
}
