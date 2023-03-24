const int potmeterPin = A0;
double potmeterVal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  potmeterVal = analogRead(potmeterPin); // range 350 ~ 975
  potmeterVal = abs((potmeterVal - 350) / (975 - 350) - 1);

  if (potmeterVal < 0) {
    potmeterVal = 0;
  } else if (potmeterVal > 1) {
    potmeterVal = 1;
  }
  Serial.println(potmeterVal);
  delay(300);
}
