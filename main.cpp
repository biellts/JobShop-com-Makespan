#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

struct OperationInfo {
    int id = -1;
    int job = -1;
    int position = -1;
    int machine = -1;
    int duration = 0;
};

struct Instance {
    std::string name;
    fs::path path;
    int jobs = 0;
    int machines = 0;
    std::vector<std::vector<int>> costs;
    std::vector<std::vector<int>> jobMachineOrder;
    std::vector<std::vector<int>> jobOperationIds;
    std::vector<OperationInfo> operations;
    std::vector<std::vector<int>> remainingWork;
};

struct Schedule {
    std::vector<int> startTime;
    std::vector<int> finishTime;
    std::vector<int> scheduledOrder;
    std::vector<std::vector<int>> machineOrder;
    int makespan = 0;
    int priorityMode = 0;
    std::uint32_t seed = 0;
};

struct DagAnalysis {
    std::vector<int> topoOrder;
    std::vector<long long> longestDistance;
    std::vector<int> parent;
    int longestEnd = -1;
    long long longestValue = 0;
};

struct SolvedInstance {
    Instance instance;
    Schedule schedule;
    DagAnalysis dag;
    std::vector<std::vector<int>> adjacency;
};

struct FixedGraphResult {
    DagAnalysis dag;
    std::vector<std::vector<int>> adjacency;
    std::vector<int> weights;
};

[[noreturn]] void fail(const std::string& message) {
    throw std::runtime_error(message);
}

std::string joinIntegers(const std::vector<int>& values, const std::string& separator) {
    std::ostringstream out;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            out << separator;
        }
        out << values[index];
    }
    return out.str();
}

std::string joinStrings(const std::vector<std::string>& values, const std::string& separator) {
    std::ostringstream out;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            out << separator;
        }
        out << values[index];
    }
    return out.str();
}

template <typename LabelBuilder>
std::string formatPath(const std::vector<int>& path, LabelBuilder buildLabel) {
    std::vector<std::string> labels;
    labels.reserve(path.size());
    for (int vertex : path) {
        labels.push_back(buildLabel(vertex));
    }
    return joinStrings(labels, " -> ");
}

std::vector<int> restorePath(int endVertex, const std::vector<int>& parent) {
    std::vector<int> path;
    int current = endVertex;
    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

DagAnalysis analyzeDag(const std::vector<std::vector<int>>& adjacency, const std::vector<int>& vertexWeight) {
    const int vertexCount = static_cast<int>(adjacency.size());
    std::vector<int> indegree(vertexCount, 0);
    for (int from = 0; from < vertexCount; ++from) {
        for (int to : adjacency[from]) {
            ++indegree[to];
        }
    }

    std::priority_queue<int, std::vector<int>, std::greater<int>> ready;
    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        if (indegree[vertex] == 0) {
            ready.push(vertex);
        }
    }

    DagAnalysis analysis;
    analysis.longestDistance.assign(vertexCount, std::numeric_limits<long long>::min());
    analysis.parent.assign(vertexCount, -1);

    while (!ready.empty()) {
        const int vertex = ready.top();
        ready.pop();
        analysis.topoOrder.push_back(vertex);

        if (analysis.longestDistance[vertex] == std::numeric_limits<long long>::min()) {
            analysis.longestDistance[vertex] = vertexWeight[vertex];
        }

        for (int next : adjacency[vertex]) {
            const long long candidate = analysis.longestDistance[vertex] + vertexWeight[next];
            if (candidate > analysis.longestDistance[next]) {
                analysis.longestDistance[next] = candidate;
                analysis.parent[next] = vertex;
            }
            --indegree[next];
            if (indegree[next] == 0) {
                ready.push(next);
            }
        }
    }

    if (static_cast<int>(analysis.topoOrder.size()) != vertexCount) {
        fail("O grafo analisado contem ciclo e nao pode ser processado topologicamente.");
    }

    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        if (analysis.longestDistance[vertex] > analysis.longestValue) {
            analysis.longestValue = analysis.longestDistance[vertex];
            analysis.longestEnd = vertex;
        }
    }
    return analysis;
}

