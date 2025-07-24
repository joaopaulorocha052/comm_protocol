# Teste inicial

A ideia é fazer duas placas rp2040 se comunicarem da maneira mais simples possível

Esse código esta baseado no projeto "ping-pong" na pagina de exempos do waveshare - https://www.waveshare.com/wiki/RP2040-LoRa.

A pasta inteira - https://files.waveshare.com/wiki/RP2040-LoRa/RP2040-LoRa-TestCode.zip


## Preparar o projeto

```
mkdir build
cd build
cmake ../ -G Ninja
```

## Compilar sem vscode:

```
cd build
ninja
```

## Referencias:

- pico-sdk: https://github.com/raspberrypi/pico-sdk
- pico examples: https://github.com/raspberrypi/pico-examples/
- Getting Started Pico - https://rptl.io/pico-get-started
- picotool udev roles: https://github.com/raspberrypi/picotool/blob/master/udev/99-picotool.rules
- sx1262: https://www.semtech.com/products/wireless-rf/lora-connect/sx1262

Feito com ajuda do San @spiccinini