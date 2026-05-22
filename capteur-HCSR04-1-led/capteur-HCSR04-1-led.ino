//ETAPE 1 : fonctionnement du système de distance avec 1 led

// Définition des broches sur l'arduino
const int trigPin = 2;
const int echoPin = 3;
const int ledRouge = 7;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledRouge, OUTPUT);
  Serial.begin(9600); // Pour voir la distance sur l'ordi
}

void loop() {
  // 1. Mesure de la distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duree = pulseIn(echoPin, HIGH);
  int distance = duree * 0.034 / 2;

  // 2. Logique de l'alerte
  if (distance > 0 && distance < 50) { 
    // Si un objet est à moins de 50cm
    int v_clignotement = distance * 10; // Plus c'est proche, plus c'est petit
    
    digitalWrite(ledRouge, HIGH);
    delay(v_clignotement); 
    digitalWrite(ledRouge, LOW);
    delay(v_clignotement);
  } 
  else {
    // Si c'est loin ou erreur, on éteint
    digitalWrite(ledRouge, LOW);
  }

  // Affichage pour vérifier
  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");
}