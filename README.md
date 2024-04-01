# Simulateur de Fourmi en C
Ce projet est un simulateur de fourmi développé en langage C dans le cadre d'un projet SAÉ (Situation d'Apprentissage et d'Évaluation) en première année du B.U.T. (Bachelor Universitaire de Technologie) Génie Électrique et Informatique Industrielle (GEII) pour le semestre 1.

![image](https://github.com/barkayahmetcan/fourmi/assets/165664119/28094c52-31df-4f20-aa00-13a94cc464ff)

# Prérequis
Avant de pouvoir exécuter le simulateur de fourmi, assurez-vous d'avoir installé la bibliothèque SDL2.

# Compilation 
gcc fourmi.c -o main -lSDL2

# Modification/Ajout de carte
Pour créer une carte, vous devez utiliser un fichier bitmap.
La carte doit contenir un point bleu (RGB 0, 0, 255) pour indiquer la position de départ des fourmis et de la fourmilière.
De plus, la carte doit contenir des points rouges (RGB 255, 0, 0) pour représenter les emplacements où les fourmis peuvent trouver de la nourriture et la ramener à la fourmilière.
Il est également possible d'ajouter des points noirs (RGB 0, 0, 0) pour définir des obstacles que les fourmis devront contourner.
Une fois que vous avez terminé de créer votre carte, assurez-vous de la renommer en "carte.bmp", puis exécutez le programme.
PS : Si un fichier bitmap existe déja sous le nom "carte.bmp", vous pouvez juste le renommer temporairement.

# Plus d'information
Les fourmis cherchent de la nourriture (points rouges) et la ramènent à la fourmilière (point bleu).
Une fourmi de couleur vert foncé recherche de la nourriture.
Une fourmi de couleur vert clair transporte de la nourriture et la ramène à la fourmilière. De plus, elle dépose des points jaunes (les phéromones) pour attirer les autres fourmis vers la nourriture. Les phéromones disparait avec le temps.
Le "SCORE" affiché dans l'onglet de la simulation représente le nombre de portions de nourriture ramenées à la fourmilière.

Des fichiers PDF ont également été inclus, fournis par les professeurs pour ce sujet.
