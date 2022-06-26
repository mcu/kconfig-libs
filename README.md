# kconfig-libs

Build components into libraries like the early ESP-IDF but with a single makefile
Kconfig updated to linux version 5.6.

## Get project and build

```bash
  git clone https://github.com/mcu/kconfig-libs.git

  cd kconfig-libs
  make
```

## Problems to be solved

Get rid of the need to use goal 'all'. It is necessary to ensure the possibility
of using the dependency of one component on another.

## Comment

The project provided as is for those who can understand and continue development.
As you explore, you will discover interesting solutions.
