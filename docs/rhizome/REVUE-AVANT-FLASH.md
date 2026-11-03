# Rhizome : revue avant la première installation sur le FM-1

Revue du 2 novembre 2026, faite comme celle de Monades avant son premier flash (`REVUE-AVANT-FLASH.md` et `REVUE-RESULTATS.md` dans le dépôt de Monades). Elle porte sur ce que Rhizome ajoute à Monades 0.1.3 : le séquenceur orbital, la bande, la HALL, Beads et les écrans.

## Verdict
Deux défauts pouvaient planter l'appareil pendant le jeu. Ils sont corrigés. Il reste à **mesurer sur le FM-1** la pile du fil audio et la charge du processeur avant tout usage réel (voir la fin).

## Corrigé avant de flasher
1. **Débordement de la pile du fil audio.**
   - L'interruption audio du FM-1 dispose de 7 936 octets de pile, et la revue de Monades l'estimait déjà entre 4,4 et 6,2 Ko au pire.
   - Beads gardait ses tableaux de travail sur la pile. Les sorties de son générateur de « taps » occupent à elles seules 2 176 octets ; avec les médias à demi-fréquence (CLEAN et DIRTY TAPE), il faut en compter le double. Rhizome y ajoutait encore deux tableaux de 256 octets dans `quantum`.
   - Estimation : environ 7,5 Ko en DIGITAL et **plus de 11 Ko en CLEAN ou DIRTY TAPE**, donc un plantage en jouant.
   - **Correction** : ces tableaux vivent maintenant hors de la pile. Ce sont des statiques dans `processor.cc`, une zone de travail commune aux médias à demi-fréquence dans `granular_processor.h` (un seul média tourne à la fois, depuis le seul fil audio), et des statiques dans `dreams.cpp` et `beads_bus.inc`. Beads n'ajoute presque plus rien à la pile.
2. **Reconstruction de Beads pendant que l'audio tourne.**
   - Charger un Monde depuis l'écran ou par USB (`md_load`, donc `clear_audio`) reconstruisait l'objet Beads sur place pendant que le fil audio pouvait s'en servir. Le fil audio aurait alors suivi des pointeurs à moitié écrits.
   - **Correction** : Beads n'est construit qu'une fois, au démarrage (`md_init`). Ensuite, une remise à zéro vide seulement sa mémoire et demande au fil audio de le reconfigurer lui-même au bloc suivant.

## Vérifié sans problème
- **Multiplications 64 bits** (`bsp/docs/CPU.md`) : il n'y en a aucune dans le code ajouté (orbites, bande, HALL, pont de Beads, écrans), ni dans Beads.
- **Tableaux de taille variable** : il n'en reste aucun. Ceux de Beads ont été remplacés par des tailles fixes, et ses appels traitent au plus 32 trames.
- **Sauvegardes** : un emplacement de Monde occupe 28 + 1 976 octets, sous la limite de 3 840 par objet. Les emplacements et le patch sauvés par Monades (v10) se chargent, orbites coupées (testé).
- **Commandes** : celles de Rhizome commencent à 120, hors des codes 100 à 116 que gardent le navigateur et l'éditeur.
- **Audio USB** : la réserve agrandie de Monades 0.1.1 (`FELUCCA_UAC_DEEP`) est gardée.
- **Le reste** : le moteur, les 18 scénarios d'interface, les tests du moteur et du transfert passent, dont quatre scénarios sous sanitizers d'adresse et de comportement indéfini. Le contrôle du moteur dans le navigateur passe aussi sur GitHub.

## Lectures sans conséquence
Quelques lectures de l'état du moteur par l'écran sont faites sans verrou, comme dans Monades : les vues de la bande, de la HALL et des grains. Au pire, une image affiche une valeur d'un bloc plus tôt. Rien n'est écrit par l'écran dans l'état du son, sauf la décroissance des crêtes d'affichage des têtes de la bande, sans effet sur le son.

