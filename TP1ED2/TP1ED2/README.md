# TP1 ED2 — Repositório reorganizado

## O que mudou em relação ao zip enviado

O zip tinha duas cópias quase inteiras do projeto (uma em C, na pasta
`TP1ED2/TP1ED2/`, e outra em conversão para C++ na raiz), além de 3 versões
diferentes da struct de registro e vários arquivos de teste avulsos
(`gerarC.c`, `gerarD.c`, `gerarArquivoB.c`, `main1.c`, `mainbe.c`). Isso
impedia o projeto de compilar como um todo.

Esta pasta mantém **um único arquivo .hpp/.cpp por módulo**, todos
compilando juntos, com `main.cpp` integrando os 4 métodos conforme a
linha de comando exigida pelo TP:

```
./pesquisa <metodo 1-4> <quantidade> <situacao 1-3> <chave> [-P]
```

Compilação:
```
g++ -std=c++17 -O2 -o pesquisa main.cpp index.cpp arvore_binaria.cpp arvoreB.cpp arvoreBE.cpp gerador_arquivo.cpp paginacao.cpp
```

Se o arquivo de dados para a `<quantidade>`/`<situacao>` pedidas ainda não
existir, ele é gerado automaticamente (`dados_<quantidade>_<situacao>.bin`).

## Arquivos removidos (motivo)
- `TP1ED2/TP1ED2/*` (cópia inteira duplicada, mais antiga)
- `registro.h`, `registrobe.h`, `registro.hpp` antigos → substituídos por um único `registro.hpp`
- `gerarC.c`, `gerarD.c`, `gerarArquivoB.c` → scripts avulsos de teste com struct de 1 campo só, não usados por ninguém
- `main1.c` / `mainbe.c` (duplicados entre si) → lógica incorporada ao `case 4` do `main.cpp`

## Status por método
| Método | Arquivo | Responsável | Status |
|---|---|---|---|
| 1. Acesso sequencial indexado | `index.hpp/.cpp` | Luiza | Compila e roda; só corrigi includes/tipos para integrar. Lógica original preservada. |
| 2. Árvore binária externa | `arvore_binaria.hpp/.cpp` | João Vítor | Corrigido bug de `typedef` ausente (quebrava a compilação do projeto inteiro). Lógica original preservada. |
| 3. Árvore B | `arvoreB.hpp/.cpp` | **você** | Renomeado para usar a struct `registro.hpp` padrão; testado com até 20.000 registros. |
| 4. Árvore B\* | `arvoreBE.hpp/.cpp` | **você** | **Corrigido um bug real** que fazia o programa travar (estouro de buffer na folha) e um segundo bug que fazia a busca não achar uma chave existente em alguns casos (a página de divisão não retinha o item no lado certo). Testado exaustivamente após a correção. |

Os dois bugs de `arvoreBE` valem a pena entendermos juntos com calma — são
ótimo material para a entrevista, porque mostram exatamente os pontos
delicados da Árvore B* (quantos itens cabem numa página durante a divisão,
e onde o item "do meio" fica fisicamente depois de dividir).
