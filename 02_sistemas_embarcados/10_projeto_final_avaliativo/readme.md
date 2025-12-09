# Projeto: Detector de Vazamento de Gás com ESP32 e MQTT

## 1. Objetivo do Projeto

Este projeto é desenvolvido como parte da **avaliação bimestral** da disciplina de Introdução a Sistemas Embarcados. O objetivo principal é aplicar a metodologia de desenvolvimento de sistemas embarcados para criar a **especificação funcional, técnica e lógica** completa de um dispositivo que **resolve um problema real**.

O problema real escolhido é o perigo de vazamentos de Gás Liquefeito de Petróleo (GLP) em ambientes residenciais, que podem não ser detectados a tempo e causar asfixia, incêndios ou explosões.

Como solução, este documento especifica um sistema de monitoramento e alerta baseado no **ESP32**. O sistema utiliza um sensor de gás da série MQ para identificar concentrações perigosas, aciona alertas locais imediatos (sonoros/visuais) e transmite notificações remotas via protocolo MQTT sobre uma rede Wi-Fi.

O restante deste documento detalha os itens de especificação exigidos:

* **Definição Técnico-Funcional** (desdobrada nas seções "O Problema Solucionado", "Arquitetura do Sistema").
* **Descrição do Hardware** (na seção "Pilha Tecnológica").
* **Descrição da Função Lógica** (desdobrada nas seções "Arquitetura do Sistema" e "Pilha Tecnológica (Periféricos)").

## 2. O Problema Solucionado

Vazamentos de gás de cozinha (GLP) em ambientes residenciais representam um risco crítico e imediato à segurança, podendo resultar em asfixia, incêndios ou explosões. A detecção puramente humana através do olfato é falível, especialmente em casos de ocupantes com olfato reduzido, durante o sono, ou em vazamentos lentos e graduais.

Este projeto propõe uma solução de engenharia automatizada para mitigar este risco. O sistema oferece monitoramento contínuo (24/7) e uma resposta de alerta em duas camadas: um alarme local (sonoro/visual) para evacuação imediata e uma notificação remota (MQTT) que pode alertar o proprietário ou serviços de emergência, independentemente de sua localização.

## 3. Arquitetura do Sistema

O firmware é desenvolvido utilizando o **ESP-IDF (Espressif IoT Development Framework)**, com uma arquitetura multitarefa baseada no **FreeRTOS**. Esta abordagem garante que as operações de sensoriamento, atuação e comunicação de rede operem de forma independente e eficiente.

A arquitetura lógica se baseia em duas tarefas principais que se comunicam através de uma Fila (Queue) do FreeRTOS, garantindo uma operação *event-driven* (orientada a eventos) e não bloqueante:

1.  **Tarefa de Sensoriamento (Sensor Task):**
    * Responsável por configurar e realizar leituras periódicas do sensor de gás MQ-5 através do periférico **ADC1** (12 bits).
    * Processa o valor "cru" (raw) lido e o compara com um limiar de segurança (threshold) pré-definido.
    * Ao detectar uma mudança de estado (de "Seguro" para "Perigo" ou vice-versa), esta tarefa **envia** uma mensagem para a fila de alertas.
    * Esta tarefa passa a maior parte do tempo em estado bloqueado (`vTaskDelay`), cedendo o processador.

2.  **Tarefa de Atuação (Buzzer/Alert Task):**
    * Responsável por controlar os alertas locais.
    * (Em desenvolvimento) Esta tarefa permanece bloqueada (`xQueueReceive`) aguardando uma mensagem na fila de alertas.
    * Ao receber uma mensagem, ela a interpreta e aciona (ou desaciona) o buzzer através do periférico **LEDC (PWM)**.
    * Esta arquitetura garante resposta imediata ao evento sem sobrecarregar o loop de sensoriamento com delays.

3.  **Módulos de Conectividade (Wi-Fi e MQTT):**
    * (Em desenvolvimento) Módulos dedicados para gerenciar a conexão Wi-Fi e a comunicação com o broker MQTT. A tarefa MQTT futuramente também "ouvirá" a fila de alertas para publicar as notificações.

## 4. Pilha Tecnológica (Technology Stack)

* **Hardware:**
    * Microcontrolador: ESP32 Dev Kit V1
    * Sensor: Módulo Sensor de Gás MQ-5 (com divisor de tensão para proteção de 3.3V)
    * Atuador: Buzzer Ativo
* **Software (Firmware):**
    * Framework: ESP-IDF v5.x
    * Linguagem: C
    * Sistema Operacional: FreeRTOS (integrado ao ESP-IDF)
* **Periféricos (ESP-IDF API):**
    * `driver/adc.h`: Para leitura analógica do sensor (ADC1).
    * `driver/ledc.h`: Para controle de PWM (tom e volume) do buzzer.
* **Protocolos:**
    * Wi-Fi (via `esp_wifi`)
    * MQTT (via `mqtt_client` da biblioteca `esp-mqtt`)
 
## 5. Esquema de montagem

![Diagrama do hardware feito na plataforma wokwi](./diagrama_wokwi.png)
    
