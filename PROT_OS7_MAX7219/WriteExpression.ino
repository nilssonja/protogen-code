///////////////////////////Matrix writing Expressions\\\\\\\\\\\\\\\\\\\\\\\\\\\

void writeExpres() {
  switch (state) {
    case 0:                             //First button press: Happy expression
      matrix.drawBitmap(EyeLeft, 0, EyeL, 16, 8, HIGH);
      matrix.drawBitmap(EyeRight, 0, Eye, 16, 8, HIGH);
      break;

    case 1:
      matrix.drawBitmap(EyeLeft, 0, Spooked, 16, 8, HIGH);
      matrix.drawBitmap(EyeRight, 0, Spooked, 16, 8, HIGH);
      break;

    case 2:                             //Third button press: Angry expression
      matrix.drawBitmap(EyeLeft, 0, AngryL, 16, 8, HIGH);
      matrix.drawBitmap(EyeRight, 0, Angry, 16, 8, HIGH);
      break;

    case 3:
      matrix.drawBitmap(EyeLeft, 0, vwvL, 16, 8, HIGH);
      matrix.drawBitmap(EyeRight, 0, vwv, 16, 8, HIGH);
      break;
    case 4:
      matrix.drawBitmap(EyeLeft, 0, FBEyeBasicL, 16, 8, HIGH);
      matrix.drawBitmap(EyeRight, 0, FBEyeBasic, 16, 8, HIGH);
      break;
    case 5:
      matrix.drawBitmap(EyeLeft, 0, FBEyeLoveL, 16, 8, HIGH);
      matrix.drawBitmap(EyeRight, 0, FBEyeLove, 16, 8, HIGH);
  }
  rising = 0;
}
