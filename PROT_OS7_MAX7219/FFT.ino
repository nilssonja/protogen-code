/////////////////////////// Frequency analyzer\\\\\\\\\\\\\\\\\\\\\\\\\\\

void fft() {
  /*SAMPLING*/
  microseconds = micros();
  for (int i = 0; i < samples; i++)
  {
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us) {
      //empty loop
    }
    microseconds += sampling_period_us;
  }
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  PrintVector(vReal, (samples >> 1));
}

void PrintVector(double *vData, uint16_t bufferSize) {
  memset(fftIcon1, 0, sizeof(fftIcon1));
  memset(fftIcon1L, 0, sizeof(fftIcon1L));
  memset(fftIcon2, 0, sizeof(fftIcon2));
  memset(fftIcon2L, 0, sizeof(fftIcon2L));
  vData[0] = vData[4];
  vData[1] = vData[8];
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    int Data = map(vData[i], 0, 800, 1, 8);
    if (Data > 8) {
      Data = 8;
    }
    if (i < 8) {
      for (int j = 0; j < Data; j++) {
        bitWrite(fftIcon2[(15 - (j * 2)) - 1], 7 - i, 1);
        bitWrite(fftIcon2L[15 - (j * 2)], i, 1);
      }
    }
    else if (i < 16) {
      for (int j = 0; j < Data; j++) {
        bitWrite(fftIcon2[15 - ((j * 2))], 7 - (i - 8), 1);
        bitWrite(fftIcon2L[15 - ((j * 2) + 1)], i - 8, 1);
      }
    }
    if (i < 24) {
      for (int j = 0; j < Data; j++) {
        bitWrite(fftIcon1[15 - (j * 2)], 7 - (i - 16), 1);
        bitWrite(fftIcon1L[15 - (j * 2) - 1], i - 16, 1);
      }
    }
    else if (i < 32) {
      for (int j = 0; j < Data; j++) {
        bitWrite(fftIcon1[15 - ((j * 2) + 1)], 7 - (i - 24), 1);
        bitWrite(fftIcon1L[15 - (j * 2)], i - 24, 1);
      }
    }
  }
}
