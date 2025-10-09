#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <chrono>
#include <utility>

#include "generation.h" // deve fornecer: generate_followers_matrix(int), generate_user_features(int,int)

// ----------------------
// Utilitários
// ----------------------

// Soma colunas (in-degree): quantos seguem cada utilizador (seguidores)
std::vector<int> compute_in_degree(const std::vector<std::vector<int>> &A) {
    const int n = (int)A.size();
    std::vector<int> colsum(n, 0);
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c)
            if (A[r][c] == 1) colsum[c]++;
    return colsum;
}

// Imprime top/bottom com base no número de seguidores
void print_top_and_bottom_users(const std::vector<std::vector<int>> &followers_matrix,
                                const std::vector<std::vector<double>> &aggregated_features) {
    auto follower_counts = compute_in_degree(followers_matrix);

    std::vector<std::pair<int,int>> user_counts;
    user_counts.reserve(follower_counts.size());
    for (int u = 0; u < (int)follower_counts.size(); ++u)
        user_counts.emplace_back(u, follower_counts[u]);

    std::sort(user_counts.begin(), user_counts.end(),
              [](const auto &a, const auto &b){ return a.second > b.second; });

    const int n = (int)user_counts.size();

    std::cout << "\n=== Top 5 Users com MAIS seguidores (in-degree) ===\n";
    for (int i = 0; i < std::min(5, n); i++) {
        int u = user_counts[i].first;
        std::cout << "User " << u << " (seguidores: " << user_counts[i].second << "): ";
        for (double val : aggregated_features[u]) std::cout << val << " ";
        std::cout << "\n";
    }

    std::cout << "\n=== Bottom 5 Users com MENOS seguidores (in-degree) ===\n";
    for (int i = 0; i < std::min(5, n); i++) {
        int u = user_counts[n - 1 - i].first;
        std::cout << "User " << u << " (seguidores: " << user_counts[n - 1 - i].second << "): ";
        for (double val : aggregated_features[u]) std::cout << val << " ";
        std::cout << "\n";
    }
}

// ----------------------
// Task + Worker
// ----------------------
struct Task {
    int user_id; // >=0 trabalho válido; <0 = poison pill
};

void worker_function(
    std::queue<Task> &tasks,
    std::mutex &mtx,
    std::condition_variable &cv,
    const std::vector<std::vector<int>> &followers_matrix, // A
    const std::vector<std::vector<double>> &user_features, // B
    std::vector<std::vector<double>> &aggregated_features  // C
) {
    const int num_users   = (int)followers_matrix.size();
    const int feature_dim = (int)user_features[0].size();

    while (true) {
        Task task;
        {   // obter tarefa da fila
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return !tasks.empty(); });
            if (task.user_id < 0) break;
            task = tasks.front();
            tasks.pop();
        }

        // Poison pill: terminar worker
        //if (task.user_id < 0) break;

        const int user_id = task.user_id;

        std::vector<double> feature_sum(feature_dim, 0.0);
        double total_follows = 0.0;

        // A[user_id][v] == 1 significa: user_id SEGUE v (row * B)
        for (int v = 0; v < num_users; ++v) {
            if (followers_matrix[user_id][v] == 1) {
                for (int f = 0; f < feature_dim; ++f)
                    feature_sum[f] += user_features[v][f];
                total_follows += 1.0;
            }
        }

        if (total_follows > 0.0) {
            for (int f = 0; f < feature_dim; ++f)
                aggregated_features[user_id][f] = feature_sum[f] / total_follows;
        } else {
            std::fill(aggregated_features[user_id].begin(),
                      aggregated_features[user_id].end(), 0.0);
        }
    }
}

// ----------------------
// Master (spawn, enqueue, shutdown)
// ----------------------
void master_function(
    int num_users,
    int num_worker_threads,
    std::queue<Task> &tasks,
    std::mutex &mtx,
    std::condition_variable &cv,
    const std::vector<std::vector<int>> &followers_matrix,
    const std::vector<std::vector<double>> &user_features,
    std::vector<std::vector<double>> &aggregated_features,
    std::vector<std::thread> &workers)
{
    // Lançar workers
    workers.reserve(num_worker_threads);
    for (int t = 0; t < num_worker_threads; ++t) {
        workers.emplace_back(worker_function,
            std::ref(tasks), std::ref(mtx), std::ref(cv),
            std::cref(followers_matrix), std::cref(user_features),
            std::ref(aggregated_features));
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        for (int u = 0; u < num_users; ++u) tasks.push({u});
        // Enviar poison pills para todos os workers
        for (int t = 0; t < num_worker_threads; ++t) tasks.push({-1});
    }
    cv.notify_all();
}

// ----------------------
// Main
// ----------------------
int main(int argc, char* argv[]) {
    // Defaults
    int num_users = 10000;            // número de utilizadores (n)
    int feature_dim = 3;           // dimensão das features (d)
    int num_worker_threads = 50;    // nº de threads

    // Args: <num_users> <num_worker_threads> <feature_dim>
    if (argc >= 2) num_users = std::stoi(argv[1]);
    if (argc >= 3) num_worker_threads = std::stoi(argv[2]);
    if (argc >= 4) feature_dim = std::stoi(argv[3]);

    std::cout << "Running with " << num_users << " users, "
              << num_worker_threads << " worker threads, feature_dim=" << feature_dim << "\n";

    // Dados
    auto followers_matrix = generate_followers_matrix(num_users);       // n x n (0/1)
    auto user_features    = generate_user_features(num_users, feature_dim); // n x d
    std::vector<std::vector<double>> aggregated_features(
        num_users, std::vector<double>(feature_dim, 0.0));

    std::cout << "Finished matrix generation\n";

    // Infra de tasks
    std::queue<Task> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::thread> workers;

    // Timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Master–worker
    master_function(num_users, num_worker_threads,
                    tasks, mtx, cv,
                    followers_matrix, user_features,
                    aggregated_features, workers);

    // Esperar pelos workers
    for (auto &th : workers) th.join();

    // Timer stop
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Output
    std::cout << "\nTotal worker computation time: " << elapsed.count() << " seconds\n";
    print_top_and_bottom_users(followers_matrix, aggregated_features);

    return 0;
}
