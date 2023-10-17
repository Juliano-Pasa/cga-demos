Implementações

1- Cada patch é refinado no tesselation shader, com um nível específico de refinamento

2- As arestas de patches vizinhos são iguais, evitando formação de t-vertex

3- As normais foram calculadas já no TES, pois o terreno foi gerado totalmente de forma procedural, e então foi possível calcular a normal a partir das funções utilizadas no TES. (Melhor explicado depois)

4- Função procedural para fazer a perturbação do terreno

5- Terreno iluminado por duas fontes luminosas em movimento

6- Nível de refinamento muda em função da câmera

7- Terreno pode ser visualizado em wireframe e preenchimento

8- É possível ajustar o nível de refinamento do vértice central, e os seus vizinhos se adaptam


* Extras *
1- Geração de t-vertex

2- Animação do terreno

3- Iluminação Phong com alteração das cores


* Como usar o programa *

O trabalho foi feito todo em cima do demo basic_tessellation.

No console são mostrados todos os possíveis comandos que podem ser executados. 

Existem 3 modos de exibição:
1 - Refinamento com base na distância da câmera
2 - Refinamento ao redor do patch central
3 - Patch central gerando t-vertex

Também é possível parar a movimentação das ondas.

O efeito das ondas do mar é obtido a partir da soma de diversas ondas Seno modificadas. Cada onda tem uma direção, amplitude e frequência diferente.

Na função *void TessellatedQuad::init()* são inicializados inúmeros parâmetros. Não havia necessidade de deixar todos parametrizáveis pelo teclado, então testei diversos valores diferentes e deixei os que geravam um melhor resultado.