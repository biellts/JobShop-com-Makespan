# Job Shop com Makespan

Implementacao em C++ para o trabalho de Algoritmos para Grafos usando instancias JSP e objetivo de minimizacao do makespan.

## O que o programa faz

- testa a caminhada topologica no grafo fixo do enunciado
- le instancias `.psi` de Job Shop
- constroi uma agenda ativa com heuristica de Giffler-Thompson
- monta o DAG de operacoes com pesos nos vertices
- calcula ordem topologica, caminho maximo e makespan
- gera relatorios por instancia e uma tabela consolidada

## Compilacao

No PowerShell, a partir da raiz do projeto:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o jsp_makespan.exe
```

## Execucao

Para rodar todas as instancias JSP padrao:

```powershell
.\jsp_makespan.exe
```

Opcoes uteis:

```powershell
.\jsp_makespan.exe --iterations 10
.\jsp_makespan.exe --jsp-dir .\temp_batches\batch1 --output-dir .\output_batches\batch1
```

## Saidas geradas

No diretorio `output`:

- `fixed_graph_report.txt`: teste do grafo fixo do enunciado
- `jsp_makespan_results.csv`: tabela consolidada das instancias JSP
- `report.md`: pequeno relatorio em Markdown
- `results_table.html`: tabela consolidada em HTML
- `results_table.pdf`: tabela consolidada em PDF
- `details\*.txt`: detalhamento por instancia com ordem topologica, caminho critico e sequencias por maquina e por job

## Documentos para a apresentacao

- `relatorio_final.md`: texto focado nos criterios de avaliacao
- `roteiro_apresentacao.md`: roteiro de slides e falas para apresentacao

## Observacoes

- a configuracao padrao usa duas regras deterministicas de prioridade para manter a execucao pratica em lote
- `--iterations` ativa tentativas randomizadas extras por instancia