std::vector<int> topoSortMachines(int machines, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<int>> adjacency(machines);
    std::vector<int> indegree(machines, 0);
    for (const auto& [from, to] : edges) {
        adjacency[from].push_back(to);
        ++indegree[to];
    }

    std::priority_queue<int, std::vector<int>, std::greater<int>> ready;
    for (int machine = 0; machine < machines; ++machine) {
        if (indegree[machine] == 0) {
            ready.push(machine);
        }
    }

    std::vector<int> order;
    order.reserve(machines);
    while (!ready.empty()) {
        const int machine = ready.top();
        ready.pop();
        order.push_back(machine);
        for (int next : adjacency[machine]) {
            --indegree[next];
            if (indegree[next] == 0) {
                ready.push(next);
            }
        }
    }

    if (static_cast<int>(order.size()) != machines) {
        fail("A ordem das operacoes do job nao forma um DAG valido.");
    }
    return order;
}

Instance parseInstance(const fs::path& path) {
    std::ifstream input(path);
    if (!input) {
        fail("Nao foi possivel abrir a instancia: " + path.string());
    }

    Instance instance;
    instance.path = path;
    instance.name = path.stem().string();

    std::string token;
    while (input >> token) {
        if (token == "TotalJobs:") {
            break;
        }
    }
    if (token != "TotalJobs:") {
        fail("Cabecalho inesperado ao ler TotalJobs em " + path.string());
    }
    input >> instance.jobs;

    input >> token;
    if (token != "TotalMachines:") {
        fail("Cabecalho inesperado ao ler TotalMachines em " + path.string());
    }
    input >> instance.machines;

    input >> token;
    if (token != "Costs:") {
        fail("Secao Costs nao encontrada em " + path.string());
    }

    instance.costs.assign(instance.jobs, std::vector<int>(instance.machines, 0));
    for (int job = 0; job < instance.jobs; ++job) {
        for (int machine = 0; machine < instance.machines; ++machine) {
            input >> instance.costs[job][machine];
        }
    }

    instance.jobMachineOrder.resize(instance.jobs);
    for (int job = 0; job < instance.jobs; ++job) {
        input >> token;
        if (token != "Job:") {
            fail("Secao Job nao encontrada para o job " + std::to_string(job) + " em " + path.string());
        }
        int edgeCount = 0;
        input >> edgeCount;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(edgeCount);
        for (int edge = 0; edge < edgeCount; ++edge) {
            int from = 0;
            int to = 0;
            std::string arrow;
            input >> from >> arrow >> to;
            if (arrow != "->") {
                fail("Aresta de precedencia invalida em " + path.string());
            }
            edges.emplace_back(from, to);
        }
        instance.jobMachineOrder[job] = topoSortMachines(instance.machines, edges);
    }

    instance.jobOperationIds.assign(instance.jobs, std::vector<int>(instance.machines, -1));
    instance.remainingWork.assign(instance.jobs, std::vector<int>(instance.machines, 0));

    int nextId = 0;
    for (int job = 0; job < instance.jobs; ++job) {
        for (int position = 0; position < instance.machines; ++position) {
            const int machine = instance.jobMachineOrder[job][position];
            const int duration = instance.costs[job][machine];
            instance.jobOperationIds[job][position] = nextId;
            instance.operations.push_back(OperationInfo{nextId, job, position, machine, duration});
            ++nextId;
        }
        int suffix = 0;
        for (int position = instance.machines - 1; position >= 0; --position) {
            const int machine = instance.jobMachineOrder[job][position];
            suffix += instance.costs[job][machine];
            instance.remainingWork[job][position] = suffix;
        }
    }

    return instance;
}

std::string operationLabel(const Instance& instance, int operationId) {
    const OperationInfo& operation = instance.operations[operationId];
    std::ostringstream out;
    out << "J" << (operation.job + 1)
        << "O" << (operation.position + 1)
        << "[M" << (operation.machine + 1)
        << ",p=" << operation.duration << "]";
    return out.str();
}

struct Candidate {
    int job = -1;
    int position = -1;
    int operationId = -1;
    int machine = -1;
    int duration = 0;
    int earliestStart = 0;
    int earliestCompletion = 0;
    int remainingWork = 0;
    int remainingOps = 0;
};

