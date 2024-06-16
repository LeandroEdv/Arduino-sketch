# Sistema de Gerenciamento de Hidroponia

Este projeto foi desenvolvido como parte do curso de automação industrial e consiste em um sistema de gerenciamento de hidroponia. O sistema analisa fatores como temperatura da água, luminosidade e nível do reservatório para otimizar a circulação de água para as plantas.

## 📋 Descrição do Projeto

O sistema de hidroponia automatizado monitora e controla os seguintes parâmetros:
- **Temperatura da água**: Utiliza um sensor DS18B20 para monitorar a temperatura da água e acionar um sistema de refrigeração quando necessário.
- **Luminosidade**: Utiliza um sensor LDR para medir a luminosidade ambiente e controlar a iluminação artificial.
- **Nível do reservatório**: Utiliza um sensor ultrassônico para medir o nível de água no reservatório e acionar a bomba de água conforme necessário.

## 🛠️ Tecnologias Utilizadas

- **Linguagem de Programação**: C++
- **Plataforma**: Arduino
- **Bibliotecas**:
  - Ultrasonic
  - RTClib
  - OneWire
  - DallasTemperature

## ⚙️ Funcionalidades

### Monitoramento e Controle da Temperatura
O sistema mede a temperatura da água utilizando o sensor DS18B20 e aciona a refrigeração se a temperatura ultrapassar 27.5°C.

### Monitoramento de Luminosidade
O sensor LDR mede a luminosidade ambiente. A iluminação artificial é acionada automaticamente se a luminosidade estiver abaixo de um certo nível.

### Controle do Nível do Reservatório
Um sensor ultrassônico mede o nível de água no reservatório. Se o nível estiver abaixo de um valor definido, a bomba de água é acionada para recircular a água.

 
