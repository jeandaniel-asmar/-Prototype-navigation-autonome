//ETAPE 3 : ajout du buzzer en fonction de l'urgence de la colision 
/* 
   ALERTE : LED ROUGE (9) ET BUZZER (12) CLIGNOTENT PLUS VITE EN APPROCHANT
*/

const byte TRIG = 2;
const byte ECHO = 3;
const byte LED_VERTE = 5;
const byte LED_JAUNE = 6;
const byte LED_ROUGE = 7; // On garde la 7 pour la LED rouge
const byte BUZZER = 12;   // NOUVELLE LIGNE : Buzzer déplacé sur la broche 12

const unsigned long MEASURE_TIMEOUT = 25000UL;
const float vitesse_son = 340.0;

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);

  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  
  pinMode(BUZZER, OUTPUT); // INITIALISATION : La broche 12 est maintenant une sortie
}

void loop() {
  /* 1. MESURE DE LA DISTANCE */
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long temps_AR_us = pulseIn(ECHO, HIGH, MEASURE_TIMEOUT);
  float distance_m = (temps_AR_us / 2.0) * (vitesse_son / 1000000.0);
  int distance_cm = distance_m * 100;

  /* 2. LOGIQUE DE DÉCISION */

  if (distance_m > 0.50 || distance_m <= 0) {
    // ZONE SÉCURITÉ : LED Verte allumée, le reste éteint
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_JAUNE, LOW);
    digitalWrite(LED_ROUGE, LOW);
    noTone(BUZZER);
  } 
  else if (distance_m <= 0.50 && distance_m > 0.20) {
    // ZONE ATTENTION : LED Jaune allumée, le reste éteint
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_JAUNE, HIGH);
    digitalWrite(LED_ROUGE, LOW);
    noTone(BUZZER);
  } 
  else {
    // ZONE DANGER (Moins de 20 cm)
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_JAUNE, LOW);

    if (distance_cm <= 5) {
      // SI TRÈS PROCHE : Le son et la lumière stagnent (fixe)
      digitalWrite(LED_ROUGE, HIGH);
      tone(BUZZER, 2000); // Son continu aigu
    } 
    else {
      // ENTRE 5cm et 20cm : CLIGNOTEMENT VARIABLE
      // Plus distance_cm est petit, plus delaiVitesse est court (donc rapide)
      int delaiVitesse = map(distance_cm, 5, 20, 40, 250);
      
      // PHASE ALLUMÉE (Lumière + Son)
      digitalWrite(LED_ROUGE, HIGH);
      tone(BUZZER, 1500); // Bip à 1500Hz
      delay(delaiVitesse); // Attend selon la distance
      
      // PHASE ÉTEINTE
      digitalWrite(LED_ROUGE, LOW);
      noTone(BUZZER);
      delay(delaiVitesse); // Attend selon la distance
    }
  }

  /* 3. AFFICHAGE POUR CONTRÔLE */
  Serial.print("Distance radar : ");
  Serial.print(distance_cm);
  Serial.println(" cm");
}