bool betterCandidate(const Candidate& challenger, const Candidate& current, int mode, std::mt19937& rng) {
    auto compareLexicographically = [](const auto& left, const auto& right) {
        return left > right;
    };

    switch (mode) {
        case 0: {
            const auto left = std::make_tuple(challenger.remainingWork, challenger.remainingOps, -challenger.earliestStart, -challenger.duration, -challenger.job);
            const auto right = std::make_tuple(current.remainingWork, current.remainingOps, -current.earliestStart, -current.duration, -current.job);
            return compareLexicographically(left, right);
        }
        case 1: {
            const auto left = std::make_tuple(challenger.remainingOps, challenger.remainingWork, -challenger.duration, -challenger.job);
            const auto right = std::make_tuple(current.remainingOps, current.remainingWork, -current.duration, -current.job);
            return compareLexicographically(left, right);
        }
        case 2: {
            const auto left = std::make_tuple(challenger.duration, challenger.remainingWork, -challenger.job);
            const auto right = std::make_tuple(current.duration, current.remainingWork, -current.job);
            return compareLexicographically(left, right);
        }
        case 3: {
            const auto left = std::make_tuple(-challenger.duration, challenger.remainingWork, -challenger.job);
            const auto right = std::make_tuple(-current.duration, current.remainingWork, -current.job);
            return compareLexicographically(left, right);
        }
        default: {
            std::uniform_int_distribution<int> noise(0, 1000);
            const int leftScore = challenger.remainingWork * 1000 + challenger.remainingOps * 100 + noise(rng);
            const int rightScore = current.remainingWork * 1000 + current.remainingOps * 100 + noise(rng);
            if (leftScore != rightScore) {
                return leftScore > rightScore;
            }
            const auto left = std::make_tuple(challenger.duration, -challenger.earliestStart, -challenger.job);
            const auto right = std::make_tuple(current.duration, -current.earliestStart, -current.job);
            return compareLexicographically(left, right);
        }
    }
}

Schedule buildSchedule(const Instance& instance, int priorityMode, std::uint32_t seed) {
    std::mt19937 rng(seed);
    const int jobCount = instance.jobs;
    const int machineCount = instance.machines;
    const int operationCount = static_cast<int>(instance.operations.size());

    Schedule schedule;
    schedule.priorityMode = priorityMode;
    schedule.seed = seed;
    schedule.startTime.assign(operationCount, 0);
    schedule.finishTime.assign(operationCount, 0);
    schedule.machineOrder.assign(machineCount, {});
    schedule.scheduledOrder.reserve(operationCount);

    std::vector<int> nextPosition(jobCount, 0);
    std::vector<int> jobReady(jobCount, 0);
    std::vector<int> machineReady(machineCount, 0);

    for (int scheduled = 0; scheduled < operationCount; ++scheduled) {
        std::vector<Candidate> ready;
        ready.reserve(jobCount);
        for (int job = 0; job < jobCount; ++job) {
            if (nextPosition[job] >= machineCount) {
                continue;
            }
            const int position = nextPosition[job];
            const int operationId = instance.jobOperationIds[job][position];
            const OperationInfo& operation = instance.operations[operationId];
            const int earliestStart = std::max(jobReady[job], machineReady[operation.machine]);
            const int earliestCompletion = earliestStart + operation.duration;
            ready.push_back(Candidate{
                job,
                position,
                operationId,
                operation.machine,
                operation.duration,
                earliestStart,
                earliestCompletion,
                instance.remainingWork[job][position],
                machineCount - position
            });
        }

        if (ready.empty()) {
            fail("Nenhuma operacao pronta foi encontrada durante a construcao da agenda.");
        }

        Candidate pivot = ready.front();
        for (const Candidate& candidate : ready) {
            const auto left = std::make_tuple(candidate.earliestCompletion, candidate.earliestStart, candidate.job, candidate.operationId);
            const auto right = std::make_tuple(pivot.earliestCompletion, pivot.earliestStart, pivot.job, pivot.operationId);
            if (left < right) {
                pivot = candidate;
            }
        }

        std::vector<Candidate> conflictSet;
        for (const Candidate& candidate : ready) {
            if (candidate.machine == pivot.machine && (candidate.operationId == pivot.operationId || candidate.earliestStart < pivot.earliestCompletion)) {
                conflictSet.push_back(candidate);
            }
        }

        Candidate chosen = conflictSet.front();
        for (std::size_t index = 1; index < conflictSet.size(); ++index) {
            if (betterCandidate(conflictSet[index], chosen, priorityMode, rng)) {
                chosen = conflictSet[index];
            }
        }

        const int startTime = chosen.earliestStart;
        const int finishTime = chosen.earliestCompletion;
        schedule.startTime[chosen.operationId] = startTime;
        schedule.finishTime[chosen.operationId] = finishTime;
        schedule.scheduledOrder.push_back(chosen.operationId);
        schedule.machineOrder[chosen.machine].push_back(chosen.operationId);
        jobReady[chosen.job] = finishTime;
        machineReady[chosen.machine] = finishTime;
        schedule.makespan = std::max(schedule.makespan, finishTime);
        ++nextPosition[chosen.job];
    }

    return schedule;
}

