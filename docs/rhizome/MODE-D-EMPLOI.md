# Rhizome : mode d'emploi

Rhizome est un firmware pour le M-VAVE FM-1, écrit pour **When Day Chokes The Night**. Il fait l'espace et le temps :
- un **séquenceur orbital**, où des corps tournent autour d'une étoile et jouent quand ils passent leurs portes ;
- une **bande** à trois têtes ;
- une **réverbe HALL** ;
- un **granulaire Beads**.

Il est complémentaire de Monades (le premier FM-1, qui fait les notes) et joue aussi très bien seul.

> **État au 2 novembre 2026.** Rhizome tourne dans le simulateur. **Il n'a pas encore été compilé ni mesuré sur un FM-1** : ne l'installez pas sur votre appareil avant que la charge du processeur ait été vérifiée (voir « Points de vigilance »).

Rhizome repart du code de **Monades 0.1.3**. Tout ce que Monades sait faire est encore là : voix, filtres, rythmes, phrases, générateurs, gammes, MIDI, sauvegardes. Ce mode d'emploi décrit ce que Rhizome ajoute ou change. Pour les pages héritées, voir le [mode d'emploi de Monades](https://dr-john-8bits.github.io/rhizome/monades/manuel/).

## Où trouver quoi

Les boutons du FM-1 portent les noms du firmware d'origine de M-VAVE. Cinq boutons ouvrent les pages : HOME, EDIT, SEQ, FX, GLO. Appuyer plusieurs fois sur le même bouton fait défiler ses pages. Sur chaque page, les potards K1 à K4 règlent ce que montre le bandeau du bas.

| Je cherche | Appuyer sur | Page |
|---|---|---|
| Les orbites d'une piste | SEQ 1 fois | ORBIT |
| La précession, la perturbation, la source des notes, un système réel | SEQ 2 fois | ORBIT / SYSTEM |
| Les lunes (ornements) | SEQ 3 fois | ORBIT / MOONS |
| La comète, les conjonctions | SEQ 4 fois | ORBIT / COMET |
| Le delay à bande | FX 1 fois | TAPE ECHO |
| La saturation, l'usure, FREEZE et STOP de la bande | FX 2 fois | TAPE ECHO / AGE |
| La réverbe | FX 3 fois | HALL |
| Les graves, les aigus, la diffusion, le mode vintage de la réverbe | FX 4 fois | HALL / COLOR |
| Le niveau, la taille, la position et le retour des grains | FX 5 fois | GRANULAR BUS |
| La densité, la forme, le hasard et le média des grains | FX 6 fois | BEADS |
| La hauteur et le sens des grains | FX 7 fois | TAPE / MOTION |
| Le retour commun des effets (bande et réverbe) | FX 8 fois, K4 | SHARED SPACE |
| Ce qu'une piste envoie aux grains, à la bande, à la réverbe | EDIT 5 fois : K2, K3, K4 | GLOBAL MIXER |
| Figer la mémoire des grains | LFO (le bouton) | |
| Le tempo, la gamme, la note de base | GLO 1 fois | HARMONY |

## Les pages, bouton par bouton

| Bouton | Pages, dans l'ordre |
|---|---|
| HOME | LIVE MONDE → MORPH → DERIVE |
| EDIT | SOUND → VELVET FILTER → ENVELOPE → MIX → GLOBAL MIXER |
| SEQ | **ORBIT → ORBIT / SYSTEM → ORBIT / MOONS → ORBIT / COMET** → RHYTHM → GROOVE → BASS / MOTIFS → PHRASE → GENERATOR → FORM |
| FX | **TAPE ECHO → TAPE ECHO / AGE → HALL → HALL / COLOR** → GRANULAR BUS → **BEADS** → TAPE / MOTION → SHARED SPACE → SPACE TONE → ECHO SYNC / CHORUS → MARBLES / TIDES / WARPS → LFO A → LFO B |
| GLO | HARMONY → RAGA / GAMAKA → SYSTEM → LOOK → MIDI → MIDI CHANNELS → MIDI OUT ZONES → USER BANKS |

Les pages en gras sont celles de Rhizome.

Les autres commandes ne changent pas :
- **SELECT** choisit la piste (1 à 6) ;
- **SEL** coupe ou rétablit la piste choisie ;
- **PLAY** lance ou arrête ;
- **ENV** annule ;
- **PRESET** change de Monde ;
- maintenir un bouton de page ouvre l'aide de la page affichée.

Deux différences :
- sur les pages de Rhizome, **REC** ne tire pas la page au sort : il donne une nouvelle variation, comme sur LIVE ;
- le bouton **LFO** fige la mémoire de Beads.

## Premiers pas : faire tourner des orbites

1. Choisir un Monde avec PRESET, par exemple le 61 (Quiet Lanterns).
2. Appuyer sur **SEQ** : la page **ORBIT** s'ouvre. Tourner **K3 GATES** : la piste choisie devient un corps qui tourne autour de l'étoile, avec autant de portes que le réglage.
3. Avec SELECT, passer à une autre piste et lui donner aussi des portes, avec une autre période (**K1**).
4. Appuyer sur **PLAY**. Tous les corps partent du haut, ensemble.
5. Ovaliser une orbite avec **K2 ECCENTRICITY** : le corps accélère près de l'étoile et ralentit loin d'elle. On entend un rubato qui revient à chaque tour.

Plus rapide encore : **SEQ 2 fois**, puis **K4 SYSTEM**, pour charger un vrai système planétaire (voir plus bas).

## Le séquenceur orbital

### L'idée, en partant d'un séquenceur pas à pas
Un séquenceur pas à pas fait tourner une boucle de pas, et certains pas jouent. Le séquenceur orbital fait la même chose, mais la boucle est **une orbite**, et la piste est **un corps** qui en fait le tour.

| Sur un séquenceur pas à pas | Sur le séquenceur orbital |
|---|---|
| La longueur de la boucle (le nombre de pas) | **PERIOD** (K1) : la durée d'un tour, de 1 temps à 12 mesures |
| Les pas qui jouent | **GATES** (K3) : de 1 à 16 portes par tour, réparties le plus régulièrement possible |
| Décaler le motif | **PHASE** (K4) : faire tourner les portes |
| Le swing, le même petit retard à chaque contretemps | **ECCENTRICITY** (K2) : le tour garde sa durée, mais les notes se resserrent d'un côté de l'orbite et s'étirent de l'autre |
| Une piste, une boucle | Une piste, un corps. Plusieurs corps aux périodes différentes font une polyrythmie qui se réaligne |

Concrètement :
- **Le nombre de pas d'un tour**, c'est PERIOD. Ses 13 valeurs, en doubles croches : 4 (1 temps), 6, 8, 12, 16 (1 mesure), 24, 32, 48, 64, 96, 112, 128 et 192 (12 mesures).
- **Le nombre de notes par tour**, c'est GATES, de 1 à 16. Il ne peut pas dépasser le nombre de pas du tour : un tour d'un temps compte 4 pas, donc 4 portes au plus. À 0, la piste n'est plus un corps et reprend son rythme de la page RHYTHM.
- **Les portes sont placées comme un rythme euclidien** : réparties le plus régulièrement possible sur les pas du tour. Par exemple, 5 portes sur un tour d'une mesure (16 pas) tombent sur les pas 1, 5, 8, 11 et 14 : c'est le rythme euclidien 5 sur 16. Avec 4 portes, on obtient quatre noires.

### L'étoile
L'étoile ne joue pas. C'est le centre de gravité : **tout se mesure par rapport à elle**.
- **Les corps tournent autour d'elle.** Sur une orbite ovale, un corps va plus vite près de l'étoile et plus lentement loin d'elle (la deuxième loi de Kepler). D'où le rubato.
- **Les portes sont des directions vues depuis l'étoile**, comme les heures d'un cadran : un corps joue quand il passe devant l'une d'elles.
- **La distance à l'étoile peut donner la note.** Avec la source ORBIT, plus le corps est près, plus la note est aiguë.
- **Les événements se passent par rapport à elle.** Une conjonction, c'est deux corps alignés avec l'étoile ; la comète agit en la frôlant.
- **À l'écran, elle brille** avec le niveau de sortie.

### Rond, puis ovale
- **Une orbite ronde** : le corps tourne à vitesse constante et passe ses portes à intervalles réguliers. C'est exactement le rythme euclidien.
- **Une orbite ovale** : il passe toujours le même nombre de portes en un tour de la même durée, donc la boucle reste calée sur la mesure. Mais il les passe vite du côté de l'étoile et lentement de l'autre. Les notes se resserrent puis s'espacent, comme un musicien qui presse puis retient : un rubato qui revient à chaque tour.
- **Avec la précession**, l'ovale tourne lentement sur lui-même. Le moment où les notes se resserrent glisse alors peu à peu par rapport à la mesure, et la phrase change sans hasard.

### Plusieurs corps
Chaque piste a sa propre orbite. Avec des périodes différentes, les corps se retrouvent tous les N tours. Par exemple, un tour de 3 mesures et un tour de 4 mesures se réalignent toutes les 12 mesures. Ce sont les rapports que montre la carte de PERIOD (par exemple 3:4:6:8), et c'est le principe des systèmes réels de K4 SYSTEM.

Le mouvement suit le tempo, le swing et l'horloge MIDI reçue. PLAY fait repartir tous les corps du haut.

### Ce que montre l'écran
- **Les ellipses** : leur vraie forme et leur vraie orientation. Celle de la piste choisie est plus vive ; une piste sans porte est en pointillé.
- **Les portes** : de petits traits sur les ellipses. Une porte devient blanche quand elle joue, et grise quand la perturbation l'a refusée.
- **Les corps et leurs traînées** : **la longueur de la traînée est la vitesse**, donc on voit le rubato.
- **Une onde** autour du corps à chaque note.
- **L'anneau extérieur** : la mesure, avec un point sur le temps en cours.
- **Un trait blanc** depuis l'étoile quand deux corps s'alignent (une conjonction).
- **Un pointillé** entre deux corps quand l'un fait écho à l'autre (ÉCHO).
- **L'étoile** brille avec le niveau de sortie.

Quand on tourne un potard, une carte apparaît sur la moitié de l'écran où le corps n'est pas : le nom du réglage, sa valeur et ce qu'il fait.

### ORBIT (piste choisie)
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | PERIOD | La durée d'un tour : de 1 temps à 12 mesures, en 13 valeurs. La carte donne les rapports de toutes les orbites, par exemple 3:4:6:8. |
| K2 | ECCENTRICITY | De rond (0) à très ovale (0,85). La carte dit combien de fois plus vite le corps va près de l'étoile. |
| K3 | GATES | Le nombre de portes par tour, de 0 (orbite coupée) à 16. |
| K4 | PHASE | Fait tourner les portes par pas de 15°. |

### ORBIT / SYSTEM
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | PRECESSION | Toutes les ellipses tournent lentement sur elles-mêmes. Le rubato glisse alors par rapport à la mesure, et la phrase change sans hasard. 100 = un tour en 16 mesures. |
| K2 | PERTURBATION | Le système vit : les orbites respirent, des portes sont refusées, des notes voisines s'invitent. 0 = une horloge parfaite. |
| K3 | PITCH | D'où viennent les notes de la piste choisie (voir ci-dessous). |
| K4 | SYSTEM | Charge un système planétaire réel, ou affiche OWN pour vos propres orbites. |

**Les trois sources de notes (K3 PITCH)** :
- **ORBIT** : plus le corps est près de l'étoile, plus la note est aiguë. L'ellipse dessine donc une mélodie qui monte et descend, dans la gamme choisie sur HARMONY.
- **MOTIF** : le corps joue, porte après porte, les notes écrites au clavier sur la page GENERATOR (comme le MOTIF de Monades). **Sur la page ORBIT, chaque porte que le corps va franchir affiche la note qu'elle jouera**, la prochaine en blanc.
- **ECHO** : le corps reprend la dernière note de la piste précédente, ramenée à la note de l'accord de la tonique juste au-dessus (l'idée du tintinnabuli d'Arvo Pärt).

**Les systèmes réels (K4 SYSTEM)** : leurs périodes et excentricités sont mesurées, ramenées aux valeurs les plus proches du séquenceur sans changer les rapports. Chaque corps a 4 portes, donc les rapports s'entendent comme une polyrythmie.

| Système | Corps | Rapports |
|---|---|---|
| JUPITER | Io, Europe, Ganymède, Callisto | 1:2:4, puis 9,3 |
| KEPLER-223 | quatre planètes | 3:4:6:8 |
| TRAPPIST-1 | les planètes e, f, g, h | 2:3:4:6 |
| PLUTO | Neptune, Pluton (et Charon, sa lune) | 3 tours contre 2 |

Comment ça se charge :
- les corps vont d'abord sur les pistes mélodiques non coupées ;
- le son, la gamme, la comète et les conjonctions ne changent pas ;
- dès qu'on touche une orbite, le système redevient OWN ;
- **ENV** revient aux orbites d'avant le premier système chargé.

### ORBIT / MOONS (piste choisie) : les lunes
Chaque note du corps fait faire un tour à ses lunes autour de lui. Une lune joue un ornement quand elle passe du côté du corps opposé à l'étoile. Au repos, les lunes attendent régulièrement espacées, donc les ornements partagent le tour en parts égales après la note.

| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | MOONS | De 0 à 3 lunes. |
| K2 | SPIN | Le temps d'un tour, de 1/16 à une mesure. La carte dit l'écart entre deux ornements. |
| K3 | INTERVAL | Ce que jouent les lunes : REPEAT (la même note), STEPS UP ou DOWN (la gamme note par note), THIRDS UP ou DOWN (par tierces), HARMONICS (l'octave, l'octave et la quinte, deux octaves). Sur une piste de batterie, la lune rejoue le même coup. |
| K4 | LEVEL | Le volume des lunes, en pourcentage de celui de la note. |

Sur cette page, le corps choisi est **en gros plan** : l'étoile est en bas, le point où les lunes jouent est marqué en haut, et chaque lune porte le nom de la note qu'elle jouera. Sur les autres pages, les lunes sont de petits points autour des corps.

Bon à savoir : les ornements partent aussi en MIDI, et l'ÉCHO reprend la note du corps, pas ses ornements.

### ORBIT / COMET (tout le système) : la comète et les conjonctions
La **comète** est un corps sans voix, sur une orbite très allongée. Elle passe presque tout son tour loin de l'étoile, puis la frôle. PLAY la fait partir de loin, donc son premier passage arrive à la moitié du tour. Son passage agit pendant à peu près un cinquième du tour, le plus fort au plus près de l'étoile.

| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | COMET | Un tour en 4, 8, 16, 32 ou 64 mesures, ou OFF. La carte dit dans combien de mesures elle repassera. |
| K2 | EVENT | SWELL (la réverbe gonfle : envoi plus fort, queue plus longue, retour plus fort), FREEZE (la mémoire des grains se fige au cœur du passage) ou BOTH. |
| K3 | AMOUNT | La force du passage. |
| K4 | CONJUNCTION | Ce que fait l'alignement de deux corps avec l'étoile : OFF (rien), ACCENT (leur temps suivant est accentué) ou CHORD (les deux rejouent ensemble ; le second prend la note de l'accord du premier la plus proche de la sienne). |

À l'écran :
- **la queue de la comète pointe à l'opposé de l'étoile**, et sa longueur montre la force du passage ;
- quand la comète est hors de l'écran, une marque sur l'anneau suit sa direction et s'éclaire à son approche.

## La bande : TAPE ECHO

Le delay est une boucle de bande qui défile sous une tête d'enregistrement et trois têtes de lecture, placées à 1/3, 2/3 et au bout du trajet. **TIME règle la vitesse de la bande**, pour que la tête la plus lointaine en service réponde après ce temps :
- **bande rapide** (jusqu'à 12 fois plus vite) : échos courts et nets ;
- **bande lente** (jusqu'à moitié moins vite) : échos longs, plus sombres et plus granuleux, jusqu'à 2,86 s.

La vitesse a de l'inertie : changer le temps fait glisser la hauteur, comme sur une vraie machine.

### TAPE ECHO
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | TIME | Le temps de la tête la plus lointaine en service. Si l'écho est synchronisé au tempo (page ECHO SYNC / CHORUS, K1), K1 choisit la division. |
| K2 | FEEDBACK | Combien de fois l'écho se répète. |
| K3 | HEADS | Les têtes en service : 1, 2, 3 ou leurs combinaisons. |
| K4 | WOW | Le wow, le flutter et la dérive de la vitesse. |

### TAPE ECHO / AGE
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | DRIVE | La saturation de la tête d'enregistrement. |
| K2 | AGE | L'usure : souffle (seulement sous ce qui est enregistré), aigus perdus, chutes de niveau. |
| K3 | TONE | Chaque répétition un peu plus sombre, ou beaucoup. |
| K4 | TRANSPORT | RUN, **FREEZE** (la tête d'enregistrement se lève, la boucle se répète) ou **STOP** (la bande freine et le son plonge ; RUN la relance). |

À l'écran :
- les **bobines** tournent à la vitesse de la bande, ralenties à l'affichage pour que les rayons ne semblent jamais tourner à l'envers ;
- le **bras tendeur** bouge avec le wow ;
- **ce que la tête d'enregistrement écrit est dessiné sur la bande et avance vers les têtes de lecture** : on voit l'écho arriver avant de l'entendre ;
- chaque tête en service affiche son temps et devient blanche quand elle rejoue ;
- sous FREEZE, la tête d'enregistrement se lève.

## La réverbe : HALL

Une réverbe de la famille des grandes réverbes « hall » numériques, celle dont le Lexicon 224 est issu. Elle reprend les longueurs de la réverbe de Rings et Clouds de Mutable Instruments.

### HALL
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | DECAY | Le temps pour perdre 60 dB, de 0,5 à 20 s, puis **INFINITE** : la queue tient et plus rien n'entre, comme une nappe figée. |
| K2 | SIZE | La taille de la salle (la carte donne la durée d'un tour de la boucle). |
| K3 | PRE-DELAY | Le temps avant que la salle réponde, de 0 à 250 ms. |
| K4 | MOD | Un lent chatoiement dans la queue. |

### HALL / COLOR
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | BASS | Les graves durent de 0,5 à 2 fois plus longtemps que les médiums. |
| K2 | TREBLE | Plus bas, les aigus s'éteignent plus vite. |
| K3 | DIFFUSION | Des échos plus distincts au début de la queue, ou un début plus doux et plus dense. |
| K4 | VINTAGE | La réverbe tourne à 22 kHz, comme les réverbes numériques d'époque : plus sombre, deux fois plus grande, deux fois moins de calcul. |

À l'écran :
- **trois droites** montrent en combien de temps les graves, les médiums et les aigus perdent 60 dB après le pré-délai. En INFINITE, elles sont plates ;
- **sur chaque droite, un point** donne l'énergie réelle de la queue dans cette bande, et glisse le long de la droite à mesure que le son s'éteint.

Les réglages DECAY, TREBLE et DIFFUSION sont aussi sur les pages SHARED SPACE et SPACE TONE, héritées de Monades.

## Les grains : Beads

Le granulaire est celui du module Beads de Mutable Instruments, réduit pour le FM-1 : 6 grains au plus et 128 Ko de mémoire, en mono. Les pistes y envoient du son (GLOBAL MIXER, K2), et SPACE règle le niveau des grains (LIVE K4, ou GRANULAR BUS K1). **Quand SPACE est à 0, Beads se repose et ne calcule rien.**

### GRANULAR BUS
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | SPACE | Le niveau des grains. |
| K2 | GRAIN SIZE | La longueur des grains. |
| K3 | POSITION | Où, dans la mémoire, naissent les nouveaux grains. |
| K4 | FEEDBACK | Les grains réinjectés dans la mémoire. |

### BEADS
| Potard | Réglage | Ce qu'il fait |
|---|---|---|
| K1 | DENSITY | **Au milieu, aucun grain.** À gauche, des grains réguliers, de plus en plus fréquents. À droite, des grains au hasard, de plus en plus nombreux. La carte donne le nombre de grains par seconde. |
| K2 | SHAPE | L'enveloppe de chaque grain : carrée, puis décroissante, puis en cloche, puis inversée. Elle est dessinée à l'écran. |
| K3 | RANDOM | Fait varier la position, la taille, la forme et la hauteur d'un grain à l'autre. |
| K4 | MEDIA | Le grain de la mémoire et sa durée : DIGITAL (16 bits, 1,4 s), VINTAGE (12 bits, 1,9 s), CLEAN TAPE (12 bits à demi-fréquence, 3,8 s), DIRTY TAPE (μ-law à demi-fréquence, avec du wow, 5,8 s). **Changer de média vide la mémoire**, comme sur le module. |

### TAPE / MOTION
- **K1 PITCH** : la hauteur des grains, de −24 à +24 demi-tons.
- **K2 DIRECTION** : à l'endroit, ou à l'envers (Beads joue alors ses grains à rebours).

### Ce que montrent GRANULAR BUS et BEADS
- La mémoire, avec le présent à droite et toute sa durée vers la gauche.
- Le niveau écrit le long de la mémoire.
- Chaque grain vivant, sous forme d'une bande qui part de l'endroit où il lit, avec un trait blanc à ce point.
- Le crochet de POSITION, et sous lui le trait du feedback.

Le bouton **LFO** fige la mémoire, et la comète peut la figer à son passage.

## Le trajet du son : envois et retours

- Chaque piste envoie une part de son son vers trois destinations, réglées sur **GLOBAL MIXER** (EDIT 5 fois) : **K2** vers les grains, **K3** vers la bande, **K4** vers la réverbe.
- Le niveau des grains est **SPACE**. Celui de la bande et de la réverbe est le retour commun, **K4 RETURN sur SHARED SPACE**.
- Quand on n'entend pas un effet, sa page le dit en haut de l'écran, par exemple « RETURN AT 0 » ou « NO SEND ».

## Recettes

- **Une nappe qui ne finit pas** : HALL K1 DECAY tout en haut (INFINITE) au moment voulu. La queue tient, le reste continue par-dessus. On redescend DECAY pour la relâcher.
- **Un écho qui devient une bande lointaine** : TAPE ECHO, K3 HEADS sur 1+2+3, K1 TIME vers 2 s (la bande ralentit et s'assombrit), K2 FEEDBACK vers 60, puis AGE à 30 et DRIVE à 30.
- **Le geste de la bande qui s'arrête** : TAPE ECHO / AGE, K4 sur STOP en fin de phrase, puis RUN.
- **Un ciel qui tourne lentement** : SEQ 2 fois, K4 SYSTEM sur KEPLER-223, puis PRECESSION à 20 et PERTURBATION à 10.
- **Un rare événement** : SEQ 4 fois, COMET sur 16 mesures, EVENT sur BOTH. Toutes les 16 mesures, la salle gonfle et les grains se figent au passage.
- **Des grains qui scintillent au-dessus** : TAPE / MOTION K1 PITCH à +12, BEADS K1 DENSITY vers 85, K2 SHAPE en cloche, K4 MEDIA sur CLEAN TAPE.
- **Des ornements en harmoniques** : ORBIT / MOONS, 2 lunes, SPIN 1/4, INTERVAL sur HARMONICS, LEVEL à 50.

## Le simulateur

Le simulateur fait tourner le vrai firmware de Rhizome dans le navigateur, avec l'écran du FM-1, ses boutons, ses potards et son clavier. Les sauvegardes du navigateur sont séparées de celles d'un FM-1.

- **Souris** : cliquer sur un bouton ; faire glisser un potard vers le haut ou le bas, ou utiliser la molette.
- **Clavier de l'ordinateur** : les touches Q W E R T Y U I et A S D F G H J K jouent les touches blanches ; 1 à 0 et le tiret jouent les noires. P = PLAY, O = REC, Échap = HOME, Z et X = OCT− et OCT+.
- **Tenir un bouton** (pour ouvrir l'aide d'une page) : Maj-clic ou clic droit le garde enfoncé jusqu'au clic suivant.

## Points de vigilance

- **La charge du processeur.** Comme pour Monades, **la limite de 85 % ne doit jamais être atteinte**, même dans le pire cas. Sur le Mac, la bande, la HALL et Beads réunis placent les Mondes les plus lourds de Monades trop près de cette limite. Seule une mesure sur l'appareil le dira.
- **Si la charge ne tient pas, Rhizome renoncera à un effet plutôt que de risquer le FM-1.** La première candidate est la HALL : sa mémoire et son calcul serviraient à enrichir le delay ou à un effet plus léger.
- En cas de surcharge, le firmware passe déjà de lui-même en mode économie, et la HALL en VINTAGE.

## Crédits

- **Rhizome** et **Monades** : Dr-John-8bits.
- **Mutable Dreams** : Kid Bunka / Christoph Link.
- **Mutable Instruments** : Émilie Gillet (Plaits, Rings, Elements, Clouds, Beads, stmlib, sous licence MIT).
- **fm1-base** : Charles Vestal. **Felucca / felucca-bsp** : Leo Kuroshita.

Distribution sous GPL-3.0-only. Projet indépendant, sans lien avec Mutable Instruments, M-VAVE ni l'auteur de Mutable Dreams.
