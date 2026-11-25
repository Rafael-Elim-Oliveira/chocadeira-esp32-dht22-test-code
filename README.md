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


Como usar no Wokwi:

1. Abra um novo projeto de ESP32 no Wokwi.
2. Clique na aba diagram.json (ao lado da aba sketch.ino).
3. Apague o conteúdo existente e cole o código JSON abaixo.
4. Na aba Library Manager, adicione a biblioteca DHT sensor library.


Como testar os cenários na simulação:

• Temperatura: Clique no sensor DHT22 durante a simulação e arraste o controle deslizante de temperatura:
	• < 37.5 °C: Acende o LED Vermelho (Aquecedor).
	• 37.5 °C a 38.0 °C: Apaga ambos os LEDs.
	• > 38.0 °C: Acende o LED Azul (Resfriador).
	• < 27.0 °C ou > 39.0 °C: Dispara o alerta de e-mail no Monitor Serial.
• Queda de Energia (Nobreak): Clique na Chave Deslizante para alternar entre rede normal (3V3) e falha de energia (GND), observando o alerta no Monitor Serial enquanto o controle da chocadeira continua operando normalmente.
