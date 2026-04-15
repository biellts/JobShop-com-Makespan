# Roteiro de Apresentacao

Tempo alvo: 15 a 20 minutos.

## Slide 1 - Tema e objetivo

Fala sugerida:

"Nosso trabalho resolveu o problema de Job Shop Scheduling com objetivo de minimizar o makespan. A ideia central foi modelar cada operacao como vertice de um DAG com peso igual ao tempo de processamento, e usar caminhada topologica e caminho maximo para avaliar cada agenda."

Fala pronta detalhada:

"Neste trabalho escolhemos o problema de Job Shop Scheduling com foco em minimizacao do makespan, que e o tempo de termino da ultima operacao da agenda. A proposta central da nossa solucao foi representar o problema com um digrafo aciclico ponderado, em que cada operacao e um vertice e o peso do vertice corresponde ao tempo de processamento. A partir dessa modelagem, usamos caminhada topologica e calculo de caminho maximo para medir diretamente a qualidade da agenda gerada. Ou seja, a teoria de grafos nao ficou apenas como justificativa conceitual, ela foi usada como base efetiva do metodo de avaliacao da solucao." 

Mostre:

- titulo do trabalho
- opcao escolhida: Job Shop com Makespan
- pasta final com os resultados gerados

## Slide 2 - O que o enunciado exigia

Fala sugerida:

"O enunciado pedia tres pontos centrais: modelagem como digrafo com pesos nos vertices, caminhada topologica e exibicao da ordem dos vertices, do comprimento do caminho maximo e do proprio caminho maximo. Nossa implementacao foi organizada exatamente para cumprir esses pontos."

Fala pronta detalhada:

"O enunciado exigia que a solucao fosse modelada como um digrafo com pesos nos vertices e que a implementacao utilizasse caminhada topologica. Alem disso, tambem pedia para mostrar a ordem em que os vertices sao processados, o comprimento do caminho maximo e o proprio caminho maximo. Entao organizamos toda a solucao em funcao desses requisitos. Primeiro validamos a parte de ordenacao topologica e caminho maximo no grafo fixo do enunciado. Depois aplicamos o mesmo raciocinio ao problema geral de Job Shop, usando as instancias JSP fornecidas. Portanto, a estrutura do projeto foi pensada para responder diretamente ao que foi cobrado." 

Mostre:

- trecho do enunciado em [Trab_01_AlgGr_Shops.pdf](Trab_01_AlgGr_Shops.pdf)
- pasta de instancias JSP

## Slide 3 - Modelagem do problema

Fala sugerida:

"Cada operacao de um job vira um vertice. O peso desse vertice e o tempo de processamento. As arestas representam restricoes: ou a precedencia dentro do job, ou a ordem escolhida na mesma maquina. Quando essas duas familias de arestas sao combinadas, temos um DAG da agenda inteira."

Pontos para explicar:

- vertice = operacao: "Cada vertice representa uma unica operacao de um job. Isso significa que o grafo nao representa apenas jobs inteiros ou apenas maquinas, mas sim o nivel mais detalhado do problema, que e a operacao individual."
- peso = tempo da operacao: "O peso de cada vertice e o tempo de processamento daquela operacao. Esse detalhe e importante porque faz com que o caminho no grafo represente tempo acumulado real de execucao."
- aresta conjuntiva = precedencia do job: "As arestas conjuntivas representam a ordem interna de cada job. Elas garantem que uma operacao so possa ocorrer depois da operacao anterior do mesmo job ter sido concluida."
- aresta de recurso = ordem na maquina: "As arestas de recurso representam a ordem das operacoes em cada maquina. Como uma maquina nao pode executar duas operacoes ao mesmo tempo, precisamos fixar uma sequencia entre operacoes concorrentes."

Fala pronta detalhada:

"A modelagem do problema parte da ideia de que cada operacao e um vertice do grafo. O peso desse vertice e o tempo de processamento da operacao. Depois ligamos esses vertices com dois tipos de arestas. O primeiro tipo representa a precedencia dentro do job, isto e, a ordem tecnologica que precisa ser respeitada. O segundo tipo representa a ordem definida em cada maquina, ja que operacoes concorrentes precisam ser organizadas em uma sequencia sem sobreposicao. Quando juntamos essas duas estruturas, obtemos um DAG da agenda. Nesse ponto, o problema de scheduling passa a ser interpretado como um problema de dependencias temporais em um grafo ponderado." 

## Slide 4 - Conexao com a teoria

Fala sugerida:

"A conexao com teoria de grafos aparece no fato de que o makespan nao e calculado separadamente. Ele coincide com o maior caminho do DAG. Entao, depois de construir a agenda, fazemos uma ordenacao topologica e propagamos as distancias para obter o caminho critico."

Pontos para reforcar:

- DAG nao tem ciclo, entao caminho maximo pode ser calculado em ordem topologica: "Como o grafo final e aciclico, podemos percorrer os vertices em ordem topologica e calcular de forma eficiente o maior tempo acumulado ate cada vertice."
- o caminho critico explica por que o makespan tem aquele valor: "O caminho critico nao e apenas um resultado adicional. Ele mostra exatamente quais operacoes e dependencias determinam o valor final do makespan."
- teoria e implementacao estao alinhadas: "A teoria explica diretamente o algoritmo. Nao existe uma parte teorica separada e uma parte pratica desconectada. O mesmo modelo conceitual e o que produz o numero final da solucao."

Fala pronta detalhada:

"A conexao com a teoria aparece no fato de que, em um DAG, o problema do caminho maximo pode ser resolvido eficientemente se os vertices forem processados em ordem topologica. Isso foi usado diretamente no projeto. Depois que a agenda e transformada em grafo, calculamos para cada vertice o maior tempo acumulado necessario para alcanca-lo. O vertice final com maior valor determina o makespan. Ao reconstruir os predecessores desse vertice, obtemos o caminho critico. Portanto, o makespan coincide com o maior caminho do DAG, e isso conecta de forma direta a teoria de grafos com a avaliacao da agenda." 

## Slide 5 - Estrutura logica da solucao

Fala sugerida:

"Sem entrar em trechos de codigo, a solucao pode ser entendida como uma sequencia de etapas logicas. Primeiro lemos a instancia e reconstruimos as precedencias de cada job. Depois geramos uma agenda viavel por heuristica. Em seguida transformamos essa agenda em um DAG ponderado e finalmente aplicamos a caminhada topologica para obter o caminho critico e o makespan."

Pontos para explicar:

- leitura da instancia: "Primeiro, o programa le cada instancia e identifica quantos jobs existem, quantas maquinas existem e qual e o tempo de processamento de cada operacao. Essa etapa e importante porque os dados de entrada nao estao em formato de cronograma pronto. Eles trazem os tempos e as restricoes de precedencia, e por isso o primeiro trabalho da solucao e reconstruir corretamente a estrutura do problema."
- reconstrução da ordem das operacoes de cada job: "Depois da leitura, precisamos descobrir a ordem tecnologica de cada job. Isso acontece porque, nas instancias, cada job e descrito por relacoes de precedencia entre maquinas. Entao reconstruimos a sequencia valida de operacoes de cada job. Essa etapa garante que nenhuma operacao seja colocada antes de outra que deveria vir antes, preservando a corretude da solucao desde o inicio."
- construcao da agenda: "Com a estrutura dos jobs definida, passamos para a construcao da agenda. Nessa fase, a heuristica escolhe progressivamente qual operacao deve ser executada a seguir. Em cada passo, consideramos apenas operacoes elegiveis, ou seja, operacoes cujos predecessores ja foram atendidos. Depois analisamos a competicao por maquina e escolhemos a proxima operacao com base na regra de prioridade. Essa etapa transforma as restricoes do problema em uma ordem real de execucao."
- montagem do DAG da solucao: "Quando a agenda fica pronta, transformamos essa agenda em um DAG ponderado. Cada operacao continua sendo um vertice com peso igual ao tempo de processamento. As arestas representam duas coisas: a ordem das operacoes dentro de cada job e a ordem em que as operacoes ficaram organizadas em cada maquina. Esse grafo resume completamente a logica temporal da solucao, porque mostra quem depende de quem para poder executar."
- calculo do caminho critico e do makespan: "Na etapa final, fazemos a caminhada topologica sobre esse DAG e calculamos o maior caminho. Esse maior caminho e o caminho critico da agenda, isto e, a sequencia de operacoes que realmente determina o tempo final da solucao. Por isso, o comprimento desse caminho coincide com o makespan. Entao, no fim, o makespan nao aparece como um numero isolado: ele surge diretamente da analise do grafo que representa a agenda."

Fala pronta mais curta:

"A estrutura da solucao pode ser explicada em cinco passos. Primeiro, lemos a instancia e identificamos jobs, maquinas e tempos de processamento. Segundo, reconstruimos a ordem correta das operacoes de cada job para respeitar todas as precedencias. Terceiro, construimos a agenda usando uma heuristica, escolhendo passo a passo qual operacao sera executada em seguida. Quarto, depois de obter a agenda, montamos um DAG da solucao, em que cada vertice representa uma operacao e cada aresta representa uma dependencia temporal. Quinto, aplicamos a caminhada topologica nesse DAG para calcular o caminho critico. O comprimento desse caminho critico e exatamente o makespan da agenda." 

Fala pronta detalhada:

"A logica completa da solucao pode ser entendida como uma cadeia de cinco etapas. Na primeira etapa, lemos a instancia e identificamos a estrutura basica do problema: quantos jobs existem, quantas maquinas existem e quais sao os tempos de processamento associados. Na segunda etapa, reconstruimos a ordem valida das operacoes de cada job, porque as instancias fornecem precedencias que precisam ser interpretadas corretamente. Na terceira etapa, usamos uma heuristica para construir uma agenda viavel, sempre respeitando as restricoes de precedencia e de recurso. Na quarta etapa, transformamos a agenda resultante em um DAG ponderado que resume toda a logica temporal da solucao. Na quinta e ultima etapa, aplicamos a caminhada topologica para calcular o maior caminho desse grafo. Esse maior caminho e o caminho critico, e o seu comprimento e exatamente o makespan da agenda." 

## Slide 5.1 - Leitura e estruturacao da instancia

Fala sugerida:

"A leitura da instancia nao e trivial, porque os dados chegam em um formato que precisa ser interpretado e reestruturado. Criamos uma classe Instance com campos bem organizados para armazenar jobs, maquinas, tempos de processamento e precedencias. O arquivo e parseado sequencialmente: primeiro extraimos o cabecalho, depois a matriz de custos, e finalmente as relacoes de precedencia entre maquinas para cada job. A partir dessas relacoes, reconstruimos a ordem topologica valida de cada job. Depois geramos IDs unicos para operacoes e calculamos trabalho remanescente para cada uma. Tudo isso acontece antes da construcao da agenda, garantindo que os dados estejam corretos."

Fala pronta detalhada:

"A leitura da instancia e feita em etapas muito bem definidas, o que garante corretude logo no inicio. Primeiro, criamos uma estrutura de dados chamada Instance que organiza todos os dados relevantes: numero de jobs, numero de maquinas, tempos de processamento, e as precedencias entre operacoes. Depois, abrimos e parseamos o arquivo JSP sequencialmente. Extraimos o cabecalho com TotalJobs e TotalMachines, depois a matriz de custos que contem os tempos de cada operacao em cada maquina, e finalmente, para cada job, lemos as relacoes de precedencia entre maquinas fornecidas no formato de arestas. A partir dessas arestas de precedencia, reconstruimos a ordem topologica valida em que as maquinas devem ser visitadas para cada job. Isso garante que a estrutura tecnologica do job seja respeitada. Depois, criamos identificadores unicos e sequenciais para cada operacao, e calculamos para cada uma quanto trabalho remanescente ainda existe ate o fim do job. Esse trabalho remanescente sera usado depois pela heuristica de priorização. No final dessa etapa, a instancia esta completamente estruturada e pronta para a construcao da agenda. Essa estruturacao em dados bem organizados e o foco em corretude desde a leitura que permitem o resto da solucao funcionar de forma confiavel."

Pontos para destacar:

- classe Instance bem estruturada: "A classe Instance centraliza todos os dados da instancia em um lugar bem organizado, facilitando acesso e permitindo validacoes de consistencia."
- parsing sequencial do arquivo: "O arquivo e lido passo a passo: cabecalho, custos, precedencias. Cada parte tem um formato esperado, e validamos enquanto parseamos."
- reconstrucao da ordem topologica: "As instancias fornecem arestas de precedencia entre maquinas. A partir delas, reconstruimos a ordem em que as maquinas devem ser visitadas para cada job."
- identificadores unicos para operacoes: "Cada operacao recebe um ID sequencial global, o que torna facil referencia-la em estruturas de dados e na analise que vem depois."
- calculo de trabalho remanescente: "Para cada operacao, calculamos quanto tempo ainda falta processar ate o fim do job. Esse valor sera crucial para a heuristica de priorização."

Mostre:

- estrutura básica da classe Instance no codigo
- exemplo de arquivo JSP sendo parseado
- como os IDs de operacoes sao gerados sequencialmente

## Slide 6 - Decisao de projeto: heuristica

Fala sugerida:

"Para construir a agenda usamos Giffler-Thompson, uma heuristica classica para Job Shop. A intuicao dela e simples: em vez de decidir a agenda inteira de uma vez, ela vai escolhendo localmente qual operacao deve ocupar o recurso critico no proximo passo, sempre respeitando precedencias e indisponibilidade de maquinas."

Pontos para explicar:

- o que significa agenda ativa: "Uma agenda ativa e uma agenda em que nao existe antecipacao simples de operacoes sem violar restricoes. Em outras palavras, a agenda ja esta estruturalmente compacta dentro das decisoes tomadas."
- como identificar operacoes elegiveis: "Em cada passo, so podem ser consideradas operacoes cujos predecessores no job ja foram programados. Essas sao as operacoes elegiveis."
- por que usar Giffler-Thompson: "Essa heuristica e classica em Job Shop porque constrói agendas viaveis de forma sistematica e resolve conflitos locais de maquina de maneira consistente."
- como escolhe a operacao pivô: "A operacao pivô e a operacao elegivel com menor tempo de termino mais cedo. Ela serve como referencia para identificar quais outras operacoes realmente competem com ela naquele momento."
- como forma o conjunto de conflito: "O conjunto de conflito e formado pelas operacoes que usam a mesma maquina da pivô e que podem interferir no uso imediato desse recurso."
- como a prioridade influencia o resultado: "Depois que o conjunto de conflito e montado, a regra de prioridade decide qual operacao sera escolhida. Essa escolha local altera a estrutura global da agenda e, consequentemente, o makespan final."

Fala pronta detalhada:

"Para construir a agenda usamos a heuristica de Giffler-Thompson, que e uma das abordagens classicas para Job Shop. A ideia dela e nao tentar decidir a agenda inteira de uma vez. Em vez disso, ela trabalha passo a passo. Primeiro identifica quais operacoes estao elegiveis, ou seja, quais ja podem ser executadas porque seus predecessores foram atendidos. Depois calcula, entre essas candidatas, qual possui o menor tempo de termino mais cedo. Essa operacao funciona como pivô. A partir dela, montamos o conjunto de conflito, que contem as operacoes que disputam o mesmo recurso critico naquele instante. Por fim, aplicamos uma regra de prioridade para escolher qual operacao do conjunto sera executada primeiro. Esse processo vai sendo repetido ate que todas as operacoes tenham sido colocadas na agenda." 

## Slide 7 - Decisao de projeto: prioridade e robustez

Fala sugerida:

"A parte mais importante da heuristica e a regra de prioridade usada dentro do conjunto de conflito. Optamos por duas regras deterministicas porque elas sao simples de explicar, reproduziveis e coerentes com o objetivo de reduzir makespan. Tambem tratamos um caso especial de duracao zero, o que reforca a corretude da solucao."

Pontos para destacar:

- maior trabalho remanescente: "Nessa regra, priorizamos jobs que ainda possuem mais tempo total a ser processado."
- maior numero de operacoes remanescentes: "Nessa regra, priorizamos jobs que ainda possuem mais etapas a cumprir."
- por que jobs mais pesados merecem prioridade: "Jobs com muita carga remanescente tendem a virar gargalos no final da agenda se forem negligenciados."
- por que jobs com muitas etapas restantes podem gerar gargalos: "Quanto mais operacoes ainda faltam para um job, maior o risco de acumular atrasos por encadeamento de dependencias."
- tratamento do caso de operacao com duracao zero: "Encontramos uma instancia com operacao de duracao zero, e isso exigiu um cuidado extra na formacao do conjunto de conflito para manter a corretude do algoritmo."

Fala pronta detalhada:

"A decisao mais importante dentro da heuristica esta na regra de prioridade usada para escolher uma operacao do conjunto de conflito. Na configuracao padrao, usamos duas regras deterministicas. A primeira prioriza jobs com maior trabalho remanescente. A intuicao aqui e simples: jobs mais pesados podem concentrar atraso no fim do cronograma, entao faz sentido dar atencao a eles mais cedo. A segunda prioriza jobs com maior numero de operacoes remanescentes. Nesse caso, a ideia e evitar que jobs com muitas etapas futuras acabem criando cadeias longas de bloqueio. Alem disso, durante os testes apareceu uma instancia com operacao de duracao zero. Isso nos obrigou a ajustar o tratamento do conjunto de conflito para garantir robustez e corretude mesmo em casos menos usuais." 

## Slide 8 - Boas praticas de programacao

Fala sugerida:

"Mesmo sem mostrar o codigo, e importante destacar as boas praticas adotadas: a solucao foi organizada em etapas bem definidas, ha validacoes de consistencia, tratamento de erros e geracao automatica dos resultados. Isso melhora legibilidade, reproducao e confiabilidade."

Pontos para citar:

- separacao entre leitura, construcao da agenda e avaliacao por grafo: "Essas etapas foram mantidas separadas para que o raciocinio do projeto fique claro e verificavel."
- validacao de consistencia das instancias e do DAG: "Ha verificacoes para garantir que as precedencias sao interpretadas corretamente e que o grafo final permanece aciclico."
- reproducao automatica dos resultados: "Os resultados foram gerados automaticamente para todas as instancias, o que facilita repeticao e conferência."
- foco em corretude antes de complexidade extra: "Optamos por uma solucao clara e defensavel, evitando acrescentar complexidade que dificultasse a explicacao sem ganho proporcional."

Fala pronta detalhada:

"Mesmo sem mostrar o codigo, vale destacar as boas praticas adotadas no projeto. A primeira foi separar claramente as etapas de leitura da instancia, construcao da agenda e avaliacao por grafo. Isso melhora a organizacao e tambem facilita justificar a corretude da solucao. A segunda foi validar a consistencia das instancias e do DAG, evitando aceitar estruturas que quebrassem a logica do problema. A terceira foi automatizar a geracao dos resultados para todas as instancias, o que melhora reproducao e confiabilidade. E, por fim, priorizamos corretude e clareza antes de adicionar tecnicas mais complexas que poderiam dificultar a apresentacao sem necessariamente melhorar a defesa do trabalho." 

## Slide 9 - Teste do grafo fixo

Fala sugerida:

"Antes de aplicar a ideia ao Job Shop, testamos a caminhada topologica no grafo fixo do enunciado. Isso foi importante para validar separadamente a parte de ordenacao topologica e caminho maximo em DAG, antes de conectar esse mecanismo ao problema geral."

Fala pronta detalhada:

"Antes de levar a abordagem para o problema geral de Job Shop, fizemos o teste no grafo fixo fornecido pelo enunciado. Esse teste foi importante porque isolou a parte de teoria de grafos do restante do problema. Nele, validamos a ordenacao topologica, o calculo do caminho maximo e a reconstrucao dos caminhos ate os vertices finais pedidos no trabalho. Isso nos deu seguranca de que a base teorica da solucao estava correta antes de aplicar a mesma ideia ao conjunto completo de instancias JSP." 

Mostre:

- [output/fixed_graph_report.txt](output/fixed_graph_report.txt)

## Slide 10 - Resultados nas instancias JSP

Fala sugerida:

"Depois executamos todas as 162 instancias JSP. Para cada instancia, obtivemos uma agenda, o makespan correspondente e o caminho critico da solucao. Tambem geramos uma tabela consolidada e um detalhamento por instancia para ter evidencias completas dos resultados."

Fala pronta detalhada:

"Depois da validacao no grafo fixo, executamos a solucao em todas as 162 instancias JSP fornecidas. Para cada instancia, o programa gerou uma agenda viavel, calculou o makespan, identificou o caminho critico e registrou essas informacoes em arquivos de saida. Alem disso, produzimos uma tabela consolidada com todos os resultados e um detalhamento por instancia, incluindo sequencias de maquina, sequencias de job e dados do caminho critico. Isso foi importante para garantir que a entrega nao ficasse restrita a exemplos isolados, mas cobrisse todo o conjunto pedido." 

Mostre:

- [output/jsp_makespan_results.csv](output/jsp_makespan_results.csv)
- [output/results_table.pdf](output/results_table.pdf)
- [output/details](output/details)

## Slide 11 - Leitura critica dos resultados

Fala sugerida:

"Os resultados crescem de forma coerente com o porte das instancias. Isso e importante porque mostra que a modelagem e a avaliacao estao alinhadas com a estrutura do problema. Instancias pequenas geram agendas menores; instancias maiores e mais densas apresentam makespans muito mais altos, o que e exatamente o comportamento esperado."

Pontos para comentar:

- media geral de makespan: 2253.06: "Esse valor resume o comportamento global do conjunto, embora a analise principal deva considerar tambem o porte e a familia de cada instancia."
- diferenca entre familias de instancias: "As familias possuem escalas e dificuldades diferentes, e isso aparece naturalmente nos resultados."
- consistencia dos resultados com o tamanho do problema: "Instancias menores tendem a ter makespans menores, enquanto instancias maiores e mais densas apresentam valores bem mais altos."
- por que makespan alto nao significa erro, mas sim maior dificuldade estrutural: "Em problemas de scheduling, makespan alto muitas vezes reflete simplesmente a dificuldade da instancia e a quantidade de restricoes, e nao um erro de modelagem."

Fala pronta detalhada:

"Na leitura dos resultados, o aspecto mais importante nao e olhar apenas um numero isolado, mas observar a coerencia do comportamento das instancias. A media geral de makespan foi 2253.06, mas o que realmente importa e que os valores crescem de forma compatível com o porte e a estrutura das familias de instancias. Instancias pequenas tendem a gerar makespans menores. Ja instancias maiores, com mais jobs, mais maquinas e mais interdependencias, apresentam makespans significativamente maiores. Isso nao indica erro. Pelo contrario, mostra que a modelagem esta respondendo de forma consistente ao aumento da complexidade estrutural do problema." 

## Slide 12 - Fechamento

Fala sugerida:

"Em resumo, o projeto atende ao enunciado porque modela o Job Shop como DAG com pesos nos vertices, usa caminhada topologica para calcular caminho maximo e gera resultados para todas as instancias JSP. O ponto mais forte da abordagem e a coerencia: a mesma teoria que usamos para explicar o problema foi usada diretamente para medir a qualidade de cada agenda."

Fala pronta detalhada:

