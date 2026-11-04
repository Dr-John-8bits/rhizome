# Rhizome — feuille de route (second FM-1)

Firmware pour le second M-VAVE FM-1, au service de **When Day Chokes The Night** (projet ambient). Monades, sur le premier FM-1, reste celui de Flash Dog Duker Silver.

- 2 novembre 2026 : premier brainstorm.
- 2 novembre 2026 : feuille de route affinée, avec un premier point de faisabilité chiffré.

## Le nom
**Rhizome**, d'après Deleuze et Guattari (*Rhizome*, 1976, repris en introduction de *Mille Plateaux*, 1980) : un réseau sans centre ni hiérarchie, qui pousse par tous les bouts. Le grain, l'écho et la réverbe font la même chose au son : ils le prolongent dans toutes les directions.
- Préfixe firmware : `FM-1_968`, d'après *Différence et répétition* (Deleuze, 1968). Le premier choix, `FM-1_976`, était déjà pris par LSDj Burrito. `FM-1_968` n'apparaît ni dans les catalogues de firmwares FM-1 ni dans une recherche web (2 novembre 2026) : à revérifier avant une publication. Monades : `FM-1_714`, Mutable Dreams : `FM-1_710`.
- Identité USB : « Rhizome Project » / « Rhizome FM-1 ».

## Le rôle
Monades fait les **notes** : six voix, l'harmonie, le MIDI. Rhizome fait **l'espace et le temps** : les orbites, les grains, la salle, la bande. Il joue aussi très bien seul.

Rhizome reprend ce que Monades n'a pas pu loger : Monades est plein (voir « Faisabilité »).

