# Relatorio Final - Job Shop com Makespan

## 1. Objetivo do trabalho

O problema escolhido foi o Job Shop Scheduling com objetivo de minimizacao do makespan, isto e, do instante em que a ultima operacao termina. O enunciado exigia uma modelagem por digrafo com pesos nos vertices, uma caminhada topologica e a identificacao do caminho maximo. A solucao desenvolvida foi organizada exatamente em torno dessa exigencia: primeiro e construida uma agenda viavel; em seguida, essa agenda e transformada em um DAG ponderado; por fim, o makespan e obtido pelo maior caminho desse DAG.

Essa escolha e importante porque evita tratar o grafo apenas como uma ilustracao teorica. No projeto, o grafo e o mecanismo central de representacao e de avaliacao da solucao.

## 2. Modelagem do problema

No Job Shop, cada job e composto por uma sequencia de operacoes. Cada operacao deve ser executada em uma maquina especifica por um determinado tempo, e uma mesma maquina nao pode processar duas operacoes ao mesmo tempo. Portanto, o problema possui dois tipos de restricao:

- restricoes de precedencia, que obrigam a respeitar a ordem tecnologica das operacoes dentro de cada job
- restricoes de recurso, que impedem sobreposicao de operacoes na mesma maquina

Para representar esse sistema, cada operacao foi modelada como um vertice com peso igual ao seu tempo de processamento. As arestas do grafo indicam dependencias temporais. Existem dois grupos de arestas:

- arestas conjuntivas, que ligam operacoes consecutivas de um mesmo job
- arestas de maquina, que ligam operacoes que usam a mesma maquina segundo a ordem definida pela agenda

Depois que essas arestas sao definidas, o resultado e um DAG. Nesse grafo, qualquer caminho representa uma sequencia de dependencias que impede paralelismo total entre certas operacoes. O caminho de maior peso representa exatamente a cadeia critica da agenda, e por isso seu comprimento coincide com o makespan.

## 3. Conexao entre teoria e implementacao

Do ponto de vista teorico, o ponto principal e que em um DAG o problema do caminho maximo pode ser resolvido de forma eficiente usando uma ordenacao topologica. Essa propriedade foi explorada diretamente na solucao.

Primeiro, a estrutura de cada job e interpretada como um pequeno grafo aciclico de precedencias entre maquinas. Isso permite reconstruir corretamente a ordem tecnologica das operacoes de cada job.

Depois, a agenda produzida pela heuristica induz uma ordem em cada maquina. Ao combinar essa ordem com as precedencias dos jobs, obtemos o DAG global da solucao. A partir dele, a caminhada topologica permite calcular, para cada vertice, o maior tempo acumulado necessario para alcanca-lo. Em outras palavras, a distancia em cada vertice e o melhor valor de termino possivel respeitando todas as dependencias anteriores.

Essa formulacao fornece tres resultados centrais pedidos no trabalho:

- a ordem topologica em que os vertices sao processados
- o comprimento do caminho maximo
- a reconstrução explicita do caminho maximo, ou caminho critico

Assim, a teoria de grafos nao aparece separada da implementacao. Ela explica por que o algoritmo funciona e tambem como o makespan e medido.

## 4. Algoritmo utilizado

O problema de Job Shop e combinatoriamente dificil. Por isso, em vez de buscar uma solucao exata, foi adotado um algoritmo heuristico de construcao de agendas viaveis. A ideia e escolher, passo a passo, qual operacao executar em seguida, sempre respeitando as restricoes do problema.

O algoritmo funciona em quatro niveis logicos:

### 4.1 Operacoes elegiveis

Em cada instante da construcao da agenda, apenas algumas operacoes podem ser consideradas. Uma operacao e elegivel quando todas as operacoes anteriores do mesmo job ja foram programadas. Para cada operacao elegivel, calcula-se o inicio mais cedo possivel, levando em conta:

- o instante em que o job fica disponivel
- o instante em que a maquina correspondente fica livre

Isso gera um conjunto de candidatos viaveis para o proximo passo da agenda.

### 4.2 Operacao pivô

Entre as operacoes elegiveis, identifica-se a que possui menor tempo de termino mais cedo. Essa operacao serve como pivô. A intuicao e a seguinte: se ela for a primeira a terminar entre as candidatas, qualquer operacao que possa atrasar seu uso da maquina entra em competicao direta com ela e precisa ser analisada com cuidado.

### 4.3 Conjunto de conflito

Depois de escolher a operacao pivô, o algoritmo monta o conjunto de conflito. Esse conjunto contem operacoes que disputam a mesma maquina da pivô e que, se forem executadas antes dela, podem alterar a estrutura temporal da agenda. Essa etapa e fundamental, porque transforma o problema em uma decisao local bem definida: nao e preciso comparar todas as operacoes do sistema entre si, apenas aquelas que realmente afetam o uso imediato do recurso critico naquele passo.

### 4.4 Regra de prioridade

Uma vez montado o conjunto de conflito, entra a heuristica propriamente dita. A funcao da regra de prioridade e escolher qual operacao do conjunto deve ser programada primeiro. A cada escolha, a agenda parcial e atualizada, os tempos de disponibilidade de jobs e maquinas sao recalculados e o processo continua ate que todas as operacoes tenham sido programadas.

## 5. Heuristicas adotadas

Foram usadas duas regras de prioridade deterministicas como base da solucao.

### 5.1 Maior trabalho remanescente

Nessa regra, recebe prioridade o job que ainda possui maior carga de processamento restante. A justificativa e que jobs mais pesados tendem a concentrar risco de atraso no final do cronograma. Se forem deixados para depois, podem formar gargalos e elevar o makespan.

Do ponto de vista intuitivo, essa heuristica tenta atacar cedo os jobs mais longos para reduzir a probabilidade de que um grande volume de trabalho fique represado no fim da agenda.

### 5.2 Maior numero de operacoes remanescentes

Nessa regra, recebe prioridade o job que ainda possui mais operacoes nao executadas. A ideia e dar preferencia a jobs com maior profundidade de precedencias, porque eles possuem mais etapas para completar e, por isso, mais oportunidades de gerar atrasos futuros se forem negligenciados.

Essa heuristica e especialmente interessante em Job Shop porque o problema nao depende apenas do tempo total de processamento, mas tambem da quantidade de bloqueios sucessivos que cada job ainda precisa atravessar.

### 5.3 Por que usar heuristicas

Job Shop com makespan e um problema dificil, e a busca exata se torna rapidamente inviavel em conjuntos grandes de instancias. Como o trabalho exigia processar todas as instancias fornecidas, a opcao por heuristicas foi tecnicamente apropriada. Elas nao garantem otimo global, mas fornecem solucoes viaveis, coerentes e computacionalmente praticas em todo o conjunto testado.

## 6. Decisoes de projeto

Algumas decisoes foram importantes para manter o projeto correto e explicavel.

### 6.1 Separar construcao e avaliacao

Uma decisao importante foi separar o processo de construir a agenda do processo de avaliar a agenda. A heuristica decide a ordem das operacoes; o grafo e a caminhada topologica medem o resultado dessa escolha. Essa separacao melhora a clareza, porque deixa explicito onde termina a decisao heuristica e onde começa a analise baseada em teoria de grafos.

### 6.2 Priorizar clareza na solucao padrao

A configuracao padrao foi mantida enxuta, usando duas regras deterministicas. Isso facilita a reproducao dos resultados e evita que a apresentacao dependa de explicar um conjunto muito grande de variantes. Como extensao, o programa tambem aceita iteracoes randomizadas extras, mas essa parte ficou opcional para nao comprometer simplicidade e tempo de execucao.

### 6.3 Tratar casos especiais das instancias

Nos testes apareceu uma instancia com operacao de duracao zero. Esse caso mostrou que a modelagem precisava ser robusta mesmo em situacoes menos usuais. O criterio de formacao do conjunto de conflito foi ajustado para incluir corretamente a operacao pivô, preservando a coerencia da heuristica e evitando falhas estruturais na construcao da agenda.

## 7. Clareza, corretude e boas praticas

Para atender aos criterios de avaliacao, o projeto buscou clareza tanto na organizacao quanto na explicacao da solucao.

Do ponto de vista de corretude, cada etapa tem um papel bem definido:

- leitura da instancia e reconstrucao das precedencias
- construcao de uma agenda viavel respeitando jobs e maquinas
- transformacao da agenda em DAG ponderado
- aplicacao da caminhada topologica
- calculo e reconstrucao do caminho critico

Do ponto de vista de boas praticas, foram adotados cuidados como:

- separacao entre dados da instancia, dados da agenda e dados da analise
- validacao de erros de leitura e de consistencia do grafo
- geracao automatica de saidas detalhadas e consolidadas
- uso de estruturas padrao da linguagem para reduzir riscos de erros manuais

Essas decisoes tornam a solucao mais facil de explicar, testar e manter.

## 8. Resultados e interpretacao

Foram processadas 162 instancias JSP. A entrega inclui o teste do grafo fixo do enunciado, uma tabela consolidada de resultados e um detalhamento por instancia contendo ordem topologica, caminho critico e sequencias de execucao.

O valor medio de makespan nas instancias processadas pela configuracao padrao foi 2253.06. Mais importante que a media isolada e a consistencia observada: instancias pequenas apresentam makespans menores, enquanto familias maiores e mais densas geram valores bem mais altos, o que esta de acordo com a dificuldade estrutural do problema.

Essa coerencia ajuda a defender que a modelagem, a heuristica e a avaliacao por caminho maximo estao funcionando de maneira compatível com o comportamento esperado das instancias.

## 9. Limitacoes e extensoes naturais

A principal limitacao da abordagem e a natureza heuristica. A solucao produz boas agendas, mas nao prova otimalidade. Em trabalhos futuros, seria natural explorar:

- novas regras de prioridade
- buscas locais para melhorar agendas iniciais
- metaheuristicas para instancias maiores
- comparacoes com melhores valores conhecidos da literatura

Mesmo com essa limitacao, a abordagem atual atende bem ao objetivo do trabalho porque combina base teorica correta com capacidade pratica de processar todo o conjunto de instancias fornecido.

## 10. Conclusao

O projeto atende ao enunciado em todos os pontos centrais: modela o Job Shop como um DAG com pesos nos vertices, utiliza caminhada topologica, calcula caminho maximo e apresenta os resultados em todas as instancias JSP fornecidas. A principal qualidade da solucao e a coerencia entre teoria e implementacao: o mesmo modelo conceitual usado para explicar o problema e o que efetivamente mede a qualidade da agenda produzida.