std::vector<std::vector<int>> buildScheduleDag(const Instance& instance, const Schedule& schedule) {
    const int operationCount = static_cast<int>(instance.operations.size());
    std::vector<std::vector<int>> adjacency(operationCount);
    for (int job = 0; job < instance.jobs; ++job) {
        for (int position = 1; position < instance.machines; ++position) {
            const int previous = instance.jobOperationIds[job][position - 1];
            const int current = instance.jobOperationIds[job][position];
            adjacency[previous].push_back(current);
        }
    }
    for (const auto& machineSequence : schedule.machineOrder) {
        for (std::size_t index = 1; index < machineSequence.size(); ++index) {
            adjacency[machineSequence[index - 1]].push_back(machineSequence[index]);
        }
    }
    return adjacency;
}

SolvedInstance solveInstance(const Instance& instance, int randomizedIterations) {
    struct TrialConfig {
        int mode = 0;
        std::uint32_t seed = 0;
    };

    std::vector<TrialConfig> trials;
    trials.push_back({0, 101u});
    trials.push_back({1, 202u});

    const std::uint32_t instanceHash = static_cast<std::uint32_t>(std::hash<std::string>{}(instance.name));
    for (int iteration = 0; iteration < randomizedIterations; ++iteration) {
        const int mode = 4 + (iteration % 3);
        const std::uint32_t seed = instanceHash ^ static_cast<std::uint32_t>(iteration * 2654435761u + 17u);
        trials.push_back({mode, seed});
    }

    std::optional<Schedule> bestSchedule;
    DagAnalysis bestDag;
    std::vector<std::vector<int>> bestAdjacency;

    std::vector<int> vertexWeight(instance.operations.size(), 0);
    for (const OperationInfo& operation : instance.operations) {
        vertexWeight[operation.id] = operation.duration;
    }

    for (const TrialConfig& trial : trials) {
        Schedule schedule = buildSchedule(instance, trial.mode, trial.seed);
        std::vector<std::vector<int>> adjacency = buildScheduleDag(instance, schedule);
        DagAnalysis dag = analyzeDag(adjacency, vertexWeight);

        const bool better = !bestSchedule.has_value()
            || schedule.makespan < bestSchedule->makespan
            || (schedule.makespan == bestSchedule->makespan && dag.longestValue < bestDag.longestValue)
            || (schedule.makespan == bestSchedule->makespan && dag.longestValue == bestDag.longestValue && schedule.priorityMode < bestSchedule->priorityMode);
        if (better) {
            bestSchedule = schedule;
            bestDag = std::move(dag);
            bestAdjacency = std::move(adjacency);
        }
    }

    if (!bestSchedule.has_value()) {
        fail("Nenhuma agenda foi gerada para a instancia " + instance.name);
    }

    SolvedInstance solved;
    solved.instance = instance;
    solved.schedule = *bestSchedule;
    solved.dag = std::move(bestDag);
    solved.adjacency = std::move(bestAdjacency);
    return solved;
}

