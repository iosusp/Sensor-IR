Como eu fiz
Leitura dos SensoresO código lê os pinos 20 e 21 da Porta E (onde liguei os sensores).
- **Lógica de Memória:** Eu criei uma "memória" para as curvas. Se o sensor vê a linha e depois perde, ele continua virando por um tempinho (PERSISTENCIA_CURVA) para não se perder na pista.
- Movimentação
  - Se os dois sensores estão no branco, ele vai reto.
  - Se um sensor detecta a linha, ele liga o motor de um lado e inverte o outro (faz o pivô) para virar bem rápido.
- LED Deixei o LED da placa em laranja para saber que o sistema está ligado.

Dificuldades e Erros
- Carrinho Torto: Mesmo mandando a mesma velocidade, as rodas não eram iguais. Tive que colocar 1000 em uma e 960 na outra para ele andar reto no cruzeiro.
-Configuração da Porta E: No começo o Zephyr não reconhecia os sensores, aí vi que precisava definir o DT_NODELABEL(gpioe) certinho para funcionar a entrada digital.
- **Pivô:** Tive que ajustar a velocidade de ré na curva (os valores 680 e 670) porque se fosse muito rápido o carrinho capotava ou saía da linha.

Testes Realizados
Testei na pista e o carrinho conseguiu completar a volta. Ele treme um pouco nas curvas por causa da correção rápida, mas a memória ajudou ele a não sair nos cruzamentos.