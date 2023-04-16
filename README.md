Projet : Création d'un logiciel de gestion de version
--- 

**Binôme (L2 Mono-Info S4 GR6) :**
 - PINHO FERNANDES Enzo : `21107465`
 - DURBIN Deniz Ali : `21111116`

--- 

Ce projet nous a été attribué en Structure de Données (LU2IN006), à Sorbonne Université Sciences (2022-2023)

--- 

Ce projet consiste à réaliser un logiciel de gestion de versions, c’est-à-dire un outil permettant de créer et
stocker différentes sauvegardes/versions d’un projet ou d’un ensemble de fichiers et répertoires.
Nous pourrons donc les visionner et effectuer des opérations dessus, comme accéder à l’historique des
changements effectués, revenir à une version antérieure s’il y a un problème, et même faciliter un travail
collaboratif où nous pourrions fusionner différentes versions du même projet tout en gérant les conflits
potentiels.

Il y a donc une utilité non négligeable pour les grands projets qui, à cause de nombreuses modifications
apportées, demandent une trace de chaque version. D’autant plus pour les projets collaboratifs où chaque
personne aura ses propres versions qu’il faudra réunir en une seule.
Nous pouvons prendre comme exemple de comparaison Git, qui est lui aussi un logiciel de gestion de
versions, gratuit, et créé en 2005 par Linus Torvalds, alias le créateur du noyau Linux.

---

## Utilisation de myGit

Pour commencer, il faut bien évidemment utiliser la commande `make` pour compiler `myGit.c`.
C’est optionnel mais vous pouvez rajouter le programme dans votre PATH. Placez vous de préférence dans la racine de votre projet, et voici les différentes commandes que vous pouvez utiliser grâce à `myGit`.

\
**`myGit init`**
- Initialise le répertoire de références et la branche courante. Ce sera la première commande à lancer pour la première fois. Elle crée le dossier “.refs”, ainsi que ses fichiers “HEAD” et “master”. Elle crée également le fichier “.current_branch” stockant la branche courante.

\
**`myGit list-refs`**
- Affiche toutes les références existantes.

\
**`myGit create-ref <name> <hash>`**
- Crée la référence nommée <name> qui pointe vers le commit correspondant à <hash>.

\
**`myGit delete-ref <name>`**
- Supprime la référence <name>.
- Dans ce cas, il faut faire attention. L’utilisateur, s’il supprime sa branche courante, devrait
penser à faire un “checkout-branch” vers la branche qu’il souhaite afin de changer de branche
courante.

\
**`myGit add <elem> [<elem2> <elem3>...]`**
- Ajoute AU MOINS un fichier (elem obligatoirement, elem2, elem3...) à la zone de préparation pour faire partie du prochain commit.

\
**`myGit list-add`**
- Affiche le contenu de la zone de préparation.

\
**`myGit clear-add`**
- Vide la zone de préparation.

\
**`myGit commit <branch_name> [-m <message>]`**
- Effectue un commit sur une branche nommée <branch_name>, avec ou sans message
descriptif.

\
**`myGit myGit get-current-branch`**
- Affiche le nom de la branche courante.

\
**`myGit branch <branch-name>`**
- Crée une branche qui se nomme <branch-name> si elle n’existe pas déjà..

\
**`myGit branch-print <branch-name>`**
- Affiche le hash de tous les commits de la branche <branch-name>, accompagné de leur
message descriptif éventuel.

\
**`myGit checkout-branch <branch-name>`**
- Réalise un déplacement sur la branche <branch-name> si elle existe.

\
**`myGit checkout-commit <pattern>`**
- Réalise un déplacement sur le commit qui commence par <pattern> s’il existe et est le seul.

\
**`myGit merge <branch> <message>`**
- S’il y a pas de collision entre la branche courante et <branch>, on réalise la fusion entre les
deux branches. 
- Sinon, l’utilisateur choisira entre : garder les fichiers/répertoires de la branche
courante, ceux de la branche <branch>, ou choisir manuellement conflit par conflit les
fichiers/répertoires concernés..