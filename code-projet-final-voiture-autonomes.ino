#include <Servo.h>

/*a) La Navigation (Ultrasons + Servo) : Évitement d'obstacles et gestion de la trajectoire.
  b) La Sécurité Périmétrique (PIR "la boule") : Détection thermique de piétons pour le freinage d'urgence.*/

/* --- CONFIGURATION DU SYSTÈME AUTONOME --- */

/* * PROJET : SYSTÈME DE VÉHICULE AUTONOME INTÉGRÉ
 * --------------------------------------------
 * Ce code gère la fusion de 3 systèmes de sécurité :
 * 1. RADAR DE RECUL (Ultrasons + LEDs + Buzzer)
 * 2. DIRECTION ASSISTÉE (Servomoteur proportionnel)
 * 3. DÉTECTION PIÉTON (Capteur thermique PIR)
 */

/* --- CONFIGURATION --- */
Servo monVolant;
const byte TRIG = 2;
const byte ECHO = 3;
const byte BUZZER = 12;
const byte PIN_PIETON = 10;
const byte LED_ROUGE = 7;
const byte LED_JAUNE = 6;
const byte LED_VERTE = 5;

// --- VARIABLES DE LISSAGE (Pour calmer le moteur) ---
long totalDistance = 0;
int moyenneDistance = 0;
const int nbLectures = 5; // On fait la moyenne sur 5 mesures rapides à la suite (nbLectures = 5).

void setup() {
  Serial.begin(9600); //Serial.begin(9600) : Ouvre le canal de communication pour lire les données sur l'ordinateur
  monVolant.attach(11);
  pinMode(TRIG, OUTPUT); //pinMode(...) : Définit si chaque broche doit envoyer du courant ou écouter un signal
  pinMode(PIN_PIETON, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(LED_VERTE, OUTPUT);
  
  monVolant.write(90); 
}

void loop() {
  // --- 1. FILTRE DE LISSAGE (Le "Calmant") ---
  totalDistance = 0;
  for (int i = 0; i < nbLectures; i++) {
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    totalDistance += pulseIn(ECHO, HIGH, 25000UL);
    delay(10); // Petite pause entre les lectures
  }
  
  // Calcul de la distance moyenne en cm du capteur de distance ultrasons
  // 340.0/10000.0, conversion directement du temps (en ms) en distance (cm).
  moyenneDistance = (totalDistance / nbLectures / 2.0) * (340.0 / 10000.0);// diviser par 2 car le son a fait un aller-retour 

  // 2. LOGIQUE DE DÉCISION AMÉLIORÉE 
  //utilisation de la fonction digitalRead pour lecture et communication avec arduino pour savoir s'il faut agir ou non

  if (digitalRead(PIN_PIETON) == HIGH)  //digitalRead(PIN_PIETON) == HIGH) permet de savoir si le détecteur de mouvement thermique a déclenché une alerte.
  {
    // URGENCE PIÉTON
    digitalWrite(LED_ROUGE, HIGH); // digitalwrite fonction pour l'action => Elle envoie un ordre
    // tone indispensable pour la régulation du buzzer, plus la fréquence est élevée, plus le son est aigu.
    tone(BUZZER, 2000);
    monVolant.write(90); //braquage du volant en cas d'extreme urgence (piéton détecter)
    Serial.println("URGENCE : PIETON DETECTE"); 
  } 
  else {
    noTone(BUZZER); // pas de danger donc buzzer inutile

    // ZONE VERTE : Sécurité totale (> 50 cm)
    // On a monté le seuil pour que le Jaune serve vraiment à quelque chose
    if (moyenneDistance > 50 || moyenneDistance <= 0) {
      digitalWrite(LED_VERTE, HIGH);
      digitalWrite(LED_JAUNE, LOW);
      digitalWrite(LED_ROUGE, LOW);
      monVolant.write(90); // Initialisation : Roues droites volant a 90 degres
    } 
    
    // ZONE JAUNE : Approche (Entre 25 cm et 60 cm)
    // Ici, le volant commence à tourner doucement
    else if (moyenneDistance <= 50 && moyenneDistance > 20) 
    {
      digitalWrite(LED_VERTE, LOW);
      digitalWrite(LED_JAUNE, HIGH);
      digitalWrite(LED_ROUGE, LOW);
      
      // La direction devient fluide et proportionnelle
      int angle = map(moyenneDistance, 20, 50, 45, 90);
      monVolant.write(angle); // initialisation et braquage selon la situation
    } 
    
    // ZONE ROUGE : Danger (< 25 cm)
    else 
    {
      digitalWrite(LED_VERTE, LOW);
      digitalWrite(LED_JAUNE, LOW);
      // ajout du BIP PERMANENT SI TRÈS PROCHE
      if (moyenneDistance <= 3) 
      {
        // LIGNE AJOUTÉE : À 3cm ou moins, on ne clignote plus
        digitalWrite(LED_ROUGE, HIGH); // LED fixe
        tone(BUZZER, 2000);            // Son continu (pas de noTone)
        Serial.println("DANGER : COLLISION PROCHE");
      }
      else{
      // Alerte sonore et visuelle
      int bipVitesse = map(moyenneDistance, 3, 20, 30, 250);
      digitalWrite(LED_ROUGE, HIGH);
      tone(BUZZER, 1500);
      delay(bipVitesse);
      digitalWrite(LED_ROUGE, LOW);
      noTone(BUZZER);
      delay(bipVitesse);
      }
      
      monVolant.write(20); // Braquage fort du servo moteur en cas de danger immédiat
    }
  }

  Serial.print("Distance : ");
  Serial.print(moyenneDistance);
  Serial.println(" cm");
}