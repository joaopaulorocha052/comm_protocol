# ⚡ Comm Protocol — Sistema de Monitoramento de Cercas Elétricas com LoRa

![Status](https://img.shields.io/badge/status-ativo-success)
![Platform](https://img.shields.io/badge/platform-RP2040-orange)
![Language](https://img.shields.io/badge/language-C%2FC%2B%2B-green)

---

## 📖 Resumo

O **Comm Protocol** é um sistema de **baixo custo e baixo consumo de energia** para detecção de falhas em cercas elétricas, desenvolvido como parte da disciplina **Projetos em Engenharia de Computação**.

O projeto visa proteger plantações e animais contra invasores, como javalis, alertando imediatamente quando há interrupção na cerca elétrica. Ele combina **hardware simples**, **firmware otimizado em C/C++** e **comunicação via LoRa** para operação em áreas rurais isoladas.

> 🎯 **Impacto Social**: Aumenta a segurança rural, reduz prejuízos e fortalece a autonomia de pequenos produtores, alinhando-se aos ODS 7, 10, 11 e 12.

---

## 📚 Contexto Acadêmico

Este projeto foi concebido e implementado como parte da disciplina **Projetos em Engenharia de Computação**, com abordagem híbrida de desenvolvimento (Waterfall + Kanban) e validação em laboratório.

O trabalho engloba:

- Desenvolvimento de **circuito analógico** para sensoriamento seguro de alta tensão.
- Desenvolvimento de **firmware embarcado** para acoplar ao sensoriamento.
- Documentação e entrega de **prototipagem funcional**.

---

## 🎯 Objetivos

- Detectar quedas abruptas de tensão na cerca elétrica.
- Enviar alertas usando comunicação **LoRa**.
- Garantir confiabilidade com **protocolo de confirmação (ACK)**.
- Operar por longos períodos com baixo consumo energético.

---

## 🛠 Tecnologias e Componentes

- **MCU**: Raspberry Pi Pico W / RP2040 com LoRa SX1262
- **Linguagem**: C/C++ (Pico SDK)
- **Comunicação**: LoRa (modulação de espectro espalhado)
- **Circuito Sensor**: Resistor de alto valor, capacitor e retificador de onda completa
- **Ferramentas**: CMake, Ninja, Trello, GitHub, Osciloscópio, Multímetro

---

## 🖧  Arquitetura do Sistema

![Arquitetura](docs/images/architecture_diagram.png)

- **Transmissor**: Conectado à cerca elétrica, detecta quedas abruptas de tensão via circuito analógico e envia alerta via LoRa.
- **Receptor**: Recebe o alerta e notifica o usuário via LED, som ou ambos.

---

## 📡 Protocolo de Comunicação com Confirmação (ACK)

O sistema implementa um **protocolo de confirmação** para garantir a entrega confiável das mensagens.

| Componente   | Função                                            |
| ------------ | --------------------------------------------------- |
| **TX** | Envia alerta e aguarda confirmação do RX.         |
| **RX** | Recebe alerta e envia confirmação de volta ao TX. |

<div style="display: flex; gap: 10px;">
  <div style="flex: 1; text-align: center; border: 1px solid #ccc; border-radius: 10px; padding: 10px;">
    <strong>Transmissor (TX)</strong><br>
    <img src="docs/images/protocol_flowchart_tx.png" width="100%">
  </div>
  <div style="flex: 1; text-align: center; border: 1px solid #ccc; border-radius: 10px; padding: 10px;">
    <strong>Receptor (RX)</strong><br>
    <img src="docs/images/protocol_flowchart_rx.png" width="100%">
  </div>
</div>

---

## 📊 Consumo de Energia

| Estado               | Corrente Média | Potência Aproximada |
| -------------------- | --------------- | -------------------- |
| Transmissão (TX)    | 114 mA          | 65 mW                |
| Processamento (Idle) | 32 mA           | 5,1 mW               |

> O sistema só consome energia relevante durante o envio de alertas, permitindo autonomia de semanas usando pilhas comuns.

---

## 🖼 Cartilha de Uso

![Cartilha de Uso](docs/images/cartilha_uso.png)
📄 [Baixar cartilha em alta resolução](docs/images/cartilha_uso.png)

---