"Para concluir, o principal ponto forte do projeto e a coerencia entre modelagem, algoritmo e avaliacao. O problema foi representado como um DAG ponderado, a agenda foi construida por uma heuristica apropriada para Job Shop e o makespan foi calculado diretamente pelo caminho maximo desse grafo. Alem disso, a solucao foi testada em todas as instancias JSP fornecidas, e os resultados foram organizados de forma completa para a entrega. Portanto, acreditamos que o trabalho atende ao enunciado nao apenas formalmente, mas tambem conceitualmente, porque a teoria de grafos foi realmente usada como base do metodo." 

## Orientacao importante para a apresentacao

- nao mostrar trechos de codigo na explicacao principal
- priorizar figuras, fluxo logico, grafo fixo e tabela de resultados
- usar o codigo apenas se a banca pedir detalhes tecnicos especificos
- focar em algoritmo, modelagem, heuristica, corretude e interpretacao dos resultados

## Perguntas que podem aparecer

### Por que o grafo final e aciclico?

Resposta curta:

"Porque a ordem de cada job ja e aciclica por definicao, e na maquina escolhemos uma sequencia linear entre operacoes concorrentes. Quando essas duas estruturas sao combinadas corretamente, obtemos um grafo sem ciclos, adequado para ordenacao topologica."

### Por que o makespan coincide com o caminho maximo?

Resposta curta:

"Porque cada vertice representa uma operacao com seu tempo de processamento, e cada caminho representa uma cadeia de dependencias que impede paralelismo completo. O maior desses caminhos determina o tempo minimo necessario para concluir a agenda inteira."

### Por que usar heuristica e nao busca exata?

Resposta curta:

"Porque Job Shop e um problema dificil e o conjunto de instancias fornecido e grande. Uma abordagem exata seria muito mais pesada para explicar e executar. A heuristica permite gerar solucoes boas, coerentes e em tempo pratico, sem perder a conexao com a modelagem em grafo pedida no trabalho."

### Como voces escolheram a heuristica Giffler-Thompson?

Resposta completa:

"Giffler-Thompson e uma heuristica classica e bem estabelecida para Job Shop. Ela tem varias qualidades: primeiro, constrói agendas ativas, que sao agendas estruturalmente compactas. Segundo, opera de forma sistematica, escolhendo a cada passo qual operacao deve ocupar o recurso critico, respeitando sempre as precedencias. Terceiro, permite experimentos com diferentes regras de prioridade sem mudar a estrutura base. Quarto, o resultado final sempre forma um DAG, o que nos permite aplicar a teoria de grafos conforme exigido no trabalho. Por esses motivos, ela foi a escolha natural."

### Qual seria o impacto de trocar a regra de prioridade?

Resposta completa:

"A regra de prioridade altera localmente qual operacao sera colocada na agenda, e isso altera a estrutura global do DAG e o valor final do makespan. Testamos duas regras: maior trabalho remanescente e maior numero de operacoes remanescentes. Ambas sao simples de explicar e deterministicas, o que facilita reproducao. Uma regra mecanica que nao considerasse o remanescente teria desempenho pior, porque nao priorizaria jobs pesados que poderiam virar gargalos. Se usassemos uma regra aleatoria, perderíamos reproducibilidade e teriamos dificuldade em discutir a qualidade da solucao."

### Como voces garantem que o DAG nao tem ciclos?

Resposta completa:

"Primeiro, as arestas conjuntivas, que representam a ordem dentro de cada job, nao formam ciclos porque ja vem em uma ordem linear definida nos dados de entrada. Segundo, as arestas de recurso, que representam a ordem nas maquinas, sao adicionadas de forma a formar uma sequencia linear sem retorno. Terceiro, quando combinamos essas duas familias de forma correta, o grafo resultante herda a estrutura aciclica de ambas. Tambem incluimos validacoes no codigo para detectar ciclos em caso de inconsistencia, o que nunca aconteceu nos testes, confirmando que o grafo permanece aciclico."

### Por que a escolha do vertice pivô usa o menor tempo de termino mais cedo?

Resposta completa:

"A operacao pivô e aquela que ocupará o recurso critico mais cedo entre as operacoes elegiveis. Escolher a com menor tempo de termino mais cedo e uma escolha natural porque ela define o momento em que o recurso fica realmente ocupado. A partir desse ponto, identificamos o conjunto de operacoes que competem pelo mesmo recurso naquele intervalo, e é sobre esse conjunto que a regra de prioridade atua. Essa logica mantém coerencia: a pivô marca o evento critico, e a prioridade decide como resolvê-lo."

### Como sao tratadas as precedencias entre operacoes do mesmo job?

Resposta completa:

"Cada job tem uma sequencia de operacoes definida nos dados de entrada. Essas operacoes ja vem com uma ordem tecnologica fixa. No grafo, essa ordem vira uma cadeia de arestas conjuntivas que liga as operacoes do job em sequencia linear. Quando geramos a agenda, essas arestas sao respeitadas: uma operacao so pode ser colocada no cronograma depois que seu predecessor no job ja foi colocado. Isso garante que a ordem tecnologica seja mantida durante toda a construcao da agenda."

### Qual e a complexidade computacional do algoritmo?

Resposta completa:

"A complexidade pode ser dividida em partes. A leitura da instancia e a reconstrucao das precedencias levam tempo linear no tamanho da instancia. A construcao da agenda com Giffler-Thompson, em cada passo, percorre as operacoes elegiveis para escolher a pivô e depois o conjunto de conflito, o que no pior caso pode ser quadratico no numero de operacoes. A caminhada topologica e o calculo do caminho maximo no DAG sao lineares no numero de vertices e arestas. No geral, a abordagem e pratica e escalável para as 162 instancias fornecidas, mesmo as maiores."

### O que significa uma operacao ter duracao zero, e por que isso e um problema especial?

Resposta completa:

"Uma operacao com duracao zero e uma operacao que nao consome tempo de processamento na maquina. Encontramos esse caso em algumas instancias. Isso e um problema especial porque, na formacao do conjunto de conflito, operacoes com duracao zero nao deixam um intervalo claro que outras operacoes possam ocupar. Por isso, precisamos de um cuidado extra: consideramos que uma operacao com duracao zero ainda ocupa o recurso por um infinitesimal de tempo, e por isso ainda pode interferir na ordem das outras operacoes. Esse tratamento garante que a logica da agenda permaneca coerente mesmo em casos incomuns."

### Como voces interpretam um makespan alto? Indica erro da heuristica?

Resposta completa:

"Nao necessariamente. Um makespan alto pode significar simplesmente que a instancia e dificil estruturalmente. Instancias maiores, com mais jobs, mais maquinas ou mais restricoes, tendem a ter makespans maiores. A coisa importante nao e o numero isolado, mas se o resultado cresce de forma coerente com o tamanho da instancia. Nos testes, vimos que instancias menores geram makespans menores, e instancias maiores geram makespans maiores, o que e o comportamento esperado. Isso sugere que a heuristica esta respondendo corretamente à estrutura do problema."

### Como se calcula o caminho critico no DAG?

Resposta completa:

"Primeiro, fazemos uma ordenacao topologica dos vertices do DAG. Depois, para cada vertice, calculamos a maior distancia desde o inicio ate ele, considerando os pesos dos vertices (tempos de processamento) e as arestas (dependencias). Isso pode ser feito de forma eficiente em uma unica passagem se processarmos os vertices em ordem topologica: para cada vertice, consideramos todos os seus predecessores ja calculados, somamos o peso do vertice ao maior valor dos predecessores, e registramos esse valor. No final, o vertice com maior valor é a extremidade do caminho critico. Depois, rastrejamos os predecessores para reconstruir o caminho inteiro."

### E possível melhorar o resultado inicial de forma iterativa?

Resposta completa:

"Sim, existem varias abordagens, mas nao foram o foco desse trabalho. Uma opcao seria usar a agenda gerada como solucao inicial e aplicar tecnicas de melhoria local, como busca tabu ou simulated annealing. Outro caminho seria executar Giffler-Thompson multiplas vezes com diferentes regras de prioridade e escolher a melhor. Porem, o objetivo do trabalho era demonstrar a modelagem em grafo e o calculo do caminho maximo. Essas melhorias sao importantes em pratica, mas acrescentariam complexidade à explicacao sem ser estritamente necessario para o enunciado."

### Como voces validaram a corretude da solucao?

Resposta completa:

"A validacao foi feita em multiplos niveis. Primeiro, testamos a caminhada topologica no grafo fixo do enunciado para garantir que a teoria estava funcionando isoladamente. Segundo, para cada instancia JSP, validamos que o grafo gerado permanecia aciclico e que as precedencias eram respeitadas. Terceiro, verificamos que o makespan calculado era coerente com a agenda produzida, isto e, nenhuma operacao iniciava antes que seus predecessores terminassem. Quarto, os resultados crescem de forma logica com o porte das instancias. Essas validacoes multiplas nos deram confianca na corretude da implementacao."

### Por que escolheram representar cada operacao como um vertice e nao usar outra modelagem?

Resposta completa:

"Porque o enunciado pedia explicitamente uma modelagem com arestas e pesos nos vertices. Existem outras formas de modelar Job Shop, como grafos com arestas ponderadas ou redes PERT, mas essas nao atenderiam ao que foi exigido. Alem disso, colocar peso nos vertices e uma escolha natural aqui, porque cada vertice representa a operacao inteira com seu tempo de processamento. Isso faz com que o caminho no grafo represente tempo acumulado real, e o caminho maximo represente exatamente o makespan. Entao a modelagem adotada nao foi arbitraria: foi escolhida para atender ao enunciado e para manter coerencia entre teoria e resultado final."

### Como as arestas de recurso (de maquina) sao definidas?

Resposta completa:

"As arestas de recurso ligam operacoes que compartilham a mesma maquina na ordem em que foram programadas. Se duas operacoes A e B usam a mesma maquina e A deve ser executada antes de B na agenda, entao adicionamos uma aresta de A para B. Essas arestas sao criadas durante a construcao da agenda: a cada vez que uma operacao é colocada no cronograma, atualizamos a ordem de operacoes em cada maquina. Ao final, essas arestas formam cadeias lineares dentro de cada maquina, que sao exatamente as sequencias de execucao daquela maquina na agenda gerada."

### Os resultados sao otimos ou apenas bons?

Resposta completa:

"Nossa solucao nao encontra o makespan global otimo para cada instancia. Isso seria um problema muito mais dificil, porque Job Shop e NP-dificil. A nossa heuristica encontra uma solucao viavel e de boa qualidade cuja qualidade pode ser explicada diretamente pela estrutura do grafo e pelo caminho maximo. Isso e suficiente para o escopo do trabalho, que era modelar o problema em grafo e usar a teoria de grafos para avaliacao, nao otimizar globalmente. Uma busca de solucao otima teria um custo computacional muito maior e teria menos relacao com o conteudo de grafos pedido no trabalho."

### Como lidam com empates na escolha de prioridade?

Resposta completa:

"Quando dois ou mais jobs no conjunto de conflito empatam na regra de prioridade, usamos uma ordem secundaria para desempate. Essa orden secundaria pode ser o indice do job ou uma segunda metrica. Na implementacao, quando ocorrem empates, escolhemos de forma deterministica e consistente, o que permite reproducao dos resultados. Isso nao afeta a corretude, porque qualquer escolha valida do conjunto de conflito produz uma agenda viavel. A orden secundaria apenas assegura que o resultado seja sempre igual para a mesma entrada."

### O tamanho do arquivo de resultados é proporcional ao número de instancias?

Resposta completa:

"Sim. Para cada instancia JSP, geramos um arquivo de detalhes contendo a agenda, a sequencia de cada maquina, a sequencia de cada job e o caminho critico. Com 162 instancias, isso resulta em 162 arquivos de detalhes. Alem disso, geramos um arquivo CSV consolidado com um resumo de cada instancia. O tamanho total fica na ordem de megabytes, o que é completamente pratico para armazenar e revisar. Os arquivos estao organizados para permitir verificacao manual de exemplos pequenos e analise automatica do conjunto completo."

### Como voces explicariam a solucao para um publico nao-tecnico?

Resposta completa:

"Basicamente, é assim: imagine que voce tem varios cargos em maquinas diferentes, e precisa decidir em que ordem executá-los. O que importa é quando voce termina tudo. Para resolver isso, a gente modela cada cargo como um ponto em um grafo, com peso igual ao tempo dele. Se um cargo depende de outro, a gente liga os dois com uma seta. Quando todos os cargos estão conectados assim, a gente procura o caminho mais longo pelo grafo. Esse caminho mais longo é exatamente quanto tempo você precisa para terminar tudo. A gente usa uma estrategia simples para organizar os cargos de uma forma que fica boa, depois a gente mede a qualidade usando esse grafo e esse caminho. Simples assim."

### Como a instância é lida e estruturada no programa?

Resposta completa:

"A leitura da instância é feita em etapas muito bem definidas, o que garante corretude logo no inicio. Primeiro, criamos uma estrutura de dados chamada Instance que organiza todos os dados relevantes: numero de jobs, numero de maquinas, tempos de processamento, e as precedencias entre operacoes. Depois, abrimos e parseamos o arquivo JSP sequencialmente. Extraimos o cabeçalho com TotalJobs e TotalMachines, depois a matriz de custos que contem os tempos de cada operacao em cada maquina, e finalmente, para cada job, lemos as relacoes de precedencia entre maquinas fornecidas no formato de arestas. A partir dessas arestas de precedencia, reconstruimos a ordem topologica valida em que as maquinas devem ser visitadas para cada job. Isso garante que a estrutura tecnologica do job seja respeitada. Depois, criamos identificadores unicos e sequenciais para cada operacao, e calculamos para cada uma quanto trabalho remanescente ainda existe ate o fim do job. Esse trabalho remanescente sera usado depois pela heuristica de priorização. No final dessa etapa, a instancia esta completamente estruturada e pronta para a construcao da agenda. Essa estruturacao em dados bem organizados e o foco em corretude desde a leitura que permitem o resto da solucao funcionar de forma confiavel."