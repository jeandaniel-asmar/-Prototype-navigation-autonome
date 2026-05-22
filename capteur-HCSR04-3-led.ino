//ETAPE 2 : debut du projet de detection d'obstacle avec 3 led(vert, jaune, rouge) selon la distance

/* PROJET : SYSTÈME D'AIDE AU STATIONNEMENT (ADAS)
  LE CAPTEUR à ultrasons HC-SR04 :
  4 pattes: 1 VCC (5V)     2 TRIG (entrée)     3 ECHO (sortie)     4 GND
  Le capteur envoie un train de 8 impulsions et mesure le temps mis par 
  l'onde ultrason (40kHz) pour faire l'aller-retour sur un obstacle.
  --> Une impulsion de 10µs sur TRIG déclenche la mesure.
  --> Le capteur renvoie sur ECHO une impulsion de largeur le temps mesuré.
*/

/* Constantes pour les broches du capteur */
const byte TRIG = 2;    // Patte TRIG sur broche 2
const byte ECHO = 3;    // Patte ECHO sur broche 3

/* Constantes pour les broches des LEDs */
const byte LED_VERTE = 5;
const byte LED_JAUNE = 6;
const byte LED_ROUGE = 7;

/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s

/* Vitesse du son dans l'air en m/s */
const float vitesse_son = 340.0;

void setup() {
  /* Initialisation du port série */
  Serial.begin(9600);
   
  /* Initialisation des broches du capteur */
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW); // Repos
  pinMode(ECHO, INPUT);

  /* Initialisation des broches des LEDs */
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
}

void loop() {
  
  /* 1. Lance une mesure (impulsion de 10µs sur TRIG) */
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
 
  /* 2. Mesure du temps de l'aller-retour en microsecondes */
  long temps_AR_us = pulseIn(ECHO, HIGH, MEASURE_TIMEOUT);
  
  /* 3. Calcul de la distance en mètres (float pour la précision) */
  float distance_m = (temps_AR_us / 2.0) * (vitesse_son / 1000000.0);
     
  /* 4. LOGIQUE DE DÉCISION (SÉCURITÉ VÉHICULE) */

  if (distance_m > 0.50 || distance_m <= 0) {
    // ZONE 1 : Sécurité (Obstacle à plus de 50cm ou rien du tout)
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_JAUNE, LOW);
    digitalWrite(LED_ROUGE, LOW);
  } 
  else if (distance_m <= 0.50 && distance_m > 0.20) {
    // ZONE 2 : Attention (Obstacle entre 20cm et 50cm)
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_JAUNE, HIGH);
    digitalWrite(LED_ROUGE, LOW);
  } 
  else if (distance_m <= 0.20) {
    // ZONE 3 : DANGER (Obstacle à moins de 20cm)
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_JAUNE, LOW);
    // Alerte visuelle critique : clignotement rapide
    digitalWrite(LED_ROUGE, HIGH);
    delay(50);
    digitalWrite(LED_ROUGE, LOW);
    delay(50);
  }

  /* 5. Affichage du résultat sur le moniteur série */
  Serial.print("Distance: ");
  Serial.print(distance_m);
  Serial.println(" m"); 
  
  /* On réduit le délai à 100ms pour plus de réactivité (important pour un véhicule !) */
  delay(100); 
}
