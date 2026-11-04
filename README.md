# Rhizome

*Un firmware ambient pour le M-VAVE FM-1, au service de **When Day Chokes The Night**.*

Rhizome fait l'espace et le temps :
- un **séquenceur orbital** : des corps tournent autour d'une étoile et jouent quand ils passent leurs portes. Une orbite ronde joue un rythme euclidien, une orbite ovale le même rythme avec du rubato (la deuxième loi de Kepler). Des lunes ornent les notes, une comète passe rarement, les corps alignés frappent des accords, et de vrais systèmes planétaires se chargent sur les pistes ;
- **quatorze voix**, dont deux propres à Rhizome : **SIX-OP FM** (les sons des DX7) et **STRING MACHINE** (les cordes d'ensemble des années 70) ;
- un **delay à bande** à trois têtes, avec ses bobines à l'écran, son usure et son REWIND ;
- une **réverbe HALL** dans l'esprit des grandes réverbes numériques des années 80 ;
- un **nuage de grains**.

**30 Mondes**, en neuf familles (DUSK, NIGHT, DAWN, DEEP, ECHO, VOICE, RAGA, MAQAM, EAST). Mesuré sur le FM-1 : de 60 à 77 % du processeur au plus fort, sans aucun décrochage (la limite est 85 %).

Rhizome est le compagnon de [Monades](https://github.com/Dr-John-8bits/monades), l'autre FM-1 du projet, qui fait les notes et le rythme. Il joue aussi très bien seul.

Le nom vient de Deleuze et Guattari (*Rhizome*, 1976) : un réseau sans centre, qui pousse par tous les bouts.

- Site, avec le simulateur dans le navigateur : [dr-john-8bits.github.io/rhizome](https://dr-john-8bits.github.io/rhizome/)
- Mode d'emploi : [docs/rhizome/MODE-D-EMPLOI.md](docs/rhizome/MODE-D-EMPLOI.md) (le même [sur le site](https://dr-john-8bits.github.io/rhizome/manuel/))
- Pourquoi d'autres voix ont été essayées puis écartées : [docs/rhizome/VOIX-ECARTEES.md](docs/rhizome/VOIX-ECARTEES.md)
- Feuille de route, mesures sur le FM-1, budget mémoire : [docs/rhizome/FEUILLE-DE-ROUTE.md](docs/rhizome/FEUILLE-DE-ROUTE.md)

## Construire et installer
- Tests sur le Mac : `python3 tools/build_local.py dsp` puis `app/build/native/dsp-tests` (depuis `tests/`), de même avec `next` ; les scripts d'écran de `tests/*.txt` se jouent avec `app/build/native/mutable-dreams-host` (`python3 tools/build_local.py host`).
- Firmware : `sh tools/build_device.sh` (chaîne JieLi), qui produit `app/build/rhizome-0.0.1.fwsc`, identité FM-1_968000001.
- Installation par USB : `python3 base/tools/fm1_install.py <paquet> --yes`.
- Charge du processeur, Monde par Monde, sur l'appareil : `python3 tools/measure_fm1.py`.

## D'où vient le code
Rhizome est dérivé de [Monades](https://github.com/Dr-John-8bits/monades), lui-même dérivé de [Mutable Dreams](https://github.com/kidbee85-beep/mutable-dreams). L'historique complet est conservé. Le README de Monades est gardé dans [README-MONADES.md](README-MONADES.md), celui de Mutable Dreams dans [README-MUTABLE-DREAMS.md](README-MUTABLE-DREAMS.md).

Sources Git de ce dépôt :
- `origin` : Rhizome ;
- `monades` : pour récupérer les corrections communes (MIDI, gammes, plateforme) ;
- `upstream` : Mutable Dreams.

Le site (`site/`) est publié par `.github/workflows/pages.yml`, qui compile aussi le simulateur. Le mode d'emploi du site se régénère depuis le Markdown avec `python3 tools/site_manual.py`, puis `python3 tools/site_nav.py`.

## Crédits et licence
- **Rhizome** et **Monades** : Dr-John-8bits.
- **Mutable Dreams** : Kid Bunka / Christoph Link.
- **Mutable Instruments** : Émilie Gillet (Plaits, Rings, Elements, Clouds, stmlib, sous licence MIT ; les changements faits pour Rhizome sont listés dans le `NOTICE.md` de chaque dossier de `app/third_party`).
- **fm1-base** (Charles Vestal) et **Felucca / felucca-bsp** (Leo Kuroshita).

Distribution combinée sous **GPL-3.0-only**. Les notices MIT, MPL-2.0 et des polices sont conservées (voir [LICENSING-GPL.txt](LICENSING-GPL.txt)). Projet indépendant, sans lien avec Mutable Instruments, M-VAVE ni l'auteur de Mutable Dreams, et non approuvé par eux.
