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

### Systèmes réels comme presets
Des rapports d'orbites mesurés, qui sonnent très bien parce qu'ils sont en résonance :
- **Io, Europe, Ganymède** : 1:2:4 (la résonance de Laplace) ;
- **Kepler-223** : quatre planètes en chaîne de résonances ;
- **TRAPPIST-1** : sept planètes en chaîne proche de 8:5:3:2 ;
- **Pluton et Neptune** : 2:3.

À vérifier dans les données publiées avant de les figer. Chaque preset affiche son nom et ses rapports.

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

### Réglages (idée, K1 à K4)
- **Page ORBIT** (par corps) : PERIOD (rapport ou division), ECCENTRICITY, GATES (nombre de portes), PHASE.
- **Page SYSTEM** : PRECESSION, PERTURBATION, MOONS, COMET.
- **Page PITCH** : SOURCE (ORBIT / MOTIF / ÉCHO), gamme, registre, conjonction → accord.

### Coût
L'équation de Kepler se résout avec une table (anomalie moyenne → position, 256 valeurs par excentricité) ou deux itérations de Newton, une fois par bloc de 64 échantillons (1,45 ms). La précision des coups est donc d'un bloc, comme dans Monades. Coût de calcul négligeable.

## Les sons

### Le granulaire : Beads au centre
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
| Granulaire Beads réduit | 🟡 pas encore mesuré : le plus gros risque |
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

**Ce que ça donne pour Rhizome**, avec par exemple STRING MACHINE, WAVETABLE, SIX-OP et CHORDS :
- les quatre voix : environ 24 % ;
- le HALL, la bande et les orbites : environ 8 % ;
- le mélange, les filtres et les LFO : quelques % de plus.

On arrive autour de 35 à 40 %. Il reste donc **environ 45 % pour Beads** sous le plafond de 85 % que Monades s'impose.

**Beads n'est pas encore mesuré.** Le banc sait le faire (`--beads` : chemin d'une copie de `pichenettes/eurorack`, compilé avec ses 32 grains puis avec 8). Mais compiler et lancer du code téléchargé demande une autorisation explicite.

**Une découverte en route** : les autres moteurs de Plaits 1.2 et Rings demandent quelques fonctions que le FM-1 n'avait pas (`std::copy`, `std::sort`, `std::rotate`, `fabs` en simple précision). Elles sont ajoutées dans `app/compat`, en version courte et sans `double`.

### Le risque connu de la plateforme
D'après `bsp/docs/CPU.md`, des multiplications 64 bits mêlées à des lectures 16 bits peuvent corrompre un registre sur ce processeur. Le code en virgule flottante (Plaits, Rings, Beads le sont surtout) n'est pas concerné. Il faudra quand même chercher ces motifs dans le désassemblage du code de Rhizome.

## Le code et GitHub
- Rhizome est **dérivé de Monades**. Mais **GitHub ne permet pas de forker son propre dépôt dans le même compte**. Donc on crée un nouveau dépôt `Dr-John-8bits/rhizome` et on y pousse l'historique de Monades. Monades reste une « source » (un remote `monades`) d'où l'on récupère les corrections communes (MIDI, gammes, plateforme).
- Même plateforme fm1-base, même chaîne de compilation, mêmes outils.
- Licence héritée : GPL-3.0-only, avec les notices MIT (Mutable Instruments, Parasites), MPL-2.0 (felucca-bsp) et les crédits de Mutable Dreams et Monades.

## Ordre conseillé
0. **Maquette jouable de l'écran ORBIT dans le navigateur** (240 × 240, avec le son) : valider la lisibilité et l'envie de jouer avant d'écrire le firmware. **Faite le 2 novembre 2026** : `docs/rhizome/maquette/orbite.html`, publiée sur [claude.ai](https://claude.ai/artifact/2Q3b1BXKckoXwbATJbcFHZ) (privée). La mécanique est la vraie : équation de Kepler, portes euclidiennes, précession, lunes, comète, conjonctions. Le son n'est qu'une esquisse dans le navigateur. Six systèmes : Kepler-223, Io-Europe-Ganymède, TRAPPIST-1, Pluton-Neptune, Euclidien, Dérive.
1. **Banc d'essai du calcul** : chaque bloc mesuré sur le Mac, puis sur le FM-1. Il décide du nombre de voix et de la taille de Beads. **Sur le Mac, le 2 novembre 2026, sans Beads** : voir « Banc d'essai sur le Mac ». Restent Beads, puis la confirmation sur le FM-1.
2. **Création du dépôt** à partir de Monades. **Fait le 2 novembre 2026** : dépôt privé `Dr-John-8bits/rhizome`, identité `FM-1_968`, noms USB et écrans Rhizome. Le préfixe `FM-1_976` prévu au départ était déjà pris par LSDj Burrito ; `FM-1_968` renvoie à *Différence et répétition* (Deleuze, 1968). Le grand nettoyage (retirer les 91 Mondes, les écrans et le site de Monades) se fera au moment où le moteur de Rhizome les remplacera : le moteur actuel en a besoin pour tourner.
3. **Séquenceur orbital** dans le simulateur, puis sur le FM-1.
4. **Delay à bande** et son animation.
5. **Réverbe HALL**.
6. **Granulaire Beads**.
7. **Voix Plaits 1.2 et Rings.**

Chaque étape se développe d'abord sur le Mac (`tools/build_local.py host`), puis dans le navigateur, puis sur le FM-1.
