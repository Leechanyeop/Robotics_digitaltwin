
#define ENA_PIN 4
#define DIR_PIN 5
#define STP_PIN 6

void setup() 
{
  pinMode(ENA_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STP_PIN, OUTPUT);
  digitalWrite(ENA_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
}
   
void loop() 
{

  for( int i = 0; i < 500; i++)
  {
    digitalWrite(STP_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STP_PIN, LOW);
    delayMicroseconds(1000);
  }

  digitalWrite(DIR_PIN, HIGH);

  for( int i = 0; i < 500; i++)
  {
    digitalWrite(STP_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STP_PIN, LOW);
    delayMicroseconds(1000);
  }

}


