# Atividades Realizadas na Parte 1

Estudantes:

- Cláudio Padilha, 2017 1904 003 6;
- Felipe Machado, 2017 1904 014 1.

## A1 - Coleção de SceneObject's em Scene e SceneObject

A estrutura de dados escolhida para implementar esta coleção foi `std::list`,
uma lista duplamente encadeada, por ser simples e a adição de itens ser linear.
Será utilizado o iterador fornecido pela estrutura.
Scene.h -> linha 82; SceneObject.h -> linha 122.

## A2 - Implementação de `SceneObject::setParent`

Função define pai de um objeto de cena e atualiza as coleções necessárias através
das funções add_object() e remove_object(). Também atualiza as referências necessárias.
SceneObject.cpp -> linhas 44 a 70.
## A3 - Coleção de Componentes em SceneObject

Foi utilizada a mesma estrutura de dados mencionada em A1, pelos mesmos motivos.
SceneObject.h -> linha 123.

## A4 - Alterando a cena

Foram criados uma box, dois objetos vazios e uma "esfera" (atividade extra) como raizes da cena.
Em um dos objetos raizes, foram adicionados 3 box como filhos desse objeto.
E no último box, foram adicionados mais dois objeto de cena.
P1.cpp -> linhas 200 a 211.

## A5 - Botão create e hierarquia

Foi implementado o botão create para o usuário adicionar objetos de cena a cena. Também foi 
utilizado o método 'hierarchy_tree_from()' (linhas 165 a 205) que usa DFS para procurar os 
nós da cena e formar a árvore de visualização.
P1.cpp -> linhas 256 a 301.

## A6 - Modificação do "Inspetor"

Foi alterado o método 'P1::sceneObjectGui()' para que o inspetor mostre o nó atual na sua 
janela.
P1.cpp -> linhas 414 a 443.

## A7 - Mostrando todos os primitivos na cena

Foi alterado o método 'P1::render()' para que ele exiba todos os primitivos de objetos de
uma determinada cena.
P1.cpp -> linhas 510 a 542.

# Extensões implementadas por iniciativa própria

Foi implementada uma tentativa de gerar a malha de uma "esfera"
para criar um novo objeto de cena além da box e explorar os conceitos
de criação de triângulos sobre o vertex buffer. Porém, o resultado não
foi o esperado mas é funcional. O objeto de cena criado se comporta como o esperado
apesar de não ser uma esfera.

