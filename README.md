# Projetos de Eletrônica Digital e Sistemas Embarcados

Repositório com projetos desenvolvidos durante as disciplinas de Introdução à Eletricidade e Eletrônica Digital e Introdução a Sistemas Embarcados que cursei durante a minha graduação de Bacharelado em Ciência da Computação no Instituto Federal do Paraná. A maioria dos projetos utiliza a plataforma ESP32 e neste reposítório estão salvas as implementações simuladas no [Wokwi](https://wokwi.com/).

## 🎯 Estrutura do Repositório

O repositório está organizado em duas categorias principais, uma para cada disciplina:

* **`/elet-digital`**: Projetos focados nos conceitos fundamentais da eletrônica digital.
* **`/sist-embarcados`**: Projetos que exploram sistemas microcontrolados, interações com hardware e software de baixo nível.

Dentro de cada pasta de projeto, você encontrará:
* `sketch.ino`: O código-fonte principal para o microcontrolador.
* `diagram.json`: O arquivo de diagrama para simulação no Wokwi.
* `about.md`: Uma breve descrição e detalhes sobre o projeto específico.

Posteriormente, também, adicionarei projetos feitos com a ferramenta open source para projetos de lógica digital e simulação de circuitos digitais chamada [Digital](https://github.com/hneemann/Digital)
---


### ⚡ Eletrônica Digital

| Projeto                                                                                 | Descrição                                                                          |
| --------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------- |
| [**Display de 7 Segmentos**](./elet-digital/esp32-e-display7segmentos-wowki)                  | Controle de um único display de 7 segmentos com um ESP32.                          |
| [**Múltiplos Displays de 7 Segmentos**](./elet-digital/esp32-e-multiplosdisplays7segmentos-wowki) | Controle de múltiplos displays de 7 segmentos para exibir números maiores.          |
| [**Soma via Wi-Fi com Displays**](./elet-digital/esp32-e-multiplosdisplays7segmentos-soma)     | Projeto que recebe números via Wi-Fi, realiza uma soma e exibe o resultado.        |
| [**Semáforo com LEDs e Wi-Fi**](./elet-digital/esp32-ledsSemaforico-wifi-wowki)                 | Simulação de um semáforo com LEDs controlado remotamente via Wi-Fi.                |

### ⚙️ Sistemas Embarcados

| Projeto                                                                    | Descrição                                                                      |
| -------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| [**Pisca LED com Frequência**](./sist-embarcados/esp32-freq-led-pisca)         | Projeto básico para piscar um LED em uma frequência predefinida.               |
| [**Controle de Frequência com Botão**](./sist-embarcados/esp32-freq-led-botao) | Altera a frequência com que um LED pisca utilizando um botão como entrada.     |
| [**Controle de LED RGB**](./sist-embarcados/esp32-led-rgb)                     | Demonstração de controle de cores de um LED RGB utilizando sinais PWM.         |

---

### 🚀 Como Usar

1.  Clone o repositório:
    ```bash
    git clone [https://github.com/seu-usuario/eletronica-digital-sist-embarcados.git](https://github.com/seu-usuario/eletronica-digital-sist-embarcados.git)
    ```
2.  Abra um dos projetos na sua IDE de preferência (ex: Arduino IDE, VS Code com PlatformIO).
3.  Para os projetos com `diagram.json`, você pode abrir o simulador Wokwi, colar o conteúdo do `diagram.json` e do `sketch.ino` para ver o projeto em ação.