## Principes
- **Toute animation visualise ce qu'on règle.** Rien de décoratif : chaque mouvement à l'écran reflète un paramètre ou l'état réel du moteur. Une version sans animation reste disponible, et elle garde l'information.
- **Génératif d'abord, notes si on veut.** Par défaut, on règle un système et il joue. Mais on peut aussi **taper ses notes** (comme MOTIF dans Monades) : le séquenceur orbital les joue alors à son rythme (voir « Hauteur des notes »).
- **Aussi beau que Monades, et plus clair encore** (voir « L'écran »).

## L'écran : ce que Rhizome améliore
Monades est clair, mais ses quatre cartes K1 à K4 prennent presque la moitié de l'écran : le dessin qui montre le son n'a qu'environ 240 × 100 pixels. Rhizome inverse les proportions.

1. **Un grand dessin par page**, sur toute la largeur (environ 240 × 180 pixels). C'est lui qui raconte la page.
2. **Les réglages en bandeau fin** en bas : quatre petites jauges sur une ligne, avec leur nom court. Quand on tourne un potard, sa carte complète apparaît par-dessus le dessin, **à côté de l'élément qu'elle change**, puis s'efface. L'élément concerné s'allume pendant qu'on tourne.
3. **Une couleur par voix, la même partout** : orbite, mixer, MIDI, bande. On reconnaît une voix sans lire.
4. **Moins de mots.** Des titres courts ; l'aide reste sur un appui long, comme dans Monades.
5. **Mouvement fluide là où il porte l'information** : on ne redessine que la zone qui bouge (les orbites, les bobines), pour gagner en images par seconde. Le rafraîchissement possible est à mesurer sur l'appareil (Monades redessine environ toutes les 80 ms).
6. On garde de Monades : la palette sombre, l'univers spatial, le fond étoilé, les symboles habituels (▶ ■) là où ils portent une fonction.

## Le séquenceur orbital

### Est-ce que ça existe ?
Des séquenceurs à planètes existent, mais en applications :
- [Les Cercles](https://github.com/jeromepotter/les-cercles) (navigateur) : des planètes qui déclenchent en franchissant des portes ;
- [Orbital Harp](https://orbitalharp.pages.dev/) (iOS) ;
- [Aphelian](https://apps.apple.com/app/aphelian/id1435855056) (iOS), d'après le modèle de Kepler ;
- [Orbital](https://github.com/percuss-io/Orbital) (navigateur).

En matériel, le plus proche est le module Lunar Orbiter (A Magic Pulsewave), un disque optique qui tourne. Aucun n'est sur le FM-1. Et le 2 novembre 2026, je n'en ai trouvé aucun qui utilise la **vraie mécanique de Kepler** : l'orbite elliptique comme rubato, les conjonctions comme accents, la précession comme variation lente. C'est là qu'est la nouveauté de Rhizome.

### Le principe
- Au centre, une **étoile**. Autour, jusqu'à quatre **corps** : un par voix de Rhizome (voir « Les sons » pour le nombre de voix).
- Chaque orbite porte une ou plusieurs **portes**. Un corps qui franchit une porte déclenche un événement : une note, un nuage de grains, un accord, un changement de son.
- **N portes régulièrement espacées sur une orbite circulaire = un rythme euclidien.** L'euclidien est donc un cas particulier du séquenceur orbital, on ne le perd pas.

### Ce que la mécanique donne en musique
| Phénomène | Ce qu'on entend |
|---|---|
| **Périodes en rapports** (3:4:5, 5:7, 2:3…) | Polyrythmes qui se réalignent toutes les N mesures. Le réalignement s'entend comme un point d'appui |
| **Excentricité** (orbite ovale) | **Rubato naturel.** Deuxième loi de Kepler : le corps accélère près de l'étoile et ralentit loin d'elle. Les coups se resserrent puis s'espacent, et la boucle reste périodique, donc calée sur la mesure |
| **Distance à l'étoile** | Hauteur de la note, au choix : proche = aigu, loin = grave. L'ellipse dessine un contour mélodique qui monte et descend |
| **Conjonction** (deux corps alignés avec l'étoile) | Accent, accord, ou grain plus long. Rare si les périodes sont lointaines, donc précieux |
| **Précession** (l'ellipse tourne lentement sur elle-même) | Le rubato glisse peu à peu par rapport à la mesure : la phrase change lentement tout en restant la même. C'est la variation lente, sans hasard |
| **Lunes** (un petit corps autour d'un corps) | Ornements, ratchets, notes de passage, petits grains autour de la note principale |
| **Comète** (orbite très longue et très excentrique) | Un événement rare : un changement de section, une montée de la réverbe, un FREEZE de la bande |
| **Perturbation** (les corps se tirent un peu les uns les autres) | Une dérive aléatoire contrôlée, comme le curseur déjà-vu de Monades. 0 = mécanique parfaite, plus haut = le système vit |

### Hauteur des notes
Pour chaque corps, une source de hauteur :
- **ORBIT** (génératif) : la distance à l'étoile choisit le degré de la gamme. Avec une perturbation, les notes varient sans sortir de la gamme.
- **MOTIF** (notes tapées, comme dans Monades) : on tape quelques notes au clavier. À chaque porte franchie, le corps joue la note suivante. Le rythme reste celui de l'orbite. **Les notes tapées s'écrivent sur les portes à l'écran**, donc on voit quelle note joue où.
- **ÉCHO D'UN AUTRE CORPS** : le corps reprend la note d'un autre corps, ramenée à la note de l'accord la plus proche. C'est l'idée du tintinnabuli d'Arvo Pärt, gardée comme simple option de hauteur.

Les 91 gammes de Monades (ragas, microtonales, CUSTOM) sont reprises.

### Les autres modes : abandonnés
Le séquenceur orbital couvre ce qu'ils apportaient, donc on ne les fait pas :
- **Turing Machine** : la perturbation et le verrouillage de la graine font le même travail (boucle figée ↔ boucle qui mute).
- **Pluie** : c'est la densité des grains de Beads, qui sait déjà déclencher au hasard ou régulièrement.
- **Tintinnabuli** : devient l'option de hauteur « ÉCHO D'UN AUTRE CORPS ».
- **Automate cellulaire** : abandonné.

### Tempo
- **Synchronisé** : les périodes sont en temps ou en mesures, et suivent l'horloge MIDI. La MPC envoie l'horloge en DIN vers TRS, c'est suffisant.
- **Libre** : périodes en secondes, pour une dérive organique sans pulsation (le plus « ambient »).
- Les orbites repartent ensemble avec PLAY, comme une conjonction générale au départ.

### Systèmes réels comme presets (dans le firmware depuis le 2 novembre 2026)
Des rapports d'orbites mesurés, qui sonnent bien parce qu'ils sont en résonance. Vérifiés le 2 novembre 2026 dans les données publiées. Les périodes sont ramenées aux valeurs les plus proches du séquenceur, en gardant les rapports.

| Preset | Corps | Périodes réelles | Dans Rhizome | Excentricités |
|---|---|---|---|---|
| **JUPITER** | Io, Europe, Ganymède, Callisto | 1,77 / 3,55 / 7,15 / 16,69 jours | 3 temps, 1,5 mesure, 3 mesures, 7 mesures (1:2:4, et 9,3 au lieu de 9,4) | 0,004 / 0,009 / 0,001 / 0,007 |
| **KEPLER-223** | les planètes b, c, d, e | 7,38 / 9,85 / 14,79 / 19,73 jours | 3, 4, 6, 8 mesures (3:4:6:8) | 0,078 / 0,15 / 0,037 / 0,051 |
| **TRAPPIST-1** | les planètes e, f, g, h | 6,10 / 9,21 / 12,36 / 18,78 jours | 2, 3, 4, 6 mesures (2:3:4:6) | 0,005 / 0,010 / 0,003 / 0,004 |
| **PLUTO** | Neptune, Pluton (et Charon) | 164,8 / 247,9 ans | 4 et 6 mesures (3 tours contre 2) | 0,009 / 0,249 |

Comment ça marche :
- **chaque corps a 4 portes.** Les rapports des périodes s'entendent donc comme une polyrythmie : sur Kepler-223, en 24 mesures, b joue 32 notes, c 24, d 16 et e 12 ;
- les corps vont d'abord sur les pistes mélodiques non coupées, dans l'ordre. Les autres pistes perdent leur orbite et reprennent leur rythme euclidien ;
- le son, la gamme, la source des notes, la comète et les conjonctions ne changent pas ;
- Charon est une lune de Pluton, qui joue une tierce au-dessus de lui ;
- dès qu'on touche une orbite, le système redevient « OWN », le tien, et ENV revient aux orbites d'avant le premier système chargé.

Sources :
- [Mills et al. 2016, Nature](https://arxiv.org/abs/1612.07376), pour Kepler-223 ;
- Agol et al. 2021, *Planetary Science Journal*, pour TRAPPIST-1 ;
- les éphémérides du JPL, pour les lunes galiléennes, Neptune et Pluton.

### L'écran ORBIT : lisible et beau
C'est la page principale de Rhizome. L'animation *est* le séquenceur. Ce qu'on voit, et ce que chaque élément dit :

| Élément | Ce qu'il montre |
|---|---|
| L'étoile au centre | Son éclat suit le niveau du mélange |
| Les ellipses, fines, dans la couleur de chaque voix | Période, excentricité et orientation réelles. La précession les fait tourner lentement |
| Les portes, petits traits sur l'orbite | Où le corps déclenche. En MOTIF, la note tapée est écrite à côté |
| Le corps, un disque plein, avec une traînée | Sa vraie position. **La longueur de la traînée = sa vitesse** : on voit le corps accélérer près de l'étoile, donc le rubato |
| Une onde qui s'élargit à chaque porte franchie | Une note jouée. Sa taille suit la vélocité, sa durée l'enveloppe |
| Un trait entre deux corps alignés | Une conjonction (accent ou accord) |
| Un anneau extérieur gradué | La mesure et le temps en cours, pour voir comment les orbites tombent sur la pulsation |
| La voix choisie en plein, les autres atténuées | Ce que K1 à K4 règlent |

Chaque élément porte une information, donc la règle « rien de décoratif » est respectée d'office. La version sans animation montre la même chose, à l'arrêt (orbites, portes, positions).

### Réglages (dans le firmware depuis le 2 novembre 2026)
SEQ ouvre **ORBIT**, puis **ORBIT / SYSTEM**, puis **ORBIT / MOONS**, puis **ORBIT / COMET**, puis les pages de Monades (RHYTHM, GROOVE, BASS, PHRASE, GENERATOR, FORM). SELECT choisit la piste.
- **ORBIT** (piste choisie) :
  - K1 PERIOD : de 1 temps à 12 mesures, 13 valeurs ;
  - K2 ECCENTRICITY : de 0 à 0,85 ;
  - K3 GATES : de 0 à 16 ; 0 = orbite coupée, la piste reprend son rythme euclidien de RHYTHM ;
  - K4 PHASE : les portes tournent par pas de 15°.
- **ORBIT / SYSTEM** :
  - K1 PRECESSION et K2 PERTURBATION, pour tout le système ;
  - K3 PITCH (ORBIT / MOTIF / ECHO), pour la piste choisie ;
  - K4 SYSTEM : charge un système réel (JUPITER, KEPLER-223, TRAPPIST-1, PLUTO) ou montre OWN. La note de base de la piste reste sur MIX K2.
- **ORBIT / MOONS** (piste choisie) :
  - K1 MOONS : de 0 à 3 lunes ;
  - K2 SPIN : le temps d'un tour, de 1/16 à une mesure, 8 valeurs ;
  - K3 INTERVAL : ce que jouent les lunes à partir de la note, en 6 choix. REPEAT rejoue la même note (un ratchet). STEPS UP et STEPS DOWN montent ou descendent la gamme degré par degré, THIRDS UP et THIRDS DOWN par tierces. HARMONICS joue l'octave, l'octave et la quinte, puis deux octaves. Sur une piste de batterie, la lune rejoue le même coup ;
  - K4 LEVEL : la vélocité des lunes, en pourcentage de celle de la note.
- **ORBIT / COMET** (tout le système) :
  - K1 COMET : un tour en 4, 8, 16, 32 ou 64 mesures, ou OFF ;
  - K2 EVENT : ce que fait son passage. SWELL fait gonfler la salle, FREEZE fige la bande granulaire, BOTH fait les deux ;
  - K3 AMOUNT : l'intensité du passage, de 0 à 100 ;
  - K4 CONJUNCTION : ce que fait l'alignement de deux corps. OFF ne fait rien, ACCENT accentue leur temps suivant, CHORD les fait frapper ensemble en accord.
- La gamme et la tonalité restent sur HARMONY (GLO). Le MOTIF s'écrit au clavier sur GENERATOR, comme dans Monades.

### Les lunes (dans le firmware depuis le 2 novembre 2026)
La mécanique, la même à l'écran et dans le son :
- chaque note du corps lance ses lunes pour **un tour** autour de lui ;
- une lune joue quand elle passe du **côté extérieur** du corps, à l'opposé de l'étoile ;
- au repos, les lunes attendent régulièrement espacées derrière ce point. Pendant le tour, la lune k y passe donc aux k/(n+1) du tour : les ornements partagent le tour en parts égales après la note. Exemple : trois lunes et un tour d'un temps donnent trois doubles croches après la note.

Ce que ça donne :
- les ornements suivent le tempo, le swing et l'horloge MIDI, et partent aussi en MIDI sur le canal de la piste ;
- ils ne dérangent pas l'ÉCHO : la voix qui répond reprend la note du corps, pas ses ornements ;
- si une note arrive avant la fin du tour, elle relance les lunes, et les ornements pas encore joués sautent.

À l'écran :
- sur ORBIT et ORBIT / SYSTEM, chaque lune est un petit point autour de son corps, blanc quand elle joue ;
- **ORBIT / MOONS montre le corps choisi en gros plan.** L'étoile est en bas, au bout d'un pointillé. Le côté extérieur est marqué en haut, et la lune qui y passe s'allume. Chaque lune porte le nom de la note qu'elle joue (par exemple A#4, G#4, F#4 en descendant depuis B4), et le corps porte la sienne.

### La comète et les conjonctions (dans le firmware depuis le 2 novembre 2026)
**La comète** est un corps sans voix, d'excentricité 0,9. Elle passe presque tout son tour loin de l'étoile, puis la frôle :
- PLAY la fait partir de loin, donc le premier passage arrive à la moitié du tour ;
- son passage agit pendant à peu près un cinquième du tour, et le plus fort au plus près de l'étoile ;
- SWELL : envoi vers la salle plus fort, queue plus longue, retour plus fort, puis tout revient en douceur. Ça ne touche pas au patch ;
- FREEZE : la bande granulaire se fige pendant le cœur du passage.

À l'écran :
- dans l'anneau de la mesure, on voit la comète elle-même. **Sa queue pointe à l'opposé de l'étoile, comme en vrai, et sa longueur est l'intensité du passage** ;
- au-delà de l'anneau, une marque sur l'anneau indique sa direction, et brille plus à mesure qu'elle approche ;
- sur ORBIT / COMET, tourner K1 dessine toute son orbite, et la carte dit quand elle repassera (« NEXT PASS IN 5 BARS »).

Mesuré dans le simulateur sur Quiet Lanterns, avec une comète de 8 mesures à 80 : au passage, la différence avec et sans comète atteint −39 dB pour une musique à −38 dB. Le gonflement s'entend donc nettement.

**L'accord sur une conjonction** : les deux corps rejouent tout de suite ensemble. Le second prend la note la plus proche de la sienne dans l'accord du premier (tierce, quinte ou octave dans la gamme), donc chacun garde son registre. Avec cinq corps, les conjonctions sont fréquentes : OFF permet de les rendre muettes.

**Les notes du MOTIF à côté des portes** : quand la piste choisie joue un MOTIF, chaque porte que le corps va franchir porte le nom de la note qu'elle jouera, la prochaine en blanc. C'est le vrai parcours du moteur : il part de son curseur dans la phrase, saute les silences, et un MOTIF plus court ou plus long que le nombre de portes fait tourner les notes d'un tour à l'autre. Huit étiquettes au plus. Si la phrase est vide, l'écran l'indique.

Prévu et pas encore fait : le tempo libre (des périodes en secondes, sans horloge).

### Coût
L'équation de Kepler se résout avec une table (anomalie moyenne → position, 256 valeurs par excentricité) ou deux itérations de Newton, une fois par bloc de 64 échantillons (1,45 ms). La précision des coups est donc d'un bloc, comme dans Monades. Coût de calcul négligeable.

## Les sons

### Le granulaire : Beads au centre
> **4 novembre 2026 :** Beads prenait 50 à 70 % du processeur dès qu'il recevait du son ; il est remplacé par le granulaire de Monades (mêmes réglages, mêmes médias). Le texte ci-dessous est l'idée de départ.

- **Beads** (Mutable Instruments) : **sources publiques, licence MIT**, dans le dépôt `pichenettes/eurorack` (vérifié le 2 novembre 2026). C'est le successeur de Clouds. Ce qu'il apporte :
  - granulaire, delay et grains déclenchés au hasard ou réguliers (la densité) ;
  - **quatre « médias » d'enregistrement** : DIRTY TAPE, CLEAN TAPE, VINTAGE DIGITAL, DIGITAL. On retrouve l'esprit de la bande jusque dans le granulaire ;
  - un stockage 8 bits μ-law (`mu_law.cc`) pour doubler la durée de la mémoire ;
  - sa propre réverbe (même famille que le HALL ci-dessous) ;
  - un petit synthé à tables d'ondes quand rien n'entre.
  
  Sur son module, Beads dispose de 768 Ko de mémoire et d'un processeur à 480 MHz. Sur le FM-1, il faudra le réduire : mémoire plus courte, en mono, moins de grains, à 22 ou 32 kHz (voir « Faisabilité »).
- **Clouds** (MIT aussi) : on y prend ce qui manque à Beads. Le mode spectral sera sans doute trop lourd.
- **Les Parasites de Clouds** (Matthias Puech) : les fichiers portent l'en-tête MIT. La réverbe **Oliverb** est une deuxième couleur de réverbe possible, et **Resonestor** un résonateur. Licence à vérifier fichier par fichier.
- **Ce que les grains lisent** :
  - les voix internes ;
  - de courts samples en flash ;
  - la sortie du delay à bande (granuler la bande).

### Les voix : ce que Monades n'a pas
> **4 novembre 2026 : plan LEAD.** Les huit moteurs de Plaits coûtaient trop sur le FM-1 ; Rhizome reprend ceux de Monades et ne garde que SIX-OP FM et STRING MACHINE (voir « Première mesure sur le FM-1 »). Le texte ci-dessous est l'idée de départ.

Monades n'utilise de Plaits que trois moteurs (FM, VA, additif) et les batteries, mais **le code de tous les moteurs de Plaits 1.2 est déjà dans son dossier `third_party`**. Pour Rhizome :
- **Plaits 1.2** : String Machine (nappes façon Solina avec ensemble), Wavetable, Wave Terrain, Phase Distortion, Six-op FM (sons DX7) ;
- **Plaits** : Chords, Granular formant, Swarm, Particle ;
- **Rings** complet, avec sa polyphonie.

**Trois ou quatre voix** au lieu de six, pour laisser le calcul aux effets. Chaque voix choisit son moteur.

## La réverbe HALL « 224 dans l'idée »
- **La bonne famille d'algorithmes** : la boucle en huit du papier de Dattorro reprend le style de David Griesinger, à qui l'on doit les algorithmes du Lexicon 224. Les réverbes de Clouds, Rings, Elements et Beads sont de cette famille. On part de celle de Beads (une mémoire de 32 768 valeurs 16 bits, soit 64 Ko).
- **Réglages** :
  - PRE-DELAY ;
  - DECAY, de 0,5 à 20 s, plus une position infinie ;
  - BASS ×, pour que les graves durent plus ou moins que les médiums ;
  - TREBLE ;
  - SIZE ;
  - DIFFUSION ;
  - MOD, la modulation de la queue qui donne le côté soyeux.
- **Mode « vintage »** : la réverbe tourne à 22 kHz, avec une bande passante limitée vers 10 kHz, comme les réverbes numériques de l'époque qui tournaient à basse fréquence d'échantillonnage. C'est ce qui donne ce grain sombre, et cela **divise par deux** la mémoire et le calcul de la réverbe.
  - **Correction du premier brainstorm** : stocker en 12 bits n'économise pas de mémoire (Clouds et Beads rangent les 12 bits dans des cases de 16). C'est le passage à 22 kHz qui fait l'économie.
- **Oliverb** (Parasites) comme deuxième algorithme, dans la même mémoire : une seule réverbe à la fois.
- **Écran** : les courbes de décroissance des graves, des médiums et des aigus, avec l'énergie réelle de la queue qui glisse le long des courbes.
- Pas de nom de marque à l'écran.

### Dans le firmware (2 novembre 2026)
La HALL **remplace la petite salle de Monades** (quatre lignes courtes et ses diffuseurs).

**L'algorithme**
- La boucle en huit de Griesinger, avec les longueurs de la réverbe de Rings et Clouds (licence MIT) : un pré-délai, quatre passe-tout de diffusion, puis deux moitiés de boucle qui s'alimentent l'une l'autre.
- Une seule mémoire de 64 Ko en 16 bits, lignes bout à bout derrière un pointeur commun, comme le FxEngine de Mutable Instruments.
- La décroissance est calculée à partir du RT60 demandé et de la vraie longueur de la boucle. **Mesurée avec du bruit : 2,11 s pour 2 s demandées, 2,43 s en VINTAGE.**
- Une mesure avec une note tenue donnerait autre chose : les passe-tout retardent différemment selon la fréquence.
- Les écritures sont arrondies, sans quoi la troncature en 16 bits coupe net la toute fin de la queue.

**Pages** (FX, après la bande) :
- **HALL** :
  - K1 DECAY, de 0,5 à 20 s, puis INFINITE (la boucle tient, plus rien n'entre) ;
  - K2 SIZE ;
  - K3 PRE-DELAY, de 0 à 250 ms ;
  - K4 MOD.
- **HALL / COLOR** :
  - K1 BASS, de ×0,5 à ×2 ;
  - K2 TREBLE ;
  - K3 DIFFUSION ;
  - K4 VINTAGE (22 kHz).
- DECAY, TREBLE et DIFFUSION gardent les champs de la salle de Monades (`reverb_time`, `room_tone`, `room_diffusion`), donc les pages SPACE les règlent aussi.

**L'écran**, comme prévu :
- trois droites, pour la chute de 60 dB des graves, des médiums et des aigus après le pré-délai, plates en INFINITE ;
- sur chacune, un point qui donne l'énergie réelle de la queue dans cette bande, et qui glisse le long de la droite à mesure que la queue s'éteint.

**La comète** agit maintenant sur la HALL : la queue s'allonge jusqu'à ×4 et l'entrée double au passage. Quand le processeur est surchargé, l'ancien abandon de la diffusion devient un passage en VINTAGE, qui divise par deux le calcul de la réverbe.

**Mémoire**
- La mémoire granulaire de Monades passe de 2,97 s à 1,49 s, ce qui libère 128 Ko : 64 Ko pour la HALL, et 64 Ko d'avance pour Beads et la bande longue.
- La part du moteur passe de 352 à 275 Ko.

**Niveau** : avant et après sur les mêmes Mondes, le retour de la HALL est à −1,3 à +1,8 dB de celui de la salle. La queue est plus longue.

**Calcul**
- La HALL ajoute environ +0,6 à +1,1 ms par seconde de son, soit **+2,5 à +4,5 % du FM-1**, moins que la salle retirée.
- Avec la bande, les Mondes les plus lourds de Monades arriveraient vers 77 et 83 % : trop près du plafond de 85 %, qui ne doit jamais être atteint (voir « Points de vigilance »).
- VINTAGE divise le coût de la HALL par deux, et Beads remplacera la mémoire granulaire de Monades.

**Pas fait** : Oliverb comme second algorithme.

## Le delay à bande et son animation reel-to-reel

### Le son
- **La vitesse de la bande a de l'inertie** : changer le temps fait glisser la hauteur, comme sur une vraie machine.
- **Wow, flutter et dérive** de la vitesse.
- **Trois têtes de lecture**, combinables comme sur une Space Echo.
- **Saturation** à la tête d'enregistrement.
- **Chaque passage est plus sombre**, avec du souffle.
- **AGE** : pertes d'aigus et petites coupures.
- **Gestes sur les touches** : TAPE STOP (la bande freine, le son plonge), REWIND, FREEZE (la bande tourne en boucle).
- Stockage en 22 kHz et 8 bits μ-law : environ 2,9 s de bande dans 64 Ko. Le lo-fi fait partie du son.

### L'animation
- **Les bobines** tournent à la vitesse réelle, et on voit le wow les faire vaciller.
- **Les têtes** : enregistrement et trois lectures, espacées selon les vrais temps de delay.
- **Les traces magnétiques** sont écrites sur la bande avec leur niveau, puis défilent vers les têtes. **On voit l'écho arriver avant de l'entendre.** Elles s'effacent selon le feedback.
- **Deux limites de l'écran** :
  - le flutter (6 à 12 Hz) serait invisible ou faux à l'image : on le montre par l'amplitude du bras tendeur ;
  - une bobine à trois rayons paraît tourner à l'envers au-delà d'un tiers de tour par image (2 tours par seconde à 12 images par seconde) : vitesse d'affichage à l'échelle.

### Dans le firmware (2 novembre 2026)
Le delay à bande **remplace l'écho de Monades**, dans la même mémoire (32 Ko).

**La mécanique**
- C'est une boucle de 32 768 échantillons μ-law qui défile sous une tête d'enregistrement et trois têtes de lecture fixes, à 1/3, 2/3 et au bout du trajet.
- **TIME règle la vitesse de la bande**, pour que la tête la plus lointaine en service réponde après TIME :
  - bande rapide (jusqu'à ×12) : échos courts et nets ;
  - bande lente (jusqu'à ×0,5, soit 11 kHz) : échos longs et plus sombres, jusqu'à 2,86 s sur la tête 3.
  - C'est le compromis d'une vraie machine : la durée vient de la vitesse, pas de la mémoire.
- La vitesse a de l'inertie : changer TIME fait glisser la hauteur.

**Le son**
- Wow (0,55 Hz), flutter (7,3 Hz) et dérive aléatoire font varier la vitesse.
- La tête d'enregistrement sature (DRIVE).
- Chaque passage traverse le filtre TONE, et le filtre d'enregistrement suit la vitesse de la bande.
- AGE ajoute du souffle (seulement sous ce qui est enregistré : une bande vide reste silencieuse, PANIC donne un silence exact), perd des aigus et provoque des chutes de niveau.
- Les trois têtes sont réparties dans l'espace stéréo.

**Les commandes**
- K4 TRANSPORT : RUN, FREEZE (la tête d'enregistrement se lève, la boucle se répète) et STOP (la bande freine, le son plonge ; RUN la relance).
- Le REWIND n'est pas fait, et les gestes passent par K4 plutôt que par des touches.

**Les réglages déjà là restent** : temps, retour, synchro (avec trois divisions de plus : 1/4 pointée, 1/2, 1/2 pointée), tonalité et envois par piste. Les patchs de Monades jouent sur la tête 1, entre 40 ms et 0,95 s.

**Pages** (FX les ouvre en premier) :
- **TAPE ECHO** : K1 TIME, K2 FEEDBACK, K3 HEADS (1, 2, 1+2, 3, 1+3, 2+3, 1+2+3), K4 WOW ;
- **TAPE ECHO / AGE** : K1 DRIVE, K2 AGE, K3 TONE, K4 TRANSPORT.

**L'écran**
- Les deux bobines tournent à la vraie vitesse, ramenée à l'échelle (vitesse^0,625).
- Le bras tendeur suit le wow.
- **Ce que la tête d'enregistrement a écrit est dessiné sur la bande et avance vers les têtes de lecture : on voit l'écho arriver.**
- Les têtes en service sont colorées, blanches quand elles rejouent, et chacune affiche son temps réel. La tête d'enregistrement se lève sous FREEZE.
- Une ligne prévient si le retour est à 0 ou si aucune piste n'envoie vers la bande.

**Calcul**
- Mesure sur le Mac, avant et après, avec les mêmes Mondes : +1,0 à +1,2 ms de calcul par seconde de son, soit environ **+4 à +5 % du FM-1**. Les Mondes 87 et 89 passeraient d'environ 69 et 76 % à environ 73 et 81 %.
- La première version coûtait le double ; deux réglages l'ont ramenée là :
  - un seul encodage par échantillon de sortie quand la bande va vite ;
  - une interpolation linéaire au-delà de ×2.

**Reste** : REWIND, les gestes sur les touches, et 64 Ko de bande (2,97 s à ×1) quand Beads aura remplacé la mémoire granulaire de Monades.

## Ce qu'on garde de Monades
- **Sortie audio USB du mélange final** vers l'ordinateur (dans `fm1.toml`, c'est la ligne `usb_audio_in = true`, nom trompeur).
- **Sortie MIDI** des notes générées, par pistes ou par zones (`midi_out.inc`).
- **Horloge MIDI** en entrée (TRS et USB) et en sortie.
- Les 91 gammes, les LFO, l'univers spatial, l'installeur, le simulateur dans le navigateur, les tests.

## Hors champ
- **La liaison entre les deux FM-1.** La MPC envoie l'horloge MIDI (DIN vers TRS) à l'un ou l'autre : c'est suffisant.
- **Traiter le son de Monades** par l'audio USB dans les deux sens. Peut-être un jour, pas un objectif.

## Faisabilité (premier point, 2 novembre 2026)

### Ce que le FM-1 offre
Relevé dans la compilation de Monades 0.1.3 pour l'appareil (`app/build/monades.elf`) et dans fm1-base :

| Ressource | Disponible | Monades 0.1.3 en utilise |
|---|---|---|
| Flash de l'application | 602 112 octets (`FLASH_SIZE` de `fm1pkg.py`) | 444 828 (dont 153 608 pour les 91 Mondes) |
| RAM « bss » | 98 304 octets | 83 160 |
| RAM « pool » (gros tampons) | 344 064 octets | 334 872 (dont 262 144 pour la bande granulaire, 32 768 pour l'écho) |
| Processeur | pi32v2, 240 MHz selon le SDK, 360 MHz sous le chargeur selon fm1-base | jusqu'à 77 % en ECO (Mondes 87 et 89) |
| Second cœur | `plat_cpu2_*` de fm1-base, déjà utilisé par X0X, NuSaw, SPH-16 et XY-1 | non utilisé |

Au total, environ **440 Ko de RAM** pour l'application.

### Budget mémoire de Rhizome (estimation)
| Bloc | RAM |
|---|---|
| Plateforme (écran par bandes, USB audio, sauvegarde) | environ 35 Ko |
| Mémoire granulaire Beads (mono, μ-law 8 bits : 3 s à 44,1 kHz ou 6 s à 22 kHz) | 128 Ko |
| Travail des grains | environ 8 Ko |
| Réverbe HALL (64 Ko à 44,1 kHz, ou deux fois plus longue en vintage à 22 kHz) | 64 Ko |
| Delay à bande (environ 2,9 s, 22 kHz, μ-law) | 64 Ko |
| Quatre voix Plaits ou Rings (le moteur le plus gourmand, Particle, demande 16 Ko) | environ 64 Ko |
| **Total** | **environ 363 Ko sur 440** |

Il reste environ 75 Ko pour l'état de l'application (presets, orbites, MIDI, écran). **Jouable, avec une marge modeste.** Les curseurs pour faire de la place : la durée de la mémoire granulaire, le nombre de voix, la réverbe en 22 kHz.

### Budget flash (estimation)
- Base de Monades sans ses 91 Mondes et sans ses écrans propres : environ 230 Ko.
- Code de Plaits et de Rings : quelques dizaines de Ko. Dans Monades, tout Plaits utilisé pèse 15 Ko et Rings 25 Ko.
- Tables de Plaits au complet : environ 75 Ko (tables d'ondes 50 Ko, sons DX7 12 Ko).
- Beads : environ 40 Ko, à mesurer.
- Presets de Rhizome : peu, quelques Ko.

**Total : environ 400 à 450 Ko sur 602. Jouable.**

### Calcul : le vrai point à mesurer
Ce qu'on sait :
- **Chaque moteur de Plaits** tourne sur le module Plaits, un Cortex-M4 à 72 MHz, à 48 kHz. Le FM-1 est plus rapide en fréquence. Trois ou quatre voix Plaits sont donc raisonnables.
- **Rings** tourne sur un Cortex-M4 à 168 MHz ; il est plus lourd en polyphonie.
- **Clouds** tourne sur un Cortex-M4 à 168 MHz à 32 kHz, et ajuste lui-même son nombre de grains à la charge.
- **Beads** tourne sur un Cortex-M7 à 480 MHz : c'est le plus lourd. Il faudra le faire tourner comme Clouds (22 ou 32 kHz, mono, moins de grains).

| Élément | Verdict (après le banc d'essai sur le Mac) |
|---|---|
| Séquenceur orbital, MIDI, horloge, USB audio | ✅ sûr (orbites : environ 1 %) |
| Delay à bande | ✅ sûr (prototype : environ 4 %) |
| HALL à 44,1 kHz | ✅ probable (environ 2,6 %) |
| HALL vintage à 22 kHz | ✅ probable (environ 1,3 %) |
| 3 ou 4 voix Plaits | ✅ probable : 2 à 9 % par voix pour la plupart des moteurs |
| Moteurs Plaits lourds (FM 2 opérateurs, Particle, String, Modal) | 🟡 11 à 20 % par voix : un ou deux au plus |
| Rings polyphonique | 🔴 trop lourd : 32 à 57 % pour une seule partie. Seul RINGS STRING à une voix passe (environ 12 %) |
| Granulaire Beads | ✅ probable : 8 à 21 % selon le nombre de grains et le média |
| Mode spectral de Clouds | 🔴 sans doute trop lourd |
| Second cœur (Beads sur un cœur, le reste sur l'autre) | 🟡 faisable d'après X0X et les autres, à intégrer dans fm1-base |

### Banc d'essai sur le Mac (2 novembre 2026)
`python3 tools/bench_rhizome.py` compile `tests/rhizome_bench.cpp` avec les réglages du DSP du FM-1 :
- même optimisation, les maths rapides de `app/compat`, et la vectorisation automatique coupée, car le FM-1 n'a pas d'unité SIMD ;
- chaque bloc rend une seconde de son à 44,1 kHz, par quanta de 64 échantillons, et on garde le meilleur temps de 5 passages de 20 s ;
- le temps est converti en charge du FM-1 grâce à deux Mondes de Monades dont la charge a été lue sur l'appareil (87 CLOUDEE 69 %, 89 ALCHIMISTE 76 %, en ECO).

**Les deux étalons concordent à 10 % près** : avec l'un seul, CLOUDEE donnerait 66 % et ALCHIMISTE 80 %. Les chiffres ci-dessous sont donc des ordres de grandeur, à ±10 % au moins. Ils seront confirmés sur le FM-1.

| Bloc | Charge estimée sur le FM-1 |
|---|---|
| **Voix Plaits** (une voix, une note toutes les 0,5 s) | |
| CHIPTUNE (1.2) | 1,8 % |
| WAVESHAPING | 3,2 % |
| CHORDS | 3,6 % |
| VA (déjà dans Monades) | 3,9 % |
| NOISE | 4,0 % |
| VA-VCF (1.2) | 5,0 % |
| WAVETABLE | 5,5 % |
| ADDITIVE (déjà dans Monades) | 6,0 % |
| STRING MACHINE (1.2) | 6,3 % |
| PHASE DISTORTION (1.2) | 6,3 % |
| WAVE TERRAIN (1.2) | 6,4 % |
| GRANULAR FORMANT | 7,5 % |
| SIX-OP FM, sons DX7 (1.2) | 8,0 % |
| SWARM | 8,9 % |
| MODAL | 11,2 % |
| STRING (inharmonique) | 14,1 % |
| PARTICLE | 19,4 % |
| FM 2 opérateurs (déjà dans Monades) | 19,6 % |
| **Rings** (une partie complète) | |
| RINGS STRING, 1 voix | 11,6 % |
| RINGS SYMPATHETIC, 2 voix | 31,9 % |
| RINGS STRING, 4 voix | 43,3 % |
| RINGS MODAL, 4 voix | 51,0 % |
| RINGS MODAL, 1 voix (plus de modes par voix) | 56,5 % |
| **Effets et séquenceur** | |
| ORBITS, 4 corps (équation de Kepler à chaque quantum) | 1,3 % |
| HALL vintage à 22 kHz (stéréo) | 1,3 % |
| HALL à 44,1 kHz (stéréo) | 2,6 % |
| TAPE DELAY, prototype (22 kHz, μ-law, 3 têtes, wow, flutter, saturation) | 4,1 % |

**Beads** (mesuré le 2 novembre 2026, avec `--beads` : une copie de `pichenettes/eurorack`, compilée avec ses 32 grains d'origine puis réduite à 8 ; mono, une dent de scie à l'entrée) :

| Média de Beads | 32 grains | 8 grains |
|---|---|---|
| DIGITAL (16 bits) | 13,8 % | 8,3 % |
| VINTAGE (32 kHz, 12 bits) | 21,2 % | 11,2 % |
| CLEAN TAPE (24 kHz, 12 bits) | 19,9 % | 15,0 % |
| DIRTY TAPE (24 kHz, 8 bits μ-law) | 19,3 % | 12,1 % |

- La densité des grains (0,65 ou 0,9) ne change presque rien : c'est le nombre de grains possibles qui compte.
- Les médias « bande » coûtent plus que DIGITAL : ils convertissent la fréquence d'échantillonnage dans les deux sens, avec des filtres de 48 points.
- **Même Beads complet (32 grains) tient** : au plus 21 %.

**Ce que ça donne pour Rhizome**, avec par exemple STRING MACHINE, WAVETABLE, SIX-OP et CHORDS :
- les quatre voix : environ 22 % ;
- le HALL, la bande et les orbites : environ 8 % ;
- Beads : 8 à 21 % ;
- le mélange, les filtres et les LFO : quelques % de plus.

**Au total, environ 45 à 55 %**, sous le plafond de 85 % que Monades s'impose. Il reste de la marge pour une voix lourde (PARTICLE, MODAL) ou un Rings à une voix. Le second cœur n'est pas nécessaire au départ.

Pour Beads sur le FM-1, il faudra : un `abs` entier dans `app/compat`, retirer un `printf` de débogage (actif sous `TEST`, que le FM-1 compile aussi), et réduire ses tampons (768 Ko sur le module) à la mémoire du FM-1. `std::partial_sort` est déjà ajouté.

**Une découverte en route** : les autres moteurs de Plaits 1.2 et Rings demandent quelques fonctions que le FM-1 n'avait pas (`std::copy`, `std::sort`, `std::rotate`, `fabs` en simple précision). Elles sont ajoutées dans `app/compat`, en version courte et sans `double`.

### Le risque connu de la plateforme
D'après `bsp/docs/CPU.md`, des multiplications 64 bits mêlées à des lectures 16 bits peuvent corrompre un registre sur ce processeur. Le code en virgule flottante (Plaits, Rings, Beads le sont surtout) n'est pas concerné. Il faudra quand même chercher ces motifs dans le désassemblage du code de Rhizome.

## Points de vigilance
- **La charge du processeur, avant tout.** Il faut être certain que Rhizome ne fasse pas de mal au FM-1 : pas de surcharge, pas de craquements, pas de blocage.
  - **La règle, comme pour Monades : les 85 % ne doivent jamais être atteints**, même dans le pire cas (les Mondes les plus lourds, avec la bande, la HALL et Beads ouverts ensemble). Ce n'est pas un objectif mais un plafond, et il faut garder une marge en dessous.
  - Monades a déjà montré pourquoi : son audio USB craquait sur les Mondes chargés (70 à 80 % du temps de rendu) jusqu'à la réserve agrandie `FELUCCA_UAC_DEEP`, que Rhizome a gardée.
  - **Le 3 novembre 2026, les 12 Mondes de Rhizome sont estimés entre 36 et 58 % sur le Mac, pire cas compris** (SPACE à 100, Beads en CLEAN TAPE). Mais un Monde de l'utilisateur peut être bien plus lourd : six voix Rings, par exemple.
  - **La garde automatique ne connaît pas encore les effets de Rhizome.** Avant de jouer en FULL, le firmware estime le coût des voix et des effets de Monades, et passe en ECO au-delà d'un seuil. Elle ne compte ni la bande, ni la HALL, ni Beads : à recaler avec la mesure sur l'appareil. Les Mondes de Rhizome se chargent en ECO.
  - Les estimations faites sur le Mac le 2 novembre 2026 placent les Mondes les plus lourds de Monades près du plafond de 85 %, voire au-dessus. Ce sont les additions de la bande (+4 à 5 %), de la HALL (+2,5 à 4,5 %) et de Beads (+6 à 9 % quand SPACE est ouvert).
  - **Ce ne sont que des estimations : seule une mesure sur l'appareil compte.** À faire dès la première compilation, Monde par Monde, avec la bande, la HALL et Beads ouverts ensemble, dans le pire des cas.
  - La sécurité existe déjà : en cas de surcharge, le firmware passe en ECO et la HALL en VINTAGE (moitié du calcul).
- **Si la charge ne tient pas, on renonce à quelque chose plutôt que de risquer l'appareil.** Dans l'ordre :
  - **se passer de la HALL**, et utiliser sa mémoire (64 Ko) et son calcul pour enrichir le delay à bande : bande plus longue, plus de têtes, ou une diffusion dans la boucle de feedback pour un écho qui se change en nappe ;
  - ou garder la seule version VINTAGE de la HALL ;
  - ou remplacer la HALL par un effet plus original et plus léger, à inventer ;
  - réduire Beads : moins de grains en ECO, ou un seul média ;
  - passer un bloc sur le second cœur (`plat_cpu2_*`).
- Le grand nettoyage (retirer les voix lourdes de Monades au profit de celles de Rhizome) devrait de toute façon redonner de la marge.

## Première mesure sur le FM-1 (4 novembre 2026)
Rhizome est installé sur le second FM-1 (identité `FM-1_968000001`, depuis la V15). Mesures avec `tools/measure_fm1.py` (par USB, sans écriture en flash) : **beaucoup trop lourd**, le banc du Mac avait sous-estimé d'un facteur 2 environ.

| Ce qui tourne | Charge sur le FM-1 |
|---|---|
| Rien (EMPTY SKY, effets à 0, SPACE 0) | 7 % |
| Beads, silencieux, SPACE à 30 | **+24 à 28 %** (le Mac disait 6 à 9) |
| Bande et HALL (retour à 48) | +14 % (pic +29) |
| Une voix qui sonne | +6 à 9 % (WAVETABLE, TERRAIN, SWARM, SIX-OP, CHORDS, FORMANT, PD), **+13 % STRING MACHINE** |
| LAST LIGHT à l'arrêt, SPACE 0 | 55 % : les voix ne se mettent jamais en veille, même silencieuses |
| Les Mondes 1 à 4 en jeu | **121 à 130 %**, des milliers de décrochages |

À faire avant tout autre ajout :
1. **mettre les voix en veille** quand leur enveloppe est éteinte (elles calculent aujourd'hui pour rien) ;
2. **alléger Beads** (moins de grains, ou demi-fréquence, ou un seul média) ;
3. **moins de voix jouées ensemble** dans les Mondes (3 ou 4, comme prévu au départ), et STRING MACHINE avec parcimonie ;
4. recaler la garde automatique et le banc du Mac sur ces chiffres, puis remesurer.

**Deuxième et troisième essais (4 novembre 2026).** Voix en veille, Beads à 3 grains, Mondes à 4 voix : toujours 101 à 147 %. Un souffle anti-dénormaux sur Beads : aucun effet. Les relevés montrent que **Beads coûte 50 à 70 % dès qu'il reçoit du son** (17 % silencieux), et que sans lui les Mondes vont de 42 à 105 % : les voix de Plaits coûtent 10 à 20 % chacune en jeu. D'où :
- **Beads est remplacé par le granulaire de Monades** (4 grains sur une bande de 128 Ko, la page BEADS et ses réglages gardés, les médias redéfinis) ;
- **en ECO, toutes les voix tournent à demi-fréquence**, SIX-OP compris (ses enveloppes recalculées pour garder leur durée).
**Mesuré le 4 novembre au soir** : les 18 Mondes vont de 62 à 108 % (contre 101 à 147). Sous le plafond : SLOW EMBERS 77, NO STARS 67, COLD IN THE DARK 70, SLOW TIDES 65. Le budget réel sur le FM-1 : base 7 %, bande et HALL 14 %, grains de Monades 5 à 12 %, **chaque voix qui joue 11 à 15 %** même à demi-fréquence (tous les moteurs se valent). Les nappes gardent leurs voix éveillées : un Monde de 4 voix fait donc 7 + 14 + 12 + 4 × 13, soit 85 % environ. **Il faut 3 voix par Monde**, ou 4 sans les grains, ou trouver pourquoi une voix coûte autant (le traitement par voix autour du moteur est à mesurer).

**Décision du 4 novembre 2026 : le plan LEAD si le profilage ne donne rien de net.** Une seule passe de profilage sur l'appareil (le temps de chaque partie du rendu). Si elle ne désigne pas clairement une cause qu'on peut corriger, ou ne donne que des pistes vagues, on arrête l'enquête (trop coûteuse) et on passe à la voie du milieu :
- **les moteurs de Monades** pour les voix (ils tiennent à 64–74 % avec 6 voix et les effets, mesuré) ;
- **deux voix de Rhizome gardées** : **SIX-OP FM** et **STRING MACHINE** ;
- **le séquenceur orbital**, **le tape echo** et **la HALL** gardés ; les grains de Monades.
Que les autres voix sonnent comme celles de Monades n'est pas un problème : ça sonne déjà très bien.

**Le profil sur l'appareil (4 novembre 2026)**, en % du processeur : KEPLER NIGHT 95 (séquenceur 5, moteurs 32, traitement autour des voix 25, grains 8, effets et mélange 24) ; CHOKED DAWN 92 (4, 29, 24, 7, 25) ; BLACK ORCHARD 74 (4, 22, 18, 7, 22) ; NO STARS 50 (3, 8, 12, 6, 19). Pas de défaut unique à corriger : le traitement autour des voix est celui de Monades, les moteurs de Plaits coûtent environ 8 % chacun même à demi-fréquence, et la bande, la HALL et le mélange 20 à 25 %. **Verdict : plan LEAD.**

**Le plan LEAD, fait le 4 novembre 2026 (à mesurer sur le FM-1).** Les douze moteurs de Monades reviennent (KICK, SNARE, HAT, FM, VA, RINGS, HARMONIC, CHOIR, BASS, ELEMENTS, MELLOW, SATIN), avec **SIX-OP FM** et **STRING MACHINE** en moteurs 13 et 14, construits dans une petite zone de mémoire par voix (le reste de Plaits est retiré de la compilation). La bibliothèque compte 256 sons : les 224 de Monades, puis 16 pour SIX-OP FM et 16 pour STRING MACHINE. Les douze premiers Mondes retrouvent leurs sons de Monades ; les six DEEP sont recomposés avec eux, SIX-OP FM et STRING MACHINE sur une ou deux pistes. Toujours quatre voix au plus par Monde. ECO : FM, VA, HARMONIC, SIX-OP et STRING MACHINE à demi-fréquence. Les patchs passent en version 13 : une sauvegarde de Monades se charge telle quelle, une sauvegarde de l'essai à huit moteurs de Plaits reçoit les moteurs les plus proches. Le séquenceur orbital, le tape echo, la HALL et les grains de Monades ne changent pas. Reste : compiler, installer, mesurer les 18 Mondes (objectif : pic sous 85 %, vers 70 %).

**Mesuré sur le FM-1 (4 novembre 2026, plan LEAD).** Les douze premiers Mondes tiennent de 69 à 79 % de pic, sans décrochage ; TRAPPIST DUST 82, SLOW TIDES 68, LOOPS OF ASH 69. Trois dépassent : COLD IN THE DARK 100, KEPLER NIGHT 101, RAGA OF ASH 99, les seuls avec SIX-OP FM, dont le moteur coûtait 12 à 15 % : il calculait deux voix (une note qui chevauche la suivante). Corrigé : **SIX-OP n'a plus qu'une voix** (une piste est monophonique), et **une seule voix de Rhizome dans les Mondes les plus chargés** : KEPLER NIGHT garde SIX-OP (un chœur remplace les cordes), COLD IN THE DARK garde STRING MACHINE (une cloche FM remplace le DX7), RAGA OF ASH garde ses cloches DX7 avec une basse ronde pour bourdon. SLOW TIDES : 'Slow Glass', trop discret au début de sa forme, devient 'Moon Tone'. Reste : recompiler et remesurer.

**Décision du 4 novembre 2026 : trois Mondes retirés, six nouveaux, plus légers.** COLD IN THE DARK, KEPLER NIGHT et RAGA OF ASH (99 à 101 %) sont retirés. Six Mondes à trois voix arrivent, chacun dans sa gamme : la famille **ECHO** (la bande et la HALL en avant : RUST REELS, CATHEDRAL OF SMOKE, DRIFTING SIGNAL) et la famille **VOICE** (SIX-OP FM et STRING MACHINE : GLASS PLANETS, ASH ENSEMBLE, NIGHT BELLS). Rhizome passe à 21 Mondes plus EMPTY SKY. TRAPPIST DUST, à 82 %, est allégé (un FM remplace l'additif). Reste : compiler, installer, mesurer.

## Le code et GitHub
- Rhizome est **dérivé de Monades**. Mais **GitHub ne permet pas de forker son propre dépôt dans le même compte**. Donc on crée un nouveau dépôt `Dr-John-8bits/rhizome` et on y pousse l'historique de Monades. Monades reste une « source » (un remote `monades`) d'où l'on récupère les corrections communes (MIDI, gammes, plateforme).
- Même plateforme fm1-base, même chaîne de compilation, mêmes outils.
- Licence héritée : GPL-3.0-only, avec les notices MIT (Mutable Instruments, Parasites), MPL-2.0 (felucca-bsp) et les crédits de Mutable Dreams et Monades.

## Ordre conseillé
0. **Maquette jouable de l'écran ORBIT dans le navigateur** (240 × 240, avec le son) : valider la lisibilité et l'envie de jouer avant d'écrire le firmware. **Faite le 2 novembre 2026** : `docs/rhizome/maquette/orbite.html`, publiée sur [claude.ai](https://claude.ai/artifact/2Q3b1BXKckoXwbATJbcFHZ) (privée). La mécanique est la vraie : équation de Kepler, portes euclidiennes, précession, lunes, comète, conjonctions. Le son n'est qu'une esquisse dans le navigateur. Six systèmes : Kepler-223, Io-Europe-Ganymède, TRAPPIST-1, Pluton-Neptune, Euclidien, Dérive.
1. **Banc d'essai du calcul** : chaque bloc mesuré sur le Mac, puis sur le FM-1. Il décide du nombre de voix et de la taille de Beads. **Sur le Mac, le 2 novembre 2026, Beads compris** : voir « Banc d'essai sur le Mac ». Reste la confirmation sur le FM-1.
2. **Création du dépôt** à partir de Monades. **Fait le 2 novembre 2026** : dépôt privé `Dr-John-8bits/rhizome`, identité `FM-1_968`, noms USB et écrans Rhizome. Le préfixe `FM-1_976` prévu au départ était déjà pris par LSDj Burrito ; `FM-1_968` renvoie à *Différence et répétition* (Deleuze, 1968). Le grand nettoyage (retirer les 91 Mondes, les écrans et le site de Monades) se fera au moment où le moteur de Rhizome les remplacera : le moteur actuel en a besoin pour tourner.
3. **Séquenceur orbital** dans le simulateur, puis sur le FM-1. **Première version dans le simulateur le 2 novembre 2026**, sur le moteur de Monades :
   - moteur `app/dsp/orbit.inc` : patch v11 (1924 octets), 16 commandes, conjonctions (accent ou accord), lunes, comète, systèmes réels ;
   - pages ORBIT, ORBIT / SYSTEM, ORBIT / MOONS et ORBIT / COMET (`app/orbit_page.c`), dans le style prévu : grand dessin, bandeau fin, carte du potard ;
   - tests : rythme euclidien exact pour une orbite ronde (à 0,05 pas près), rubato sur un cycle stable, MOTIF, conjonction, pas de rafale, lunes (moments exacts, notes, intervalles), comète (passages, durée, gel de la bande), accord, systèmes réels (rapports, polyrythmie exacte sur 24 mesures, retour à OWN), curseur du MOTIF, import des patchs et des emplacements de Monades, scénario `tests/orbit_ui.txt` ;
   - captures dans `docs/rhizome/captures/`, quatre écoutes de 45 s (Monde 61 Quiet Lanterns : cinq corps, puis avec des lunes, puis avec la comète et les accords, puis Kepler-223 en MOTIF) dans `M-VAVE FM1/ecoutes/` ;
   - le patch v11 n'est encore sorti nulle part : sa taille peut changer sans conversion jusqu'à la première version installée ;
   - remarque : avec cinq corps, les conjonctions sont fréquentes (18 en 12 s), donc moins « rares et précieuses » qu'avec deux ou trois. À régler à l'oreille ; K4 CONJUNCTION sur OFF les rend muettes ;
   - **reste à faire : le compiler pour le FM-1 et l'essayer sur l'appareil.**
4. **Delay à bande** et son animation. **Dans le simulateur le 2 novembre 2026** (voir « Dans le firmware » plus haut). Tests : temps de chaque tête, FREEZE qui boucle, STOP qui freine, PANIC silencieux, scénario `tests/tape_ui.txt`. Une écoute dans `M-VAVE FM1/ecoutes/`.
5. **Réverbe HALL**. **Dans le simulateur le 2 novembre 2026** (voir « Dans le firmware » dans la section HALL). Tests : RT60 mesuré, VINTAGE, INFINITE, bornes, scénario `tests/hall_ui.txt`. Une écoute dans `M-VAVE FM1/ecoutes/`.
6. **Granulaire Beads**. **Dans le simulateur le 2 novembre 2026.** C'est le cœur de Beads (MIT), dans `app/third_party/beads` ; ce qui a été changé pour le FM-1 est listé dans son `NOTICE.md`.
   - **Mémoire** : les 128 Ko du granulaire de Monades, qu'il remplace. Elle reste en mono, donc sa durée dépend du média : DIGITAL 1,4 s, CLEAN TAPE 3,8 s, DIRTY TAPE 5,8 s. Le média VINTAGE de Beads est retiré pour la place en flash (sans son convertisseur à 32 kHz, ce n'était qu'un DIGITAL en 12 bits).
   - **Grains** : 6 au plus. Ils sont générés en continu, comme avec le bouton SEED verrouillé sur le module. **Avec SEED sur OFF, Beads ne lance aucun grain.**
   - **Réglages** : POSITION, SIZE, PITCH, FEEDBACK et FREEZE gardent les champs de Monades ; la comète peut aussi figer la mémoire. DIRECTION REVERSE passe par le tiers bas de SIZE, où Beads joue ses grains à l'envers.
   - **Nouvelle page BEADS** (FX, après GRANULAR BUS) : DENSITY, SHAPE, RANDOM, MEDIA. L'écran montre la mémoire et les grains vivants, et l'enveloppe de grain que SHAPE dessine vraiment.
   - Quand SPACE est à 0, Beads se repose et ne calcule rien.
   - **Niveau** : le même que les grains de Monades, à 1,7 dB près, sur six Mondes.
   - **Calcul, à surveiller** : d'après la mesure sur le Mac, Beads coûte environ 1,4 à 2,3 ms par seconde de son de plus que les grains de Monades, soit **+6 à +9 % du FM-1** quand SPACE est ouvert. Avec la bande et la HALL, les Mondes les plus lourds de Monades pourraient dépasser le plafond de 85 %. Pistes : moins de grains en ECO, le second cœur, et le grand nettoyage (les voix de Rhizome sont plus légères).
   - Changer de média vide la mémoire, comme sur le module (l'aide le dit).
   - **Tests** :
     - la mémoire de chaque média ;
     - DENSITY au milieu, sans aucun grain, et des grains de part et d'autre ;
     - FREEZE, qui fait jouer la mémoire quand les pistes se taisent ;
     - le repos à SPACE 0 ;
     - les bornes ;
     - le scénario `tests/beads_ui.txt`.
   - Une écoute dans `M-VAVE FM1/ecoutes/`.
   - **Reste à faire** : la mesure sur l'appareil (voir « Points de vigilance »).
7. **Voix Plaits 1.2 et Rings.**

**Site et mode d'emploi** (2 novembre 2026, le dépôt étant devenu public) :
- le mode d'emploi est dans `docs/rhizome/MODE-D-EMPLOI.md`, et sur le site (`site/manuel/`, régénéré par `tools/site_manual.py`) ;
- le mode d'emploi de Monades reste sur le site (`site/monades/manuel/`) pour les pages héritées ;
- le simulateur est compilé par GitHub Actions (`.github/workflows/pages.yml`, avec le WASI SDK 34) et publié avec le site sur GitHub Pages.

**Première compilation pour le FM-1** (2 novembre 2026) : `rhizome-0.0.1.fwsc`, 610 136 octets, identité `FM-1_968000001`. Flash 561 720 octets sur 602 112, RAM 85 848 sur 98 304, pool 276 852 sur 344 064. Revue avant flash et chiffres détaillés : `docs/rhizome/REVUE-AVANT-FLASH.md`. **Pas encore installé ni mesuré sur l'appareil.**

**Accueil et Mondes de Rhizome** (3 novembre 2026, en attendant le FM-1 de Rhizome) :
- **L'accueil (LIVE MONDE)** montre le système du Monde en miniature, à la place de la flotte de Monades : ellipses, portes qui s'allument, corps et ondes des notes, conjonctions, comète, étoile qui brille de ce qu'on entend, anneau de poussière pour SPACE. Même état du moteur que la page ORBIT (`md_orbit_mini` dans `app/orbit_page.c`). **Au démarrage**, les corps du Monde tombent sur leurs orbites et tournent à leurs vraies vitesses ; LOOK (MOTION) les fait tourner ou les fige.
- **DENSITY et MOTION agissent sur les orbites** : DENSITY ajoute ou retire jusqu'à cinq portes, MOTION fait sauter une part des notes (un pas de la gamme, une quarte, une quinte, une octave), comme sur un rythme. Avant, ces deux macros de l'accueil ne faisaient rien aux orbites.
- **12 Mondes propres à Rhizome** à la place des 91 de Monades, plus EMPTY SKY (page blanche) : DUSK (résonances), NIGHT (orbites excentriques), DAWN (lunes et comètes). Écrits dans `tools/rhizome_mondes.py`, en patchs v11 complets. Tous à −30 dB RMS (NO STARS à −37). La table des Mondes passe de 153 à 26 Ko de flash.
- **Charge estimée sur le Mac** (banc `tools/bench_rhizome.py`, étalonné sur CLOUDEE et ALCHIMISTE, gardés à part dans `tests/monades_yardstick.inc`) : de 36 à 53 % tels qu'ils se chargent, de 36 à 58 % avec SPACE à 100 et Beads en CLEAN TAPE. Les Mondes de Monades montaient à 77–83 %.
- **Deux défauts corrigés en route** : la bande (hasard, pleurage, vitesse) et la HALL (phase de modulation) gardaient un état d'une écoute à l'autre. Le même Monde ne sonnait donc pas deux fois pareil après un retour au départ. La bande démarre aussi à sa vitesse, sans glissement, quand on change de Monde.
- Une écoute des 12 Mondes, 30 s chacun : `M-VAVE FM1/ecoutes/rhizome-12-mondes.m4a`.
- **Héritage visuel à personnaliser** : les pages **MORPH** et **DERIVE** (HOME 2 et 3 fois) montrent encore la flotte de vaisseaux de Monades pour les Mondes A et B, et le patch entre les deux. Ce n'est pas grave, elles fonctionnent ; mais si on trouve un dessin propre à Rhizome, ce serait mieux. Une piste : dessiner les deux systèmes en miniature (comme l'accueil) et montrer les orbites qui se déforment de l'un à l'autre pendant la MORPH ; pour la DERIVE, le système de départ pâle au centre et le système actuel qui s'en éloigne. Il faudrait pour cela savoir dessiner un système à partir d'un patch qui n'est pas celui qui joue.
- **La HALL est discrète dans les Mondes d'usine** : mesurée le 3 novembre 2026, elle arrive 15 à 20 dB sous le son direct, et n'ajoute que 0 à 0,4 dB au niveau d'ensemble. Le mode d'emploi explique comment l'entendre (« Entendre la HALL »). **Montée le même jour** : les envois des pistes mélodiques sont multipliés par 2,2 (la basse reste sèche). Mesurée exactement (le rendu avec la HALL moins le rendu sans, le moteur étant déterministe), elle arrive maintenant 9 à 18 dB sous le reste, contre 16 à 25 avant. Écoute : `M-VAVE FM1/ecoutes/rhizome-12-mondes-hall.m4a`.

**Les voix de Rhizome** (3 novembre 2026) : Rhizome ne double plus Monades. Les douze moteurs hérités (batteries, FM, VA, additif, Rings, Elements, et les quatre de Mutable Dreams) sont remplacés par **huit moteurs de Plaits** : STRING MACHINE, WAVETABLE, WAVE TERRAIN, SIX-OP FM, SWARM, PHASE DISTORT, CHORDS et FORMANT. PARTICLE est écarté : 16 Ko de mémoire par voix et 15 % de processeur.
- **Mémoire** : chaque voix ne porte plus que le moteur de son modèle, construit dans sa propre zone. Les 96 sons DX7 de SIX-OP sont décodés une fois pour les six voix.
- **Une bibliothèque de 128 sons** propre à Rhizome (`tools/rhizome_tones.py`), 16 par moteur, à la place des 224 de Monades. Les 12 Mondes sont recomposés avec elle, puis rééquilibrés à −30 dB.
- **Les huit moteurs sont réglés au même niveau** (environ −31 dB sur leurs 16 sons).
- **La page SOUND nomme les potards** de chaque moteur (CHORD, BANK, SOUND…), et écrit le nom de l'accord ou du son DX7.
- **La garde automatique** compte maintenant les voix de Rhizome et ses effets (Beads quand SPACE est ouvert).
- **Charge estimée sur le Mac** : de 36 à 53 % pour les 12 Mondes, pire cas compris (contre 36 à 58 % avant). Le banc garde le facteur d'étalonnage du 2 novembre : les Mondes de Monades qui servaient de référence ne peuvent plus tourner.
- **Patch v12.** Une sauvegarde plus ancienne (Monades, ou Rhizome v11) prend pour chaque piste le moteur de Rhizome le plus proche.
- **Trois défauts de Plaits corrigés en route** (voir `app/third_party/plaits/NOTICE.md`) : le WAVETABLE écrivait et lisait hors de sa table, et SIX-OP lisait une case trop loin à une vélocité de 1. Ils sont trouvés par un plantage pendant la DERIVE, puis par les sanitizers.
- **Version de référence** gardée pour la première mesure : `M-VAVE FM1/flash/rhizome-0.0.1-reference.fwsc` (avant ce changement de voix).

**REWIND, périodes libres, six Mondes DEEP** (3 novembre 2026) :
- **REWIND** de la bande (TAPE ECHO / AGE, K4 TRANSPORT) : la tête d'enregistrement se lève, la bande repart en arrière trois fois plus vite (le moteur se retourne en un dixième de seconde), les échos reviennent à l'envers ; les bobines tournent à l'envers à l'écran.
- **Le tempo libre, orbite par orbite** : après les 13 périodes en pas, PERIOD propose 8 périodes en secondes (5,1 à 31,8 s), qui ne suivent pas le tempo ; aucune ne divise une autre. Les cinq dernières sont les boucles de *Music for Airports*. Quand le tempo change, un corps libre reste où il était (testé : 17,8 s par tour à 140 puis à 60 BPM, changement compris, à 3 ms près). Le premier tour après PLAY part du premier pas (jusqu'à un pas de décalage).
- **Les générateurs agissent sur les orbites** (ORBIT et MOTIF), avec leur mémoire déjà-vu, comme MOTION depuis le matin.
- **Six Mondes DEEP** (13 à 18) : COLD IN THE DARK (la vitrine à peu de pas), KEPLER NIGHT, TRAPPIST DUST, RAGA OF ASH, SLOW TIDES, LOOPS OF ASH. EMPTY SKY devient le 19e, avec sa graine d'avant. Niveaux mesurés sur une minute (les formes longues commencent par leur calme). Écoute : `M-VAVE FM1/ecoutes/rhizome-6-mondes-deep.m4a`.
- **Le filtre de la comète, fait le 3 novembre 2026 (avant le premier flash).** Un filtre qui se balaie avec la comète : fermé quand elle est loin, il s'ouvre à mesure qu'elle approche et s'ouvre en grand quand elle frôle l'étoile, puis se referme. Pistes pour le faire :
  - un nouvel ÉVÉNEMENT de la comète (ORBIT / COMET, K2), à côté de SWELL, FREEZE et BOTH, réglé par AMOUNT (K3) ;
  - un passe-bas sur tout le mélange (ou un décalage du CUTOFF de chaque piste), mené par la force du passage que le moteur calcule déjà (`orbit_comet_strength`) ;
  - à l'écran, la queue de la comète déjà dessinée, et le filtre qui s'ouvre visible (règle : chaque animation montre un vrai réglage) ;
  - tests : l'énergie des aigus monte au passage et redescend ; mesure du calcul.
  - **Résultat** : l'ÉVÉNEMENT FILTER (ORBIT / COMET, K2), un passe-bas 12 dB sur tout le mélange, de 18 kHz à 18 kHz / 2^(AMOUNT × 0,06) (280 Hz à 100). Mesuré : 281 Hz loin, 17,6 kHz au plus près, sept fois plus d'aigus au passage. À l'arrêt il est grand ouvert ; avec un autre événement il n'est pas calculé. La page COMET affiche la fréquence et sa jauge. KEPLER NIGHT l'utilise.
- *Shimmer* dans la HALL : écarté pour Rhizome (le 3 novembre 2026), au profit du filtre de la comète.

**La comète vagabonde** (3 novembre 2026, compilée pour le FM-1 le même soir) : K1 COMET = WANDER (orbit_comet 6, aucun champ de patch en plus). Elle revient au hasard (4 à 32 mesures, tiré de la graine) et, à chaque passage, choque le corps le plus proche de sa trajectoire : note sautée, corps poussé d'un pas, ou notes déplacées d'un ou deux degrés dans la gamme, pendant 1 à 5 notes (AMOUNT). Le choc passe au voisin avec une probabilité qui diminue de moitié à chaque maillon (60 % à AMOUNT 100). Les effets suivent l'ÉVÉNEMENT de la comète à son passage. Rien n'est écrit dans le patch ; même graine, mêmes chocs (testé : 10 passages, 18 chocs en deux minutes). Écoute : `M-VAVE FM1/ecoutes/rhizome-comete-vagabonde.m4a`. Compilée pour le FM-1 (cinquième compilation). **Les chocs secouent aussi les effets** (fait le même soir) : la HALL gonfle et la bande ralentit d'un coup puis repart, environ 1,5 s, d'autant plus que AMOUNT est haut.

Chaque étape se développe d'abord sur le Mac (`tools/build_local.py host`), puis dans le navigateur, puis sur le FM-1.
