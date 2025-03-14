#include <Adafruit_MCP4725.h>   // Bibliothèque pour le DAC MCP4725
#include <Servo.h>              // Bibliothèque pour contrôler les servos
#include "RF24.h"               // Bibliothèque pour la communication RF24

//============[ Configuration des broches ] ==================================================

#define D_PIN 1                 // Contrôle du sens de rotation du moteur (S2 du SyRen10)

#define servoPin1 3             // Servo-moteur pour la direction

#define CE_PIN 2                // CE pour la communication radio nRF24L01 (PIN D2)
#define CSN_PIN 7               // CSN pour la communication radio nRF24L01 (PIN D7)

#define IRQ_PIN 6               // Broche d'interruption pour la radio (PIN D6)
#define DAC_ADDR 0x60           // Adresse I2C du MCP4725

#define LED_PIN 13              // Pin de la LED interne pour debogage

//============[ Déclaration des Objets ] =====================================================

Servo myServo1;                 // Servo pour contrôler la direction
RF24 radio(CE_PIN, CSN_PIN);    // Objet pour la communication RF24
Adafruit_MCP4725 dac;           // Objet pour le DAC MCP4725

//============[ Variables ] ==================================================================

// Directions possibles pour le servo-moteur
int Direction[3] = {65, 87, 110};  // [Gauche, Tout droit, Droite]
int D = 1;                         // Direction actuelle (0 = Gauche, 1 = Tout droit, 2 = Droite)

// Vitesses (valeurs DAC pour MCP4725)
int stop = 0;                     // Arrêt (0V)
int Go = 500;                     // Vitesse lente (~0.6 V pour 5V d'alimentation)
int Back = 500;                   // Vitesse marche arriere (~0.6 V pour 5V d'alimentation)

// Commandes
const char* command_choose[5] = {"stop", "left", "right", "go", "down"};
volatile int commandIndex = 10;    // Index de la commande reçue
int commandToExecute;              // Commande à exécuter

// Radio
const byte address[6] = {0xAA, 0x44, 0x33, 0x22, 0x11};  // Adresse (0x11223344AA in little-endian format ) pour la communicationavec l'emetteur nRF24L01
const uint8_t channel = 52;                              // Canal de communication radio
const uint8_t payloadSize = 32;                          // Taille maximale du message reçu
char receivedData[payloadSize + 1];                      // Buffer pour le message reçu

// Drapeau d'interruption
volatile bool commandReceived = false;                   // Flag indiquant si une commande est reçue

//============[ Prototypes des fonctions ] ==================================================================================================
 
void handleRadioInterrupt();                            // Fonction pour gérer les interruptions radio
void executeCommand(int command);                       // Fonction pour exécuter une commande
void blinkLED(int times, int delayTime);                // Fonction pour faire clignoter la LED interne

//============[ Initialisation ] =============================================================

void setup() {

    // Configuration du servo
    myServo1.attach(servoPin1);                        // Associer le servo-moteur à la Pin D3 

    // Configuration de S2 (sens du moteur)
    pinMode(D_PIN, OUTPUT);                            // Contrôle du sens via S2 avec la pin D1
    digitalWrite(D_PIN, LOW);                         // Initialiser la direction


    // Initialisation du DAC
    if (!dac.begin(DAC_ADDR)) {
        Serial.println("Erreur : DAC non détecté !");
        while (1);
    }
    dac.setVoltage(stop, false);  

    // Initialisation du module radio
    radio.begin();                                  // Initialisation du module radio       
    radio.setChannel(channel);                      // Initalisation du channel de communication radio         
    radio.setPayloadSize(payloadSize);              // Initialisation de la taille du message a recevoir 
    radio.setDataRate(RF24_2MBPS);                  // Initialisation de la vitesse de transmission de 2 Mbps
    radio.setAutoAck(false);

    radio.openReadingPipe(1, address);              // Ouverture de la lecture sur l'adresse prédéfinis 
    radio.startListening();                         // Début de l'écoute 
    Serial.println("nRF24L01 Receiver Ready");

    // Interruption pour la réception radio
    pinMode(IRQ_PIN, INPUT_PULLUP);                 // Configurer la broche IRQ D6 comme une entrée 
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), handleRadioInterrupt, FALLING);
}

//============[ Boucle principale ] ==========================================================

void loop() {
  // Vérifie si une commande a été reçue
    if (commandReceived) {
        noInterrupts();                           // Désactiver les interruptions pour éviter les conflits
        commandToExecute = commandIndex;          // Stocker la commande à exécuter
        commandReceived = false;                  // Réinitialiser le drapeau
        interrupts();                             // Réactiver les interruptions

        blinkLED(2, 100);                         // Clignote deux fois pour indiquer qu'une commande a été reçue
        executeCommand(commandToExecute);         // Exécuter la commande reçue
    }
}

//============[ Fonctions ] =================================================================================================================

//============[ Gestion des interruptions ] ==================================================

void handleRadioInterrupt() {
    if (radio.available()) {                               // Vérifier si un message est disponible
        radio.read(&receivedData, payloadSize);            // Lire le message
        receivedData[payloadSize] = '\0';                  // Terminer la chaine 

        for (int i = 0; i < 5; i++) {                      // Identification de la commande
            if (strcmp(receivedData, command_choose[i]) == 0) {
                commandIndex = i;                           // Index de la commande valide
                commandReceived = true;                     // Lever le drapeau
                return;
            }
        }

        // Si aucune commande valide n'est reçue
        commandIndex = 10;
        commandReceived = true;                            // Indiquer tout de même qu'une commande a été reçue
    }
}

//============[ Exécution des commandes ] ====================================================

void executeCommand(int command) {
    switch (command) {
        case 0:                                          // [ Stop ]- - - - - - - - - - - - - -
            Serial.println("Executing: Stop");           // Annonce de la commande 
            dac.setVoltage(stop, false);                 // Envoie une tension de 0V au moteur (arrêt complet)
            break;

      case 1:                                          // [ Gauche ] - - - - - - - - - - - - -
           Serial.println("Executing: Gauche");         // Annonce de la commande "Gauche"
            if (D == 2) {                                // Si les roues sont actuellement à droite
                D = 1;                                   // Remet les roues en position centrale
            } else if (D == 1) {                         // Si les roues sont en position centrale
                D = 0;                                   // Oriente les roues vers la gauche
            }
            myServo1.write(Direction[D]);                // Envoie la position choisie au servo (angle correspondant)
            break;

        case 2:                                          // [ Droite ] - - - - - - - - - - - - -
            Serial.println("Executing: Droite");         // Annonce de la commande "Droite"
            if (D == 0) {                                // Si les roues sont actuellement à gauche
                D = 1;                                   // Remet les roues en position centrale
            } else if (D == 1) {                         // Si les roues sont en position centrale
                D = 2;                                   // Oriente les roues vers la droite
            }
            myServo1.write(Direction[D]);                // Envoie la position choisie au servo (angle correspondant)
            break;


       case 3:                                          // [ Go_1 ] - - - - - - - - - - - - - -
            Serial.println("Command: Go");              // Annonce de la commande "Avancer"
            digitalWrite(D_PIN, HIGH);                  // Configure la direction des moteurs pour avancer
            dac.setVoltage(Go, false);                  // Définir une vitesse lente (~0.6 V pour 5V d'alimentation)
            break;

        case 4:                                          // [ Back ] - - - - - - - - - - - - - -
            Serial.println("Command: Back");             // Annonce de la commande "Reculer"
            digitalWrite(D_PIN, LOW);                    // Configure la direction des moteurs pour reculer
            dac.setVoltage(Back, false);                 // Définit une vitesse lente pour reculer (~0.6V)
            break;

        default:                                         // [ Commande inconnue ] - - - - - - - -
            Serial.println("Command: Invalid");          // Avertit que la commande reçue est invalide
            dac.setVoltage(stop, false);                 // Par défaut, arrête le moteur pour sécurité
            break;
    }
}

// [ Fonction pour faire clignoter la LED interne (debogage)]
void blinkLED(int times, int delayTime) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delayTime);
        digitalWrite(LED_PIN, LOW);
        delay(delayTime);
    }
}