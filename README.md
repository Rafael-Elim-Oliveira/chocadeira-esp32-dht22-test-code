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
