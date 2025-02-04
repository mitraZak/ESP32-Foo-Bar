int n;

bool is_prime(int number){

    if(number < 2) return false;
    if(number == 2) return true;
    if(number % 2 == 0) return false;
    for(int i=3; (i*i)<=number; i+=2){
        if(number % i == 0 ) return false;
    }
    return true;

}

void setup() {
  Serial.begin(115200);
  Serial.println("enter");
  while (Serial.available() == 0) {
}
  String input = Serial.readStringUntil('\n');
  int n = input.toInt();
  Serial.print("Received ");
  Serial.println(n);

  for (int i=0; i<=n; i++){
      String type = ((i%2)==0) ? "Foo" : "Bar";
      if (is_prime(i)) type+= "Prime";
      Serial.println(i);
      Serial.println(type);
  }
}

void loop() {
}