## À mesurer sur l'appareil, avant tout usage réel
1. **La charge du processeur, Monde par Monde, dans le pire cas** : bande, HALL et Beads ouverts ensemble, Beads en CLEAN ou DIRTY TAPE. **La limite de 85 % ne doit jamais être atteinte.** Si elle l'est, on renonce d'abord à la HALL (voir « Points de vigilance » dans la feuille de route).
2. **La pile réelle de l'interruption audio**, avec les mêmes réglages.
3. **L'audio USB** pendant ces essais : aucun craquement.
4. **La mémoire** : vérifiée à la compilation (ci-dessous).

## La compilation pour le FM-1 (2 novembre 2026)
Le paquet `app/build/rhizome-0.0.1.fwsc` fait 610 136 octets, identité `FM-1_968000001`. Les contrôles automatiques de la compilation passent :
- aucune multiplication 64 bits à côté d'une lecture ;
- aucune routine en double précision ;
- accès aux registres limités à felucca-bsp.

| Mémoire | Utilisé | Limite | Marge |
|---|---|---|---|
| Flash (image) | 561 720 octets | 602 112 | 40 Ko |
| RAM (.data + .bss) | 85 848 octets | 98 304 | 12 Ko |
| Pool | 276 852 octets | 344 064 | 66 Ko |

En route, trois corrections :
1. **Le compilateur du FM-1 n'a pas `<cassert>`** (Beads l'inclut) : un `app/compat/cassert` vide le remplace.
2. **Les écrans appelaient `fminf` et `fmaxf`**, absents en C sur le FM-1 : ils utilisent maintenant `fm_minf` et `fm_maxf`.
3. **La flash dépassait de 9 724 octets.**
   - La table des 91 Mondes d'usine stockait chacun en patch complet de Rhizome, soit 296 octets de zéros par Monde (27 Ko en tout). Elle garde maintenant le format de Monades (1 680 octets par Monde) ; les champs de Rhizome sont posés au chargement, comme pour une sauvegarde de Monades.
   - Le média VINTAGE de Beads est retiré : 2,5 Ko. Sans son convertisseur à 32 kHz, ce n'était qu'un DIGITAL en 12 bits.

## Deuxième compilation (3 novembre 2026)
Avec l'accueil et les 12 Mondes de Rhizome. Les mêmes contrôles passent. L'image tombe à **434 232 octets** (−127 488) : il reste 168 Ko de flash. RAM 85 896 sur 98 304, pool 276 852 sur 344 064. Le paquet `rhizome-0.0.1.fwsc` garde sa taille (610 136 octets, complété à la taille fixe).

## Troisième compilation (3 novembre 2026)
Avec les huit voix de Plaits à la place de celles de Monades. Les mêmes contrôles passent. L'image fait **475 160 octets** : il reste 127 Ko de flash. La RAM descend à **77 512 octets** sur 98 304 (Elements et Rings retirés). Le pool monte à **311 564** sur 344 064 : il reste 32 Ko, dont 8 à garder libres. Le paquet est copié dans `M-VAVE FM1/flash/rhizome-0.0.1-voix.fwsc`, c'est celui à installer.

## Quatrième compilation (3 novembre 2026)
Avec le REWIND, les périodes libres, les générateurs sur les orbites, les six Mondes DEEP et le filtre de la comète. Les mêmes contrôles passent. Image 489 132 octets (113 Ko de flash libres), RAM 77 576 sur 98 304, pool 311 564 sur 344 064. Le paquet `M-VAVE FM1/flash/rhizome-0.0.1-voix.fwsc` est remplacé par cette version : c'est celle à installer.

## Cinquième compilation (3 novembre 2026)
Avec la comète vagabonde (WANDER) et ses chocs sur les effets. Les mêmes contrôles passent. Image 491 568 octets (110 Ko de flash libres), RAM 77 640 sur 98 304, pool 311 564 sur 344 064. Le paquet `M-VAVE FM1/flash/rhizome-0.0.1-voix.fwsc` est remplacé par cette version : c'est celle à installer.
