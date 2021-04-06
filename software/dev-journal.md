# Dev journal

### 2021-02-28
- configuration du TAS via I2C:
    - l'algo donné dans la datasheet ne semble pas complet
    - récup et essai de configs (*trouvées sur le net + mon italien)
        -> ça marche
    - à nouveau demandé à tI un lien vers PurePathConsole3 car la version quej'avais ne fonctionne plus (pas possible de récup les données depuis le net)
   - il semble qu'il y a une config spéciale PBTL dans les registres (si on en croit les videos de PurePathConsole3), j'attends d'avoir le sw et de fabriquer cette config
- niveau sonore
    - à volume = 0dB, la conso est de ~0.20A pour 30V = 6W
    - à volume = -8dB, la conso est de ~0.02A pour 30V = 0.6W
    - ça ne me semble pas normal, on est loin des 600W
    - essayé avec plusieurs config différentes de TAS, pareil, est-ce par ce qu'on a pas de config PBTL ?
    - à l'oreille, le volume fonctionne mais c'est pas linéaire, il faudra surement entrer dans du volume fin (possible avec le DSP)
- mute
    - pour entendre du son il faut placer la pin MUTE au niveau bas, mais à ce moment-là la LED MUTE s'allume
    - il faut probablement changer le sens de la LED mute
    - sinon mute et rst marchent bien
- il y a bien des registres CLOCK_DETECTION_CONFIG et PLL_CLOCK_CONFIGURATION pour ignorer MCLK et prendre la ref de la PLL sur SCLK
    -> pas besoin de MCLK pour piloter du son (et ça marche !)
    -> par contre
        - le chapitre 8.3.1 Power-on-Reset (POR) Function de tas3251.pdf dit qu'il faut MCLK, SCLK et LRCK/FS qui bagotent pendant 4ms pour sortir du POR
            -> peut-être qu'il faut MCLK quand même au moins au démarrage
        - 8.3.3 dit ":In situations where the highest audio performance is required, bringing MCLK to the device along with SCLK and LRCK/FS is recommended"

### 2021-03-01
- PPC3 option PBTL (1 channel), Standard Processing, 

### 2021-03-02
- new sample (sin, 440Hz, 48000, 16b, mono, amplitude 1)
- clipping déclenché, pourquoi ?

### 2021-03-03
- USB Audio 1.0 d'après STM32CubeF4_FW_F4_V1.24.0, projet STM324xG_EVAL (FS)
