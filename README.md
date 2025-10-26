# Simulation d'un réseau Lan ![Statut](https://img.shields.io/badge/Statut-En_revue-blue)

![réseau](https://cabanisbrive.scenari-community.org/STIDD/Premiere/Sequence_03/Edp/Modelisation_Reseau_web/res/Reseau_switch.png)

## Description
Dans le cadre de la fin du BUT1, nous devions developper une simulation d'un réseau LAN sur le terminal. Celui consiste à connecter plusieurs machines entre elles grâce au protocole STP à partir d'un fichier de configuration et d'envoyer des trames grâce au protocole Ethernet.

## Technologie utilisées
- Langage : C#
- IDE : Visual Studio Code

## Équipe
- Nombre de développeurs : 2
- Durée du projet : 4 semaines

## Installation et exécution
1. Cloner le dépôt :
    ```bash
    git clone https://github.com/mon-utilisateur/sae-simulation-reseau.git
    cd SAE-reseaux
2. Lancer le simulateur dans le terminal :
   ```bash
   make
   ./bin/main
   ````
## Fonctionnalités princiaples
- Structures du réseau avec station et switch
- Stoackage d'une architecture du réseau dans un fichier de configuration
- Communication de trames Ethernet
- Option d'ajout du protocole STP (Spanning Tree Protocol)
  
/!\ envoie trame ne marche pas lors d'une boucle (en cours de correction)
