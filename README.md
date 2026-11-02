# Rhizome

*Un firmware ambient pour le second M-VAVE FM-1, au service de **When Day Chokes The Night**.*

Monades (premier FM-1, Flash Dog Duker Silver) fait les notes. Rhizome fait l'espace et le temps : un **séquenceur orbital** génératif, la **synthèse granulaire**, une **réverbe hall** dans l'esprit des grandes réverbes des années 1980 et un **delay à bande** avec ses bobines à l'écran. Il joue aussi très bien seul.

Le nom vient de Deleuze et Guattari (*Rhizome*, 1976) : un réseau sans centre, qui pousse par tous les bouts.

> **État (2 novembre 2026)** : le séquenceur orbital (lunes, comète, systèmes réels), le delay à bande, la réverbe HALL et le granulaire Beads tournent dans le simulateur. **Rhizome n'a pas encore été compilé ni mesuré sur un FM-1 : ne l'installez pas pour l'instant.**

- Site, avec le simulateur dans le navigateur : [dr-john-8bits.github.io/rhizome](https://dr-john-8bits.github.io/rhizome/)
- Mode d'emploi : [docs/rhizome/MODE-D-EMPLOI.md](docs/rhizome/MODE-D-EMPLOI.md) (le même [sur le site](https://dr-john-8bits.github.io/rhizome/manuel/))
- Feuille de route, faisabilité, budget mémoire et points de vigilance : [docs/rhizome/FEUILLE-DE-ROUTE.md](docs/rhizome/FEUILLE-DE-ROUTE.md)
- Maquette jouable de l'écran ORBIT (à ouvrir dans un navigateur) : [docs/rhizome/maquette/orbite.html](docs/rhizome/maquette/orbite.html)

## D'où vient le code
Rhizome est dérivé de [Monades](https://github.com/Dr-John-8bits/monades), lui-même dérivé de [Mutable Dreams](https://github.com/kidbee85-beep/mutable-dreams). L'historique complet est conservé. Le README de Monades est gardé dans [README-MONADES.md](README-MONADES.md), celui de Mutable Dreams dans [README-MUTABLE-DREAMS.md](README-MUTABLE-DREAMS.md).

Sources Git de ce dépôt :
- `origin` : Rhizome ;
- `monades` : pour récupérer les corrections communes (MIDI, gammes, plateforme) ;
- `upstream` : Mutable Dreams.

Le site (`site/`) est publié par `.github/workflows/pages.yml`, qui compile aussi le simulateur. Le mode d'emploi du site se régénère depuis le Markdown avec `python3 tools/site_manual.py`, puis `python3 tools/site_nav.py`.

## Crédits et licence
- **Monades** : Dr-John-8bits.
- **Mutable Dreams** : Kid Bunka / Christoph Link.
- **Mutable Instruments** : Émilie Gillet (Plaits, Rings, Elements, Clouds, Beads, stmlib, sous licence MIT ; Beads est dans `app/third_party/beads`, avec ses changements dans son `NOTICE.md`).
- **fm1-base** (Charles Vestal) et **Felucca / felucca-bsp** (Leo Kuroshita).

Distribution combinée sous **GPL-3.0-only**. Les notices MIT, MPL-2.0 et des polices sont conservées (voir [LICENSING-GPL.txt](LICENSING-GPL.txt)). Projet indépendant, sans lien avec Mutable Instruments, M-VAVE ni l'auteur de Mutable Dreams, et non approuvé par eux.
