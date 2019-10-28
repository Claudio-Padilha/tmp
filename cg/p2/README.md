# Atividades Realizadas na Parte 2

Estudantes:

- Cláudio Padilha, 2017 1904 003 6;
- Felipe Machado, 2017 1904 014 1.

Os arquivos de configuração do projeto foram atualizados para utilizar o padrão **C++17**
e o compilador MSVC necessita do *switch* `/Zc:__cplusplus` para construir o projeto corretamente.

Durante a acoplação do código do P1 neste trabalho, as coleções de objetos de cena de um *objeto de cena*
e de uma *cena* foram implementadas com `std::vector`, por este ter maior velocidade de iteração.

## A1 - Implementar, em P2.cpp, o `métodoP2::drawCamera(c)`

Feito!
O volume de vista foi desenhado para ambos os casos de *Projeção Paralela*
e de *Projeção Perspectiva*;

Linhas 823 a 902 em `P2.cpp`;

## A2 - Implementar, em GLRenderer.cpp, o método `GLRenderer::render()`

Feito!
Foi criada uma instância de `GLSL::Program` própria para `GLRenderer` apesar de esta utilizar os mesmos shaders
que o o `GLSL::Program` de `cg::P2`.

Linhas 50 a 67 em `GLRenderer.h`;

## A3 - Definir e implementar, na classe P2 ou SceneEditor, um método de preview da renderização da cena

Feito!
Foi criada uma janela no ImGui com o título *"Camera Preview"* com a visão da câmera correntemente selecionada na janela da hierarquia.
A janela mantém a mesma razão de aspecto que o editor de cena, utilizando-se
do *callback* `void mantain_aspect(ImGuiSizeCallbackData*)` passado ao ImGui.

No método `P2::preview(Camera&)`, foi feita a renderização em um framebuffer separado e vinculada a uma **textura**.
Esta textura foi passada ao ImGui por meio de `ImGui::GetWindowDrawList()->AddImage`.

Linhas 938 a 1020 em `P2.cpp`;

## A4 - Definir e implementar, na classe P2 ou SceneEditor, um método de foco da câmera [...]

Feito!
Método de foco implementado na classe SceneEditor, na qual o método `focus_on`
recebe como parâmetro uma *bounding box* em coordenadas globais que deve ser totalmente visível pela câmera.

A câmera é sempre posta a uma distância `distance` afastada do centro da *bounding box*,
na direção contraria da direção de projeção `v_dop`,
independentemente da posição inicial da câmera.

Feito isto, resta apenas saber a distância ideal para que todo o objeto seja visível.
Na visão perspectiva, a distância mínima é dada por *d = H / (2 tan (\theta / 2))*, onde *H* é a altura do objeto e *\theta* é o ângulo de vista.
Na visão paralela, a altura da câmera é corrigida para a altura de *bounds* (`bounds.size().y`).

Linhas 54 a 91 em `SceneEditor.cpp`;

## A5 - Implementar um mecanismo interativo na GUI de ajuste do pai de um objeto de cena e completar `Transform::{update, parentChanged}`

Feito!

Linhas 162 a 199 em `Transform.cpp`;

## A6 - Implementar os métodos da GUI de criação, adição, remoção, *etc.*

Feito!
Método `P2::addComponentButton(SceneObject&)`.

Linhas [212,227] e [500,533] em `P2.cpp`;
