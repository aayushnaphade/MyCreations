// gobal variable
int t1 = 13;
int t2 = 12;
int t3 = 11;
int t4 = 10;
int t5 = 9;

// timer
long  sixty = 60000;// for converting mirosecond 
long timer = 0;

// trigger 
int button = 8;
// relay
int  relay = 7;
int relay2 = 6;

void  select();
void setup() {
  // put your setup code here, to run once:
 pinMode(t1, INPUT_PULLUP);
 pinMode(t2, INPUT_PULLUP);
 pinMode(t3, INPUT_PULLUP);
 pinMode(t4, INPUT_PULLUP);
 pinMode(t5, INPUT_PULLUP); 
 pinMode(button, INPUT_PULLUP);
 pinMode(relay, OUTPUT);
 pinMode(relay2, OUTPUT);
 digitalWrite(relay, HIGH);
 digitalWrite(relay2 , HIGH);
 
//  time selector to be selected
 select();
 Serial.begin(9600);
 Serial.println("Starting the program");
 Serial.println("BY AYUSH NAPHADE..............");
 Serial.println(timer);
}
 
void loop() {
  // put your main code here, to run repeatedly:
 if(digitalRead(button) == LOW){
   digitalWrite(relay2, LOW);
   delay(2000);
   Serial.print("Started");
   digitalWrite(relay2, HIGH);
   Serial.println(timer);
   Serial.println(timer*sixty);
   delay(timer*sixty);
   Serial.println(timer);
   Serial.println(timer*sixty);
   digitalWrite(relay , LOW);
   delay(1000);
   Serial.println("Stopped");
   digitalWrite(relay, HIGH);
 }
 
 delay(10);
}
void select(){

  if(digitalRead(t1) == LOW){
    timer = 15;
  }
  else if(digitalRead(t2) == LOW){
    timer = 30;
  }
  else if(digitalRead(t3) == LOW){
    timer = 60;
  }
  else if(digitalRead(t4) == LOW){
    timer = 90;
  }
  else if(digitalRead(t5) == LOW){
    timer = 120 ;
  }
  else timer = 0;
  return timer;
}
