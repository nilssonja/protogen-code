/////////////////////////// Microphone \\\\\\\\\\\\\\\\\\\\\\\\\\\

void microphone() {
  if (micState == 1) {
    float driftingPosition = analogRead(micPin);
    filterDCComponent = driftingPosition * 0.01 + filterDCComponent * 0.99;
    float correctedPosition = driftingPosition - filterDCComponent;
    if (correctedPosition < 0) {
      correctedPosition = correctedPosition * -1;
    }
    total = total - readings[readIndex];
    readings[readIndex] = correctedPosition;
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= numReadings) {
      readIndex = 0;
    }
    average = total / numReadings;
    // Serial.println(average);
    if (average > sensitivity && average < sensitivity + 32) {
      speakingActive = 1;
      matrix.fillRect(MawRight, 0, 32, 8, LOW);
      matrix.fillRect(MawLeft, 0, 32, 8, LOW);
      switch(state) {
        case 0:
          matrix.drawBitmap(MawRight, 0, mawSpeakS, 32, 8, HIGH);
          matrix.drawBitmap(MawLeft, 0, mawSpeakSL, 32, 8, HIGH);
          break;
        case 4:
          matrix.drawBitmap(MawLeft, 0, FBMouthBasicLSoft, 32, 8, HIGH);
          matrix.drawBitmap(MawRight, 0, FBMouthBasic, 32, 8, HIGH);
          break;
      }
      matrix.write();
      speakingDelay = millis();
      }
      else if(average >= sensitivity + 32 && average < sensitivity + 65) {
        speakingActive = 1;
        matrix.fillRect(MawRight, 0, 32, 8, LOW);
        matrix.fillRect(MawLeft, 0, 32, 8, LOW);
        switch(state) {
          case 0:
            matrix.drawBitmap(MawRight, 0, mawSpeakM, 32, 8, HIGH);
            matrix.drawBitmap(MawLeft, 0, mawSpeakML, 32, 8, HIGH);
            break;
          case 4:
            matrix.drawBitmap(MawLeft, 0, FBMouthBasicLMedium, 32, 8, HIGH);
            matrix.drawBitmap(MawRight, 0, FBMouthBasic, 32, 8, HIGH);
            break;
        }
        matrix.write();
      speakingDelay = millis();
      } else if (average >= sensitivity + 65) {
        speakingActive = 1;
        matrix.fillRect(MawRight, 0, 32, 8, LOW);
        matrix.fillRect(MawLeft, 0, 32, 8, LOW);
        switch(state){
          case 0:
            matrix.drawBitmap(MawRight, 0, mawSpeakL, 32, 8, HIGH);
            matrix.drawBitmap(MawLeft, 0, mawSpeakLL, 32, 8, HIGH);
            break;
          case 4:
            matrix.drawBitmap(MawLeft, 0, FBMouthBasicLLoud, 32, 8, HIGH);
            matrix.drawBitmap(MawRight, 0, FBMouthBasic, 32, 8, HIGH);
            break;
        }
      
      matrix.write();
      speakingDelay = millis();
      }
    else {
      if ((speakingActive == 1) && (millis() - speakingDelay > 100)) {
        speakingActive = 0;
        matrix.fillRect(MawLeft, 0, 32, 8, LOW);
        matrix.fillRect(MawRight, 0, 32, 8, LOW);
              // Serial.println("Drawing regular maw");
        switch(state) {
          case 0:
            matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, HIGH);
            matrix.drawBitmap(MawRight, 0, maw, 32, 8, HIGH);
            break;
          case 4:
            matrix.drawBitmap(MawLeft, 0, FBMouthBasicL, 32, 8, HIGH);
            matrix.drawBitmap(MawRight, 0, FBMouthBasic, 32, 8, HIGH);
        }
        matrix.write();
      }
    }
  }
}
