//ETAPE 4 : ajout de la direction assisté avec servomoteur qui joue lr role du volant
//1. tout est ok alors volant se comporte normalement
//2. premiere detection d'obstacle ou de danger, leger braquage du volant 
//3. extreme danger ou colision proche braquage du net volant 

#include <Servo.h> // ÉTAPE 1 : On importe la boîte à outils "Servo" pour piloter le moteur

// --- CRÉATION DES OBJETS ---
Servo volant; // On crée un objet nommé "volant" qui représente ton moteur bleu

// --- CONFIGURATION DES BROCHES (PINS) ---
const byte TRIG = 2;       // Capteur Ultrason - Envoi
const byte ECHO = 3;       // Capteur Ultrason - Réception
const byte LED_VERTE = 5;  // Voyant "Route Libre"
const byte LED_JAUNE = 6;  // Voyant "Attention"
const byte LED_ROUGE = 7;  // Voyant "Danger" (branché sur 7)
const byte BUZZER = 12;    // Alerte Sonore (ton nouveau branchement)

// --- PARAMÈTRES TECHNIQUES ---
const unsigned long MEASURE_TIMEOUT = 25000UL;
const float vitesse_son = 340.0;

void setup() {
  Serial.begin(9600); // Pour voir les distances sur l'écran du PC
  
  // ÉTAPE 2 : INITIALISATION DU MOTEUR
  // On dit à l'Arduino que le fil orange du moteur est sur la broche 11
  volant.attach(11); 
  
  // INITIALISATION DES AUTRES COMPOSANTS
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  // SÉCURITÉ AU DÉMARRAGE :
  // On place le volant à 90 degrés (bien droit) pour que la voiture parte droite
  volant.write(90); 
}

void loop() {
  /* --- 1. MESURE DE LA DISTANCE --- */
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long temps_AR_us = pulseIn(ECHO, HIGH, MEASURE_TIMEOUT);
  float distance_m = (temps_AR_us / 2.0) * (vitesse_son / 1000000.0);
  int distance_cm = distance_m * 100;

  /* --- 2. LOGIQUE DE LA DIRECTION ASSISTÉE (ADAS) --- */

  // CAS A : LA ROUTE EST CLAIRE (Distance > 50 cm)
  if (distance_m > 0.50 || distance_m <= 0) {
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_JAUNE, LOW);
    digitalWrite(LED_ROUGE, LOW);
    noTone(BUZZER);
    
    /* DIRECTION ASSISTÉE : 
       Tout va bien, le système maintient la trajectoire rectiligne.
       On envoie l'angle 90° au servomoteur (position centrale). */
    volant.write(90); 
  } 
  
  // CAS B : OBSTACLE DÉTECTÉ (Distance entre 20 cm et 50 cm)
  else if (distance_m <= 0.50 && distance_m > 0.20) {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_JAUNE, HIGH); // On prévient le conducteur
    digitalWrite(LED_ROUGE, LOW);
    noTone(BUZZER);
    
    /* DIRECTION ASSISTÉE : 
       Le radar détecte une gêne. Le système commence une manœuvre d'évitement fluide.
       On tourne le volant à 45° pour dévier légèrement la trajectoire. */
    volant.write(45); 
  } 
  
  // CAS C : URGENCE CRITIQUE (Distance < 20 cm)
  else {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_JAUNE, LOW);
    
    /* DIRECTION ASSISTÉE : 
       DANGER IMMÉDIAT ! Le système prend le contrôle total pour éviter le crash.
       On braque le volant au maximum (angle 0°) pour un évitement d'urgence. */
    volant.write(0); 

    // GESTION DE L'ALERTE (LED + BUZZER)
    if (distance_cm <= 5) {
      // Si on est presque au contact, on ne clignote plus, on hurle !
      digitalWrite(LED_ROUGE, HIGH);
      tone(BUZZER, 2000); 
    } 
    else {
      // Sinon, on clignote et on bippe de plus en plus vite
      int delaiVitesse = map(distance_cm, 5, 20, 40, 250);
      digitalWrite(LED_ROUGE, HIGH);
      tone(BUZZER, 1500);
      delay(delaiVitesse); // On attend selon l'urgence
      digitalWrite(LED_ROUGE, LOW);
      noTone(BUZZER);
      delay(delaiVitesse); // On attend selon l'urgence
    }
  }

  // AFFICHAGE DE CONTRÔLE SUR LE PC
  Serial.print("Dist: "); Serial.print(distance_cm); Serial.println(" cm");
}