FixedGraphResult analyzeFixedGraph() {
    FixedGraphResult result;
    result.weights.resize(16, 0);
    for (int vertex = 1; vertex <= 15; ++vertex) {
        result.weights[vertex] = vertex;
    }
    result.adjacency.resize(16);
    auto addEdge = [&](int from, int to) {
        result.adjacency[from].push_back(to);
    };

    addEdge(4, 3);
    addEdge(4, 1);
    addEdge(10, 3);
    addEdge(10, 1);
    addEdge(7, 10);
    addEdge(7, 8);
    addEdge(3, 5);
    addEdge(1, 11);
    addEdge(1, 2);
    addEdge(8, 2);
    addEdge(8, 14);
    addEdge(5, 15);
    addEdge(5, 14);
    addEdge(11, 15);
    addEdge(11, 14);
    addEdge(2, 12);
    addEdge(15, 6);
    addEdge(15, 9);
    addEdge(14, 13);
    addEdge(12, 13);
    addEdge(12, 9);
    addEdge(13, 6);

    std::vector<std::vector<int>> trimmedAdjacency(15);
    std::vector<int> trimmedWeights(15, 0);
    for (int vertex = 1; vertex <= 15; ++vertex) {
        trimmedWeights[vertex - 1] = result.weights[vertex];
        for (int next : result.adjacency[vertex]) {
            trimmedAdjacency[vertex - 1].push_back(next - 1);
        }
    }

    result.dag = analyzeDag(trimmedAdjacency, trimmedWeights);
    result.adjacency = std::move(trimmedAdjacency);
    result.weights = std::move(trimmedWeights);
    return result;
}

void writeFixedGraphReport(const FixedGraphResult& graph, const fs::path& outputPath) {
    std::ofstream out(outputPath);
    if (!out) {
        fail("Nao foi possivel gravar o relatorio do grafo fixo em " + outputPath.string());
    }

    out << "Teste do grafo fixo do enunciado\n";
    out << "================================\n\n";
    out << "Ordem topologica percorrida: ";
    for (std::size_t index = 0; index < graph.dag.topoOrder.size(); ++index) {
        if (index != 0) {
            out << ", ";
        }
        out << (graph.dag.topoOrder[index] + 1);
    }
    out << "\n\n";

    const std::vector<int> criticalPath = restorePath(graph.dag.longestEnd, graph.dag.parent);
    out << "Caminho maximo de um elemento minimal para um maximal: "
        << formatPath(criticalPath, [](int vertex) { return std::to_string(vertex + 1); }) << "\n";
    out << "Comprimento do caminho maximo: " << graph.dag.longestValue << "\n\n";

    const std::vector<int> rowEnds = {6, 13, 9};
    for (int rowEnd : rowEnds) {
        const int target = rowEnd - 1;
        const std::vector<int> path = restorePath(target, graph.dag.parent);
        out << "Caminho maximo ate o final da linha " << rowEnd << ": "
            << formatPath(path, [](int vertex) { return std::to_string(vertex + 1); })
            << " | comprimento = " << graph.dag.longestDistance[target] << "\n";
    }
}

void writeInstanceDetail(const SolvedInstance& solved, const fs::path& outputPath) {
    std::ofstream out(outputPath);
    if (!out) {
        fail("Nao foi possivel gravar o detalhamento da instancia em " + outputPath.string());
    }

    const Instance& instance = solved.instance;
    const Schedule& schedule = solved.schedule;
    const DagAnalysis& dag = solved.dag;
    const std::vector<int> criticalPath = restorePath(dag.longestEnd, dag.parent);

    out << "Instancia: " << instance.name << "\n";
    out << "Jobs: " << instance.jobs << "\n";
    out << "Machines: " << instance.machines << "\n";
    out << "Heuristica selecionada: modo=" << schedule.priorityMode << ", seed=" << schedule.seed << "\n";
    out << "Makespan: " << schedule.makespan << "\n";
    out << "Comprimento do caminho critico: " << dag.longestValue << "\n\n";

    out << "Ordem topologica do DAG de operacoes:\n";
    for (std::size_t index = 0; index < dag.topoOrder.size(); ++index) {
        out << operationLabel(instance, dag.topoOrder[index]);
        if (index + 1 != dag.topoOrder.size()) {
            out << " -> ";
        }
    }
    out << "\n\n";

    out << "Caminho critico:\n";
    for (std::size_t index = 0; index < criticalPath.size(); ++index) {
        const int operationId = criticalPath[index];
        out << operationLabel(instance, operationId)
            << " [inicio=" << schedule.startTime[operationId]
            << ", fim=" << schedule.finishTime[operationId] << "]";
        if (index + 1 != criticalPath.size()) {
            out << " -> ";
        }
    }
    out << "\n\n";

    out << "Sequencias por maquina:\n";
    for (int machine = 0; machine < instance.machines; ++machine) {
        out << "Maquina " << (machine + 1) << ": ";
        for (std::size_t index = 0; index < schedule.machineOrder[machine].size(); ++index) {
            const int operationId = schedule.machineOrder[machine][index];
            out << operationLabel(instance, operationId)
                << "[" << schedule.startTime[operationId] << ',' << schedule.finishTime[operationId] << ']';
            if (index + 1 != schedule.machineOrder[machine].size()) {
                out << " | ";
            }
        }
        out << "\n";
    }

    out << "\nCronograma por job:\n";
    for (int job = 0; job < instance.jobs; ++job) {
        out << "Job " << (job + 1) << ": ";
        for (int position = 0; position < instance.machines; ++position) {
            const int operationId = instance.jobOperationIds[job][position];
            out << operationLabel(instance, operationId)
                << "[" << schedule.startTime[operationId] << ',' << schedule.finishTime[operationId] << ']';
            if (position + 1 != instance.machines) {
                out << " | ";
            }
        }
        out << "\n";
    }
}

void writeCsvSummary(const std::vector<SolvedInstance>& solvedInstances, const fs::path& outputPath) {
    std::ofstream out(outputPath);
    if (!out) {
        fail("Nao foi possivel gravar o CSV em " + outputPath.string());
    }
    out << "instance,jobs,machines,makespan,critical_path,priority_mode,seed\n";
    for (const SolvedInstance& solved : solvedInstances) {
        out << solved.instance.name << ','
            << solved.instance.jobs << ','
            << solved.instance.machines << ','
            << solved.schedule.makespan << ','
            << solved.dag.longestValue << ','
            << solved.schedule.priorityMode << ','
            << solved.schedule.seed << '\n';
    }
}

void writeMarkdownSummary(const std::vector<SolvedInstance>& solvedInstances, const fs::path& outputPath) {
    std::ofstream out(outputPath);
    if (!out) {
        fail("Nao foi possivel gravar o resumo em Markdown em " + outputPath.string());
    }

    long long sumMakespan = 0;
    for (const SolvedInstance& solved : solvedInstances) {
        sumMakespan += solved.schedule.makespan;
    }
    const double averageMakespan = solvedInstances.empty() ? 0.0 : static_cast<double>(sumMakespan) / static_cast<double>(solvedInstances.size());

    out << "# Relatorio - Job Shop com Makespan\n\n";
    out << "## Modelagem\n\n";
    out << "Cada operacao foi modelada como um vertice ponderado pelo seu tempo de processamento. "
           "As precedencias de job geram arcos conjuntivos e a sequencia escolhida em cada maquina gera arcos de recurso. "
           "O makespan final e obtido pelo caminho maximo no DAG resultante.\n\n";
    out << "## Heuristica\n\n";
    out << "A construcao do cronograma usa a heuristica de Giffler-Thompson para gerar agendas ativas. "
           "O conjunto de conflitos e resolvido com varias regras de prioridade, incluindo LRPT e variantes randomizadas. "
           "A melhor agenda encontrada em cada instancia e a que minimiza o makespan.\n\n";
    out << "## Resumo numérico\n\n";
    out << "- Instancias processadas: " << solvedInstances.size() << "\n";
    out << "- Makespan medio: " << std::fixed << std::setprecision(2) << averageMakespan << "\n\n";
    out << "## Resultados\n\n";
    out << "| Instancia | Jobs | Maquinas | Makespan | Caminho critico | Caminho |\n";
    out << "|---|---:|---:|---:|---:|---|\n";
    for (const SolvedInstance& solved : solvedInstances) {
        const std::vector<int> criticalPath = restorePath(solved.dag.longestEnd, solved.dag.parent);
        out << '|' << solved.instance.name
            << '|' << solved.instance.jobs
            << '|' << solved.instance.machines
            << '|' << solved.schedule.makespan
            << '|' << solved.dag.longestValue
            << "|" << formatPath(criticalPath, [&](int operationId) { return operationLabel(solved.instance, operationId); }) << "|\n";
    }
}

