# Rhizome

*Un firmware ambient pour le second M-VAVE FM-1, au service de **When Day Chokes The Night**.*

Monades (premier FM-1, Flash Dog Duker Silver) fait les notes. Rhizome fait l'espace et le temps : un **séquenceur orbital** génératif, la **synthèse granulaire**, une **réverbe hall** dans l'esprit des grandes réverbes des années 1980 et un **delay à bande** avec ses bobines à l'écran. Il joue aussi très bien seul.

Le nom vient de Deleuze et Guattari (*Rhizome*, 1976) : un réseau sans centre, qui pousse par tous les bouts.

> **État (2 novembre 2026)** : idée, feuille de route et maquette. Le code est encore celui de **Monades 0.1.3**, dont Rhizome repart. Rien n'est à installer pour l'instant.

- Feuille de route, faisabilité et budget mémoire : [docs/rhizome/FEUILLE-DE-ROUTE.md](docs/rhizome/FEUILLE-DE-ROUTE.md)
- Maquette jouable de l'écran ORBIT (à ouvrir dans un navigateur) : [docs/rhizome/maquette/orbite.html](docs/rhizome/maquette/orbite.html)

## D'où vient le code
Rhizome est dérivé de [Monades](https://github.com/Dr-John-8bits/monades), lui-même dérivé de [Mutable Dreams](https://github.com/kidbee85-beep/mutable-dreams). L'historique complet est conservé. Le README de Monades est gardé dans [README-MONADES.md](README-MONADES.md), celui de Mutable Dreams dans [README-MUTABLE-DREAMS.md](README-MUTABLE-DREAMS.md).

Sources Git de ce dépôt :
- `origin` : Rhizome ;
- `monades` : pour récupérer les corrections communes (MIDI, gammes, plateforme) ;
- `upstream` : Mutable Dreams.

La publication du site de Monades (`.github/workflows/pages.yml`) a été retirée : Rhizome aura son propre site plus tard.

## Crédits et licence
- **Monades** : Dr-John-8bits.
- **Mutable Dreams** : Kid Bunka / Christoph Link.
- **Mutable Instruments** : Émilie Gillet (Plaits, Rings, Elements, Clouds, Beads, stmlib, sous licence MIT).
- **fm1-base** (Charles Vestal) et **Felucca / felucca-bsp** (Leo Kuroshita).

Distribution combinée sous **GPL-3.0-only**. Les notices MIT, MPL-2.0 et des polices sont conservées (voir [LICENSING-GPL.txt](LICENSING-GPL.txt)). Projet indépendant, sans lien avec Mutable Instruments, M-VAVE ni l'auteur de Mutable Dreams, et non approuvé par eux.
