# Protocolo de Confirmação

O projeto consiste em implementar uma solução para a detecção de falhas de funcionamento e curtos em cercas elétricas rurais, utilizando microcontroladores rp2040-LoRa. 
São utilizados 2 microcontroladores, sendo um o Transmissor(TX), que lê a tensão da cerca e informa outro, Receptor(RX) por meio de comunicação utilizando LoRa.


## Preparar o projeto
Em uma pasta externa ao projeto, clone o pico sdk.

```
git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule update --init
```

Então prepare o build

```
mkdir build
cd build
cmake ../ -G Ninja
```

## Para compilar o projeto

```
cd build
make
```

## Para carregar o código nos controladores

```
cd pico-transmitter
```
ou

```
cd pico-receiver
```
Após isso, é preciso copiar os arquivos para o microcontrolador. Mas antes, conecte o micro controlador ao computador segurando o botão de boot. Ao soltar o botão, ele se conectará como unidade de memória.

```
cp pico-transmitter.uf2 /media/<seu-usuario>/<nome-da-uniade-de-memoria-conectada>
```

ou

```
cp pico-receiver.uf2 /media/<seu-usuario>/<nome-da-uniade-de-memoria-conectada>
```
## Referencias:

- pico-sdk: https://github.com/raspberrypi/pico-sdk
- pico examples: https://github.com/raspberrypi/pico-examples/
- Getting Started Pico - https://rptl.io/pico-get-started
- picotool udev roles: https://github.com/raspberrypi/picotool/blob/master/udev/99-picotool.rules
- sx1262: https://www.semtech.com/products/wireless-rf/lora-connect/sx1262

Feito com ajuda do San @spiccinini
