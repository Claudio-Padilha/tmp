# Atividades Realizadas na Parte 2

Estudantes:

- Cl�udio Padilha, 2017 1904 003 6;
- Felipe Machado, 2017 1904 014 1.

Os arquivos de configura��o do projeto foram atualizados para utilizar o padr�o **C++17**
e o compilador MSVC necessita do *switch* `/Zc:__cplusplus` para construir o projeto corretamente.

Durante a acopla��o do c�digo do P1 neste trabalho, as cole��es de objetos de cena de um *objeto de cena*
e de uma *cena* foram implementadas com `std::vector`, por este ter maior velocidade de itera��o.

## A1 - Implementar, em P2.cpp, o `m�todoP2::drawCamera(c)`

Feito!
O volume de vista foi desenhado para ambos os casos de *Proje��o Paralela*
e de *Proje��o Perspectiva*;

Linhas 823 a 902 em `P2.cpp`;

## A2 - Implementar, em GLRenderer.cpp, o m�todo `GLRenderer::render()`

Feito!
Foi criada uma inst�ncia de `GLSL::Program` pr�pria para `GLRenderer` apesar de esta utilizar os mesmos shaders
que o o `GLSL::Program` de `cg::P2`.

Linhas 50 a 67 em `GLRenderer.h`;

## A3 - Definir e implementar, na classe P2 ou SceneEditor, um m�todo de preview da renderiza��o da cena

Feito!
Foi criada uma janela no ImGui com o t�tulo *"Camera Preview"* com a vis�o da c�mera correntemente selecionada na janela da hierarquia.
A janela mant�m a mesma raz�o de aspecto que o editor de cena, utilizando-se
do *callback* `void mantain_aspect(ImGuiSizeCallbackData*)` passado ao ImGui.

No m�todo `P2::preview(Camera&)`, foi feita a renderiza��o em um framebuffer separado e vinculada a uma **textura**.
Esta textura foi passada ao ImGui por meio de `ImGui::GetWindowDrawList()->AddImage`.

Linhas 938 a 1020 em `P2.cpp`;

## A4 - Definir e implementar, na classe P2 ou SceneEditor, um m�todo de foco da c�mera [...]

Feito!
M�todo de foco implementado na classe SceneEditor, na qual o m�todo `focus_on`
recebe como par�metro uma *bounding box* em coordenadas globais que deve ser totalmente vis�vel pela c�mera.

A c�mera � sempre posta a uma dist�ncia `distance` afastada do centro da *bounding box*,
na dire��o contraria da dire��o de proje��o `v_dop`,
independentemente da posi��o inicial da c�mera.

Feito isto, resta apenas saber a dist�ncia ideal para que todo o objeto seja vis�vel.
Na vis�o perspectiva, a dist�ncia m�nima � dada por *d = H / (2 tan (\theta / 2))*, onde *H* � a altura do objeto e *\theta* � o �ngulo de vista.
Na vis�o paralela, a altura da c�mera � corrigida para a altura de *bounds* (`bounds.size().y`).

Linhas 54 a 91 em `SceneEditor.cpp`;

## A5 - Implementar um mecanismo interativo na GUI de ajuste do pai de um objeto de cena e completar `Transform::{update, parentChanged}`

Feito!

Linhas 162 a 199 em `Transform.cpp`;

## A6 - Implementar os m�todos da GUI de cria��o, adi��o, remo��o, *etc.*

Feito!
M�todo `P2::addComponentButton(SceneObject&)`.

Linhas [212,227] e [500,533] em `P2.cpp`;
