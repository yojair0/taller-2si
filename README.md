# taller-2si
El algoritmo Minimax es un método utilizado en juegos de estrategia para encontrar la jugada óptima en un árbol de posibilidades. En el caso de Conecta 4, este algoritmo evalúa todas las posibles secuencias de movimientos hasta un cierto nivel de profundidad en el árbol de juego. Sin embargo, la complejidad de Conecta 4 aumenta rápidamente, ya que el número de posiciones posibles es considerable.

En el enfoque sin poda, Minimax examina exhaustivamente todas las ramas del árbol de juego, lo que puede resultar en una gran cantidad de cálculos y llevar a un tiempo de ejecución prolongado, especialmente en juegos más complejos. Esto puede no ser práctico en situaciones en las que se desea una respuesta rápida o se tiene un límite de tiempo para tomar decisiones.

Minimax con poda (también conocido como Alfa-Beta Pruning):

Para abordar la limitación de tiempo y mejorar la eficiencia del algoritmo Minimax, se introduce la poda alfa-beta. La poda alfa-beta permite evitar la exploración de ciertas ramas del árbol que se sabe que no conducirán a una solución óptima. Esto se logra mediante el seguimiento de dos valores, alfa y beta, que representan las mejores puntuaciones que el jugador maximizador y minimizador, respectivamente, han encontrado hasta el momento.

Cuando se encuentra una rama que definitivamente no conducirá a una solución óptima, se realiza la poda, evitando así la exploración adicional de esa rama y reduciendo significativamente el número de nodos evaluados.

En resumen, mientras que Minimax sin poda explora exhaustivamente todas las posibles jugadas, Minimax con poda alfa-beta elimina ramas innecesarias del árbol de juego, mejorando la eficiencia del algoritmo y permitiendo que se ejecute en un tiempo razonable, incluso en juegos complejos como Conecta 4.
