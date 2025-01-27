# Système de Contrôle de Véhicule avec nRF24L01, Servo et DAC MCP4725

Ce projet implémente un système de contrôle à distance pour un véhicule équipé d'un moteur, d'un servo-moteur pour la direction, et d'un module de communication radio nRF24L01. Le projet utilise des bibliothèques spécifiques pour le contrôle des différents composants matériels.

---

## Fonctionnalités Principales

- **Contrôle de la direction** : Utilisation d'un servo-moteur pour orienter les roues du véhicule (gauche, droite ou tout droit).
- **Contrôle de la vitesse** : Utilisation d'un convertisseur numérique-analogique (DAC) MCP4725 pour ajuster la vitesse et la direction du moteur.
- **Communication sans fil** : Réception des commandes via un module nRF24L01.
- **Gestion des commandes** : Commandes supportées : Arrêt, Avancer, Reculer, Gauche, Droite.
- **Débogage** : Utilisation d'une LED interne pour indiquer la réception des commandes.

---

## Matériel Requis

1. **Microcontrôleur** : Compatible avec Arduino.
2. **Servo-moteur** : Pour la direction.
3. **Module DAC MCP4725** : Pour le contrôle analogique de la vitesse.
4. **Module Radio nRF24L01** : Pour la communication sans fil.
5. **SyRen10** : Contrôleur de moteur.
6. **LED** : Pour les indications visuelles.
7. **Divers câblages** : Pour connecter les composants.

---

## Brochage

| Composant          | Pin Microcontrôleur | Description                       |
|---------------------|---------------------|-----------------------------------|
| **Servo-moteur**    | D3                  | Contrôle de la direction          |
| **Moteur (S2)**     | D1                  | Contrôle du sens                  |
| **nRF24L01**        | D2 (CE), D7 (CSN)  | Communication sans fil            |
| **IRQ (nRF24L01)**  | D6                  | Interruption                      |
| **DAC MCP4725**     | I2C (SDA/SCL)       | Contrôle de la vitesse            |
| **LED Interne**     | D13                 | Indication de commande reçue      |

---

## Dépendances

Les bibliothèques suivantes doivent être installées pour utiliser ce code :

1. [Adafruit_MCP4725](https://github.com/adafruit/Adafruit_MCP4725)
2. [Servo](https://github.com/arduino-libraries/Servo)
3. [RF24](https://github.com/nRF24/RF24)
4. [Wire](https://www.arduino.cc/en/Reference/Wire)

---

## Installation

1. **Préparation** : Assurez-vous que votre environnement Arduino est configuré et que toutes les bibliothèques nécessaires sont installées.
2. **Câblage** : Connectez les composants matériels selon le tableau de brochage ci-dessus.
3. **Téléversement** : Téléversez le code sur votre microcontrôleur via l'IDE Arduino.

---

## Fonctionnement

1. **Initialisation** :
   - Le servo est calibré pour la position centrale (tout droit).
   - Le module DAC est configuré pour initialiser le moteur à l'arrêt.
   - Le module nRF24L01 est configuré pour recevoir des commandes sur un canal défini.

2. **Réception des commandes** :
   - Les commandes sont envoyées via le module nRF24L01 et captées par le récepteur.
   - Une interruption se déclenche pour traiter la commande reçue.

3. **Exécution des commandes** :
   - Les commandes possibles incluent : `Stop`, `Gauche`, `Droite`, `Go_1` (Avancer), et `Back` (Reculer).
   - Chaque commande est exécutée en ajustant les composants concernés (servo pour la direction, DAC et SyRen10 pour la vitesse).

4. **Indication LED** :
   - La LED interne clignote pour indiquer la réception et l'exécution des commandes.

---

## Commandes Disponibles

| Commande   | Description                      |
|------------|----------------------------------|
| `Stop`     | Arrête le moteur                |
| `Gauche`   | Oriente le véhicule à gauche    |
| `Droite`   | Oriente le véhicule à droite    |
| `Go_1`     | Fait avancer le véhicule        |
| `Back`     | Fait reculer le véhicule        |

---

## Débogage

- Une LED interne (D13) clignote à chaque réception de commande.
- Les messages de débogage sont envoyés via le port série pour indiquer l'exécution des commandes.

---

## Avertissements

1. Assurez-vous que les connexions sont correctes avant de mettre sous tension.
2. Utilisez une alimentation adaptée pour le moteur et les composants électroniques.
3. Testez les commandes dans un environnement contrôlé pour éviter tout dommage matériel.

---

