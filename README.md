# Paper PSP

![PSP](https://img.shields.io/badge/PSP-003087?style=for-the-badge&logo=playstation&logoColor=white) 
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white) ![GitHub Release](https://img.shields.io/github/v/release/jeromeberg/paper-psp) 

## Description

A singleplayer Paper.io inspired homebrew game for PSP, written in C with SDL2 and PSPSDK.

Claim area by enclosing space on the map and compete with bots for territory. Cut through an opponent's trail to eliminate them. Cross your own trail or hit the border and you die.

## Screenshots

![demo](./.screenshots/demo.gif)

<img src=".screenshots/start.png" width="240"> <img src=".screenshots/paused.png" width="240">

## Installation

> ℹ️ Check out my [Awesome PSP](https://github.com/jeromeberg/awesome-psp) list of modding resources for PSP.


Download the [latest release](https://github.com/jeromeberg/paper-psp/releases/latest) and extract it into `/PSP/GAME/`.

## Build

### Prerequisites

- pspsdk
- psp-gcc
- psp-config
- SDL2

The easiest way is to install [pspdev](https://github.com/pspdev/pspdev), which includes everything needed.

### Instructions

```sh
make         # build
make re      # rebuild
make clean   # clean
```
