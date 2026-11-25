# Guia de Montagem no Wokwi - Chocadeira Aviária

| Componente | Pino do Componente | Pino no ESP32 | Observação |
| :--- | :--- | :--- | :--- |
| **DHT22** | `VCC` | `3V3` | Conforme RNF05 |
| **DHT22** | `SDA` (Dados) | `GPIO 4` | Conforme RNF03 |
| **DHT22** | `GND` | `GND` | Conforme RNF05 |
| **LED Vermelho** *(Aquecedor)* | `Anodo (+)` | `GPIO 18` | Usar resistor de 220Ω em série (RNF04) |
| **LED Azul** *(Resfriador)* | `Anodo (+)` | `GPIO 19` | Usar resistor de 220Ω em série (RNF04) |
| **Chave Deslizante** *(Slide Switch)* | `Pino Central (COM)` | `GPIO 21` | Simula a rede elétrica (RNF07) |
| **Chave Deslizante** | `Pino Lateral 1` | `GND` | Chave para este lado = Queda de energia |
| **Chave Deslizante** | `Pino Lateral 2` | `3V3` | Chave para este lado = Rede normal |  

# Como usar no Wokwi:

1. Abra um novo projeto de ESP32 no Wokwi.
2. Clique na aba diagram.json (ao lado da aba sketch.ino).
3. Apague o conteúdo existente e cole o código JSON abaixo.
4. Na aba Library Manager, adicione a biblioteca DHT sensor library.


# Como testar os cenários na simulação:

• Temperatura: Clique no sensor DHT22 durante a simulação e arraste o controle deslizante de temperatura:  

	• < 37.5 °C: Acende o LED Vermelho (Aquecedor).
	• 37.5 °C a 38.0 °C: Apaga ambos os LEDs.
	• > 38.0 °C: Acende o LED Azul (Resfriador).
	• < 27.0 °C ou > 39.0 °C: Dispara o alerta de e-mail no Monitor Serial.
• Queda de Energia (Nobreak): Clique na Chave Deslizante para alternar entre rede normal (3V3) e falha de energia (GND), observando o alerta no Monitor Serial enquanto o controle da chocadeira continua operando normalmente.  

# Plano e Casos de Teste — Sensoriamento de Chocadeira Aviária

**Equipe:** João Paulo Bastos, Thiago Rafael, Rafael Elim, Flavio Eduardo, Pedro Lucas  
**Ambiente de Validação:** Simulador Wokwi (ESP32 + DHT22 + Atuadores LED + Chave Deslizante)  

---

## 1. Matriz de Rastreabilidade (Requisitos x Casos de Teste)

| ID Caso de Teste | Critério de Aceitação | Requisitos Cobertos | Cenário Testado |
| :--- | :--- | :--- | :--- |
| **CT-01** | CA01 | RF01, RNF01, RNF02 | Ciclo de leitura periódica do sensor a cada 5 segundos |
| **CT-02** | CA03 (Mínimo) | RF04 | Acionamento do aquecedor abaixo de 37,5 °C |
| **CT-03** | CA03 (Máximo) | RF04 | Acionamento do resfriador acima de 38,0 °C |
| **CT-04** | — | RF05 | Desligamento dos atuadores na faixa térmica ideal (37,5 °C a 38,0 °C) |
| **CT-05** | CA02 | RF02, RF03 | Disparo de alerta por e-mail em temperatura crítica alta (> 39,0 °C) |
| **CT-06** | CA02 | RF02, RF03 | Disparo de alerta por e-mail em temperatura crítica baixa (< 27,0 °C) |
| **CT-07** | — | RF02, RF03, RNF01 | Tratamento de falha de leitura do sensor (`NaN`) |
| **CT-08** | CA04 | RNF07 | Continuidade operacional durante falha da rede elétrica (Modo Nobreak) |

---

## 2. Especificação Detalhada dos Casos de Teste

### CT-01: Intervalo de Monitoramento de Temperatura
* **Referência:** CA01, RF01, RNF01, RNF02
* **Pré-condição:** Simulação iniciada no Wokwi com o Monitor Serial aberto (`115200 baud`).
* **Passos para Execução:**
  1. Iniciar a simulação e observar o timestamp das mensagens exibidas no Monitor Serial.
  2. Aguardar a exibição de pelo menos 3 ciclos consecutivos de telemetria (`[TELEMETRIA MQTT]`).
* **Resultado Esperado:** O sistema realiza a coleta de temperatura e atualiza o estado dos atuadores exatamente a cada **5 segundos** (`5000 ms`), sem atrasos superiores a 10 segundos.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-02: Manutenção Térmica — Limite Mínimo (Acionamento do Aquecedor)
* **Referência:** CA03 (Cenário de limite mínimo), RF04, RNF04
* **Pré-condição:** Sistema em operação normal.
* **Passos para Execução:**
  1. Clicar no sensor **DHT22** na área de simulação.
  2. Ajustar o controle deslizante de temperatura para **`36.0 °C`** (abaixo de `37.5 °C`).
  3. Aguardar o próximo ciclo de 5 segundos.
* **Resultado Esperado:**
  * O pino **GPIO 18** vai para nível lógico alto (`HIGH`).
  * O **LED Vermelho** (Aquecedor) acende e o **LED Azul** (Resfriador) permanece apagado.
  * O Monitor Serial exibe: `[CONTROLE - CA03] Temp < 37.5°C -> AQUECEDOR LIGADO (GPIO 18) | Resfriador OFF`.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-03: Manutenção Térmica — Limite Máximo (Acionamento do Resfriador)
