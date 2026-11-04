# Retour d'expérience : Rhizome, 3 et 4 novembre 2026

*Ce que deux journées de travail sur Rhizome ont appris, pour un prochain firmware du FM-1. Les chiffres viennent des mesures sur l'appareil ; le détail est dans la [feuille de route](FEUILLE-DE-ROUTE.md) et dans [les voix écartées](VOIX-ECARTEES.md).*

## En bref

- Seule la mesure sur le FM-1 compte, et elle se fait en premier.
- Le budget du processeur se pense en voix : environ 40 % partent avant la première note.
- Les coûts cachés (le traitement autour d'une voix, la polyphonie interne d'un moteur, le chargement d'un Monde) pèsent autant que ce qu'on voit.
- Une enquête a un budget, et une solution de repli est prête avant de la commencer.
- Les tests vérifient le temps qui passe et les gestes réels, pas seulement « ça sonne ».
- Une compilation dure 25 minutes : on les regroupe.
- Les retours de jeu trouvent ce que les tests ne voient pas ; l'écran doit dire pourquoi il ne fait pas ce qu'on attend.

## 1. Mesurer sur l'appareil, et tout de suite

Dans le simulateur, les voix de Plaits et Beads sonnaient très bien. Sur le FM-1, la première compilation montait à **121 à 147 %** du processeur. Le banc d'essai sur le Mac sous-estimait la charge d'un facteur 2 à 4 pour Plaits et Beads : il donne une tendance, jamais un verdict.

Ce qui a débloqué la situation, ce sont deux outils construits en route :
- **la mesure par USB, Monde par Monde** (`tools/measure_fm1.py`, SysEx de diagnostic) : charge, pic, décrochages, blocs trop longs ;
- **le profilage** : le temps de chaque partie du rendu audio (séquenceur, moteurs, traitement des voix, grains, effets), mesuré sur l'appareil.

**Pour un prochain firmware** : l'étape zéro est de poser le cœur sonore sur l'appareil et de le mesurer, avant d'écrire la moindre page d'écran.

## 2. Le budget réel du FM-1

| Partie | Coût mesuré |
|---|---|
| Base (séquenceur, écran, système) | ~7 % |
| Bande, HALL et mélange | 20 à 25 % |
| Grains de Monades | 5 à 12 % |
| Traitement autour d'une voix (filtre, enveloppe, envois) | ~6 % par voix qui sonne |
| Moteur de Monades (FM, VA, chœur, basse…) | 2 à 5 % |
| Moteur « moderne » de Plaits, même à demi-fréquence | ~8 % |
| SIX-OP avec ses deux voix internes / avec une seule | 12 à 15 % / 6 à 7 % |
| Beads | 17 % à vide, puis 50 à 70 % de plus dès qu'il reçoit du son |

Il faut **viser 70 % au pic, pas 85 %** : il y a toujours 5 à 10 points entre la charge moyenne et le pic, et quelques blocs isolés plus longs. Avec les effets de Rhizome, cela donne **trois ou quatre voix par Monde**.

## 3. Les coûts cachés

- **Le traitement autour d'une voix** (son filtre, son enveloppe, ses envois) coûte autant que beaucoup de moteurs. Une voix « légère » ne l'est jamais tout à fait.
- **Un moteur peut calculer plusieurs voix en interne** : SIX-OP en calculait deux, pour qu'une note finisse pendant que la suivante commence. Une piste de Rhizome ne joue qu'une note à la fois ; passer à une voix a divisé son coût par deux.
- **Ce qui marche à deux voix peut casser à une.** Avec une seule voix DX7, la « gate » restait haute d'une note à l'autre et les enveloppes ne repartaient plus : seule la première note sonnait. Une simplification se re-teste dans la durée.
- **Charger un Monde** efface 128 Ko de mémoire (grains, bande, HALL) dans un seul bloc audio. Ce bloc trop long déclenchait une fausse alerte de surcharge, qui coupait le chorus et la diffusion pour rien.
- **La mémoire, elle, n'a jamais été le problème** : il restait 24 à 29 Ko de réserve, en construisant chaque moteur dans une petite zone par voix.

## 4. La méthode

- **Une enquête a un budget.** Le plan de repli (les moteurs de Monades, SIX-OP et STRING MACHINE, les effets de Rhizome) a été décidé *avant* le profilage, avec une règle simple : si le profil ne désigne pas une cause nette, on bascule. Le profil n'a trouvé aucun défaut unique, on a basculé, et la journée a été sauvée.
- **Les tests vérifient le temps et les gestes.** Le bug de SIX-OP est passé parce que le test vérifiait seulement que la voix sonnait. Désormais : des notes répétées sur plusieurs secondes, des minutes de lecture, des niveaux mesurés sur une minute, et le tutoriel du mode d'emploi rejoué geste par geste (`tests/tutorial_ui.txt`).
- **On groupe les compilations.** Une compilation dure 25 minutes, un flash et une mesure 20 de plus. On valide tout sur le Mac, puis une seule compilation, un seul flash, une seule mesure.
- **On équilibre automatiquement.** Les niveaux des Mondes sont mesurés par les tests, puis corrigés par un gain par Monde : chaque changement de voix les déplace.
- **Le jeu trouve ce que les tests ne voient pas.** HITS qui revient à 0 (une orbite qui a la main), l'alerte ECO au-delà du dernier Monde, la porte mal expliquée : trois retours de jeu, trois corrections. L'écran doit dire pourquoi il ne fait pas ce qu'on attend (« ORBIT PLAYS THIS TRACK: SET GATES TO 0 »).
- **Le site montre ce qui marche.** Ce qu'on a essayé et écarté reste dans le dépôt, documenté, pas sur le site.

## 5. Les petits pièges de l'atelier

- Les commandes qui parlent au FM-1 échouent parfois une première fois (CoreMIDI, erreur -304) : on réessaie.
- macOS garde en cache le nom USB d'un appareil : le FM-1 de Rhizome apparaît encore comme « Monades FM-1 » dans la liste MIDI, alors qu'il s'annonce « Rhizome FM-1 ».
- Le générateur du mode d'emploi doit connaître tous les niveaux de titres qu'on utilise : un titre inconnu l'a fait tourner en boucle.

## 6. Et après Rhizome ?

Un firmware « Beads seul » serait plausible mais **sans doute peu utile** : Beads prendrait à lui seul 70 à 90 % du FM-1 dès qu'il reçoit du son, il lui faudrait une source (des voix internes très légères, ou le son de l'ordinateur par USB, un sens qui n'est pas compilé aujourd'hui), et le granulaire de Monades fait déjà l'essentiel pour bien moins cher.

La piste retenue : **regarder ailleurs dans le code de Mutable Instruments, ou au-delà**, ce qui apporterait une idée nouvelle au FM-1 pour un coût mesurable. Quel que soit le choix, le prochain firmware partirait de la même base : fm1-base, l'outil de mesure et le profilage, les tests d'écran, et le site avec son mode d'emploi cherchable. Et il commencerait par une journée de mesures sur l'appareil.
