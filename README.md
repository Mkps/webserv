# Webserv - Serveur HTTP en C++

## Description
Webserv est un projet consistant à développer un serveur HTTP en C++ 98. Ce serveur permettra de comprendre les bases du protocole HTTP et de manipuler les requêtes et réponses HTTP. Le serveur doit être compatible avec les navigateurs web et répondre aux demandes conformément aux spécifications de HTTP/1.1.

## Fonctionnalités
- Serveur HTTP en C++ 98
- Support des méthodes HTTP GET, POST et DELETE
- Configuration via un fichier de configuration
- Support des sites web statiques
- Téléversement de fichiers
- Gestion de multiples ports
- Non-bloquant, utilisant `poll()` ou équivalent pour toutes les opérations I/O

## Prérequis
- C++98
- Aucun usage de bibliothèques externes ou Boost
- Utilisation des fonctions système autorisées telles que `execve`, `dup`, `pipe`, `socket`, `select`, `poll`, `epoll`, etc.

## Installation
Clonez le dépôt et compilez le projet avec le Makefile fourni.

```bash
git clone https://github.com/votre-utilisateur/webserv.git
cd webserv
make
```

## Utilisation

Pour lancer le serveur HTTP avec un fichier de configuration spécifique :

```bash
./webserv [configuration file]
```

Les configuration file sont dans le dossier : ```./test```

## Contributeurs

- Oussama Bouhlel
- Alexis Loubiere
- Yassine Zaoui