* **Referência:** CA03 (Cenário de limite máximo), RF04, RNF04
* **Pré-condição:** Sistema em operação normal.
* **Passos para Execução:**
  1. Clicar no sensor **DHT22** na área de simulação.
  2. Ajustar o controle deslizante de temperatura para **`38.5 °C`** (acima de `38.0 °C`).
  3. Aguardar o próximo ciclo de 5 segundos.
* **Resultado Esperado:**
  * O pino **GPIO 19** vai para nível lógico alto (`HIGH`).
  * O **LED Azul** (Resfriador) acende e o **LED Vermelho** (Aquecedor) permanece apagado.
  * O Monitor Serial exibe: `[CONTROLE - CA03] Temp > 38.0°C -> RESFRIADOR LIGADO (GPIO 19) | Aquecedor OFF`.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-04: Desligamento dos Atuadores na Faixa Ideal
* **Referência:** RF04, RF05
* **Pré-condição:** Um dos atuadores (aquecedor ou resfriador) previamente ligado.
* **Passos para Execução:**
  1. Clicar no sensor **DHT22** e ajustar a temperatura para **`37.8 °C`** (entre `37.5 °C` e `38.0 °C`).
  2. Aguardar o próximo ciclo de 5 segundos.
* **Resultado Esperado:**
  * Ambos os pinos (**GPIO 18** e **GPIO 19**) vão para nível lógico baixo (`LOW`).
  * Tanto o **LED Vermelho** quanto o **LED Azul** permanecem apagados.
  * O Monitor Serial exibe: `[CONTROLE - RF05] Faixa Ideal (37.5°C - 38.0°C) -> ATUADORES DESLIGADOS`.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-05: Resposta a Temperatura Crítica Alta (> 39,0 °C)
* **Referência:** CA02, RF02, RF03
* **Pré-condição:** Sistema operando dentro da faixa segura (`27.0 °C` a `39.0 °C`).
* **Passos para Execução:**
  1. Ajustar a temperatura do sensor **DHT22** para **`40.5 °C`**.
  2. Aguardar o ciclo de leitura de 5 segundos.
* **Resultado Esperado:**
  * O sistema aciona o resfriador (**LED Azul** ligado).
  * O Monitor Serial exibe o bloco `[CAMADA APLICACAO - ENVIO DE E-MAIL (RF02)]` contendo o alerta `PERIGO - TEMPERATURA CRITICA ALTA: 40.5 °C (Acima de 39.0 °C)`.
  * Nenhuma notificação de e-mail deve ser disparada quando a temperatura estiver entre `27.0 °C` e `39.0 °C`.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-06: Resposta a Temperatura Crítica Baixa (< 27,0 °C)
* **Referência:** CA02, RF02, RF03
* **Pré-condição:** Sistema operando dentro da faixa segura (`27.0 °C` a `39.0 °C`).
* **Passos para Execução:**
  1. Ajustar a temperatura do sensor **DHT22** para **`25.0 °C`**.
  2. Aguardar o ciclo de leitura de 5 segundos.
* **Resultado Esperado:**
  * O sistema aciona o aquecedor (**LED Vermelho** ligado).
  * O Monitor Serial exibe o bloco `[CAMADA APLICACAO - ENVIO DE E-MAIL (RF02)]` contendo o alerta `PERIGO - TEMPERATURA CRITICA BAIXA: 25.0 °C (Abaixo de 27.0 °C)`.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-07: Falha de Leitura do Sensor (`NaN`)
* **Referência:** RF02, RF03, RNF01
* **Pré-condição:** Sistema em execução no Wokwi.
* **Passos para Execução:**
  1. Pausar a simulação e desconectar o fio de dados (`SDA`) ligado ao **GPIO 4** (ou simular falha no sensor).
  2. Retomar a simulação e aguardar o ciclo de 5 segundos.
* **Resultado Esperado:**
  * O sistema identifica o retorno inválido (`isnan(temperatura)`).
  * Ambos os atuadores (**GPIO 18** e **GPIO 19**) são desligados imediatamente por segurança.
  * O Monitor Serial exibe `[ERRO SENSOR] Falha na leitura do DHT22 (Retorno NaN)!` e dispara o alerta de e-mail de falha de hardware.
* **Status:** [ ] Aprovado | [ ] Reprovado

---

### CT-08: Queda da Alimentação Principal (Simulação de Nobreak)
* **Referência:** CA04, RNF07
* **Pré-condição:** Sistema operando com a chave deslizante conectada ao lado **`3V3`** (Rede Elétrica Normal).
* **Passos para Execução:**
  1. Durante a simulação ativa, clicar na **Chave Deslizante** (`GPIO 21`) para comutá-la para o lado **`GND`** (simulando queda de energia).
  2. Observar o Monitor Serial e alterar a temperatura do sensor DHT22 para verificar se o controle continua ativo.
* **Resultado Esperado:**
  * Imediatamente após a comutação da chave, o Monitor Serial exibe: `[ENERGIA - RNF07] ALERTA: Queda na rede elétrica principal!` seguido de `Sistema operando normalmente via NOBREAK`.
  * O ESP32 não reinicia e mantém normalmente o ciclo de leitura a cada 5 segundos e o acionamento dos atuadores.
  * Ao retornar a chave para `3V3`, o sistema registra que a rede elétrica principal foi restabelecida.
* **Status:** [ ] Aprovado | [ ] Reprovado
