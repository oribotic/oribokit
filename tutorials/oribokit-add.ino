void setup() {
 // put your setup code here, to run once:
 Serial.begin(115200);
}
 
int x = 0;
 
void loop() {
 // put your main code here, to run repeatedly:
 Serial.print("x = ");
 add();
 Serial.println(x);
}
 
void add ()
{
 // x = x + 1
 x ++;
}
