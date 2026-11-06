// Prints a message and a counter to the Serial Monitor every second

int count = 0;

void setup() {
  Serial.begin(9600);          // start serial communication at 9600 baud
  Serial.println("Hello from Arduino!");
}

void loop() {
  Serial.print("Count: ");
  Serial.println(count);       // print the counter value
  count++;
  delay(1000);                 // wait 1 second
}