# Atividades Realizadas na Parte 1

Estudantes:

- Cl�udio Padilha, 2017 1904 003 6;
- Felipe Machado, 2017 1904 014 1.

## A1 - Cole��o de SceneObject's em Scene e SceneObject

A estrutura de dados escolhida para implementar esta cole��o foi `std::list`,
uma lista duplamente encadeada, por ser simples e a adi��o de itens ser linear.
Ser� utilizado o iterador fornecido pela estrutura.
Scene.h -> linha 82; SceneObject.h -> linha 122.

## A2 - Implementa��o de `SceneObject::setParent`

Fun��o define pai de um objeto de cena e atualiza as cole��es necess�rias atrav�s
das fun��es add_object() e remove_object(). Tamb�m atualiza as refer�ncias necess�rias.
SceneObject.cpp -> linhas 44 a 70.
## A3 - Cole��o de Componentes em SceneObject

Foi utilizada a mesma estrutura de dados mencionada em A1, pelos mesmos motivos.
SceneObject.h -> linha 123.

## A4 - Alterando a cena

Foram criados uma box, dois objetos vazios e uma "esfera" (atividade extra) como raizes da cena.
Em um dos objetos raizes, foram adicionados 3 box como filhos desse objeto.
E no �ltimo box, foram adicionados mais dois objeto de cena.
P1.cpp -> linhas 200 a 211.

## A5 - Bot�o create e hierarquia

Foi implementado o bot�o create para o usu�rio adicionar objetos de cena a cena. Tamb�m foi 
utilizado o m�todo 'hierarchy_tree_from()' (linhas 165 a 205) que usa DFS para procurar os 
n�s da cena e formar a �rvore de visualiza��o.
P1.cpp -> linhas 256 a 301.

## A6 - Modifica��o do "Inspetor"

Foi alterado o m�todo 'P1::sceneObjectGui()' para que o inspetor mostre o n� atual na sua 
janela.
P1.cpp -> linhas 414 a 443.

## A7 - Mostrando todos os primitivos na cena

Foi alterado o m�todo 'P1::render()' para que ele exiba todos os primitivos de objetos de
uma determinada cena.
P1.cpp -> linhas 510 a 542.

# Extens�es implementadas por iniciativa pr�pria

Foi implementada uma tentativa de gerar a malha de uma "esfera"
para criar um novo objeto de cena al�m da box e explorar os conceitos
de cria��o de tri�ngulos sobre o vertex buffer. Por�m, o resultado n�o
foi o esperado mas � funcional. O objeto de cena criado se comporta como o esperado
apesar de n�o ser uma esfera.

