# Les voix écartées, et pourquoi

*Rhizome, 3 et 4 novembre 2026. Ce document reste dans le dépôt : le site et le mode d'emploi ne présentent que ce que Rhizome fait aujourd'hui.*

## Ce qui a été essayé

Rhizome devait avoir ses propres voix, pour ne pas doubler Monades : huit moteurs de **Plaits 1.2** (Mutable Instruments), choisis pour l'ambient.

| Moteur | Ce qu'il apportait |
|---|---|
| STRING MACHINE | des cordes d'ensemble, façon Solina |
| WAVETABLE | des tables d'ondes qu'on parcourt |
| WAVE TERRAIN | un chemin qui survole un relief : des textures mouvantes |
| SIX-OP FM | les 96 sons de DX7 des banques de Plaits |
| SWARM | un essaim de huit oscillateurs qui dérivent |
| PHASE DISTORTION | la synthèse des Casio CZ |
| CHORDS | un accord entier, d'orgue ou de cordes |
| FORMANT | des voix sans paroles, des souffles |

Le granulaire était **Beads** (Mutable Instruments), le successeur de Clouds.

Dans le simulateur, tout sonnait très bien. Le problème est venu du **processeur du FM-1**.

## Ce que le FM-1 a mesuré

Comme pour Monades, la règle est que la charge du processeur **ne doit jamais atteindre 85 %**. Les mesures ont été faites sur l'appareil, par USB (`tools/measure_fm1.py`), Monde par Monde.

| Étape | Charge au plus fort des Mondes |
|---|---|
| Première compilation : huit moteurs de Plaits, Beads | 121 à 147 % |
| Allègement : voix en veille, Beads à 3 grains, 4 voix par Monde | 101 à 147 % |
| Le granulaire de Monades à la place de Beads, toutes les voix à demi-fréquence en ECO | 62 à 108 % |

Ce que chaque partie coûtait :
- **Beads** : 17 % sans rien faire, puis 50 à 70 % de plus dès que du son le traversait. C'est lui qui faisait exploser les premières mesures. Le granulaire de Monades, avec les mêmes réglages et les mêmes médias, coûte 5 à 12 %.
- **Une voix de Plaits qui joue** : 11 à 15 % du processeur, même calculée à demi-fréquence (environ 8 % pour le moteur, 6 % pour son filtre, son enveloppe et ses envois). Tous les moteurs se valaient à peu près.
- **La base, la bande et la HALL** : 7 % pour la base, 20 à 25 % pour la bande, la HALL et le mélange.

Un Monde de quatre voix de Plaits arrivait donc autour de 85 % avant même ses pics. Une passe de profilage sur l'appareil (le temps de chaque partie du rendu audio) n'a trouvé **aucun défaut unique** à corriger : KEPLER NIGHT à 95 % se répartissait entre séquenceur 5, moteurs 32, traitement autour des voix 25, grains 8, effets et mélange 24. Le coût était partout.

## La décision

Plutôt que de retirer des voix ou des effets, Rhizome reprend **les douze moteurs de Monades** (KICK, SNARE, HAT, FM, VA, RINGS, HARMONIC, CHOIR, BASS, ELEMENTS, MELLOW, SATIN), mesurés à 64–74 % sur le FM-1 de Monades avec six voix et ses effets. Il garde **deux voix de Plaits, SIX-OP FM et STRING MACHINE**, les plus utiles à l'ambient et les plus éloignées de ce que Monades sait faire. Le séquenceur orbital, la bande, la HALL et le granulaire ne changent pas.

Deux réglages ont suivi la première mesure :
- **SIX-OP FM calcule une seule voix au lieu de deux.** Plaits en garde deux pour qu'une note puisse finir pendant que la suivante commence ; une piste de Rhizome ne joue qu'une note à la fois, et la seconde voix doublait son coût (12 à 15 % au lieu de 6 à 7).
- **Une seule voix de Rhizome dans les Mondes les plus chargés.** Trois Mondes qui en avaient deux (COLD IN THE DARK, KEPLER NIGHT, RAGA OF ASH) dépassaient encore : ils ont été remplacés par six Mondes plus légers, les familles ECHO et VOICE.

## Le résultat

Mesuré sur le FM-1 le 4 novembre 2026 : **les 21 Mondes restent entre 60 et 78 % au plus fort, sans aucun décrochage.**

## Ce qu'on en garde

- Le code des moteurs écartés est toujours dans `app/third_party/plaits` (non compilé) ; deux corrections faites en route y restent : la carte des tables d'ondes de WAVETABLE était trop petite, et ses coordonnées pouvaient sortir de la table (voir `app/third_party/plaits/NOTICE.md`).
- Le banc d'essai sur le Mac (`tools/bench_rhizome.py`) sous-estimait la charge réelle d'un facteur 2 à 4 pour Plaits et Beads : sur le FM-1, seule la mesure sur l'appareil compte.
- Si un jour le FM-1 a de la marge (un Monde à trois voix, sans grains), un moteur comme WAVETABLE ou SWARM pourrait revenir pour un Monde précis. Le détail des mesures est dans la [feuille de route](FEUILLE-DE-ROUTE.md).
