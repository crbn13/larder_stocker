
#ifndef serialCom

class serialComClass {

public:
    char* Command = { 0 };
    serialComClass() {
        Serial.begin(9600);
    };

    char* commandInput() {
        while ( (int)tempChar != 13 ) {
            if (Serial.available() > 0) {
                tempChar = Serial.read();
                incomingString[currentChar] = tempChar;
                posModifier = inputCheck(tempChar);
                currentChar += posModifier;
                incomingString[currentChar] = char('\0');
                //incomingString[currentChar+1] = (int)'\0';
                //Serial.println(incomingString);

            } 
            else {
                if (prevCurrentChar != currentChar) {
                    prevCurrentChar += posModifier;
                    Serial.write(27);       // ESC command
                    Serial.print("[2J");    // clear screen command
                    Serial.write(27);
                    Serial.print("[H");     // cursor to home command
                    Serial.println(currentChar);
                    Serial.println(incomingString);
                };
            };
          }
        return incomingString;
    }

    int posModifier = 0;
    char tempChar;
    char incomingString[64] = { 0 };
    int currentChar = 0;
    int incomingByte = 0;
    int prevCurrentChar = 0;
    int inputCheck(int curChr) {
        if ( int(curChr) == 8 ) return -1;
        else return 1;     
    };
};

#endif

