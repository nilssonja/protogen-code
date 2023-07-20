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
    if (average > sensitivity) {
      speakingActive = 1;
      matrix.fillRect(MawLeft, 0, 32, 8, LOW);
      matrix.fillRect(MawRight, 0, 32, 8, LOW);
      fft();
      matrix.drawBitmap(MawLeft+16, 0, fftIcon2L, 16, 8, HIGH);
      matrix.drawBitmap(MawLeft, 0, fftIcon1L, 16, 8, HIGH);
      matrix.drawBitmap(MawRight+16, 0, fftIcon1, 16, 8, HIGH);
      matrix.drawBitmap(MawRight, 0, fftIcon2, 16, 8, HIGH);
      matrix.write();
      speakingDelay = millis();
    }
    else {
      if ((speakingActive == 1) && (millis() - speakingDelay > 100)) {
        speakingActive = 0;
        matrix.fillRect(MawLeft, 0, 32, 8, LOW);
        matrix.fillRect(MawRight, 0, 32, 8, LOW);
        matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, HIGH);
        matrix.drawBitmap(MawRight, 0, maw, 32, 8, HIGH);
        matrix.write();
      }
    }
  }
}