void writeHtmlTable(const std::vector<SolvedInstance>& solvedInstances, const fs::path& outputPath) {
    std::ofstream out(outputPath);
    if (!out) {
        fail("Nao foi possivel gravar o HTML em " + outputPath.string());
    }

    out << "<!DOCTYPE html>\n<html lang=\"pt-BR\">\n<head>\n<meta charset=\"utf-8\">\n";
    out << "<title>Tabela JSP Makespan</title>\n";
    out << "<style>body{font-family:Georgia,serif;margin:32px;color:#1b1b1b;}h1{margin-bottom:8px;}table{border-collapse:collapse;width:100%;font-size:12px;}th,td{border:1px solid #444;padding:6px 8px;text-align:left;vertical-align:top;}th{background:#efefef;}tr:nth-child(even){background:#fafafa;} .num{text-align:right;}</style>\n";
    out << "</head>\n<body>\n<h1>Resultados JSP Makespan</h1>\n";
    out << "<p>Tabela gerada automaticamente pelo solver em C++.</p>\n";
    out << "<table>\n<thead><tr><th>Instancia</th><th>Jobs</th><th>Maquinas</th><th>Makespan</th><th>Caminho critico</th></tr></thead>\n<tbody>\n";
    for (const SolvedInstance& solved : solvedInstances) {
        out << "<tr><td>" << solved.instance.name << "</td>"
            << "<td class=\"num\">" << solved.instance.jobs << "</td>"
            << "<td class=\"num\">" << solved.instance.machines << "</td>"
            << "<td class=\"num\">" << solved.schedule.makespan << "</td>"
            << "<td class=\"num\">" << solved.dag.longestValue << "</td></tr>\n";
    }
    out << "</tbody>\n</table>\n</body>\n</html>\n";
}

std::vector<fs::path> findJspInstances(const fs::path& root) {
    std::vector<fs::path> instances;
    for (const auto& entry : fs::directory_iterator(root)) {
        if (entry.is_regular_file() && entry.path().extension() == ".psi") {
            instances.push_back(entry.path());
        }
    }
    std::sort(instances.begin(), instances.end());
    return instances;
}

int main(int argc, char** argv) {
    try {
        fs::path workspace = fs::current_path();
        fs::path jspDirectory = workspace / "Instancias" / "Instancias" / "JSP";
        fs::path outputDirectory = workspace / "output";
        int randomizedIterations = 0;

        for (int index = 1; index < argc; ++index) {
            const std::string argument = argv[index];
            if (argument == "--jsp-dir" && index + 1 < argc) {
                jspDirectory = fs::path(argv[++index]);
            } else if (argument == "--output-dir" && index + 1 < argc) {
                outputDirectory = fs::path(argv[++index]);
            } else if (argument == "--iterations" && index + 1 < argc) {
                randomizedIterations = std::stoi(argv[++index]);
            } else {
                fail("Argumento invalido: " + argument);
            }
        }

        if (!fs::exists(jspDirectory)) {
            fail("Diretorio de instancias JSP nao encontrado: " + jspDirectory.string());
        }

        fs::create_directories(outputDirectory / "details");

        const FixedGraphResult fixedGraph = analyzeFixedGraph();
        writeFixedGraphReport(fixedGraph, outputDirectory / "fixed_graph_report.txt");

        const std::vector<fs::path> instancePaths = findJspInstances(jspDirectory);
        if (instancePaths.empty()) {
            fail("Nenhuma instancia JSP foi encontrada em " + jspDirectory.string());
        }

        std::vector<SolvedInstance> solvedInstances;
        solvedInstances.reserve(instancePaths.size());
        for (const fs::path& instancePath : instancePaths) {
            const Instance instance = parseInstance(instancePath);
            SolvedInstance solved = solveInstance(instance, randomizedIterations);
            writeInstanceDetail(solved, outputDirectory / "details" / (instance.name + ".txt"));
            solvedInstances.push_back(std::move(solved));
            std::cout << "Processada: " << instancePath.filename().string()
                      << " | makespan=" << solvedInstances.back().schedule.makespan << '\n';
        }

        writeCsvSummary(solvedInstances, outputDirectory / "jsp_makespan_results.csv");
        writeMarkdownSummary(solvedInstances, outputDirectory / "report.md");
        writeHtmlTable(solvedInstances, outputDirectory / "results_table.html");

        std::cout << "\nArquivos gerados em: " << outputDirectory.string() << '\n';
        std::cout << "- fixed_graph_report.txt\n";
        std::cout << "- jsp_makespan_results.csv\n";
        std::cout << "- report.md\n";
        std::cout << "- results_table.html\n";
        std::cout << "- details/*.txt\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Erro: " << error.what() << '\n';
        return 1;
    }
}