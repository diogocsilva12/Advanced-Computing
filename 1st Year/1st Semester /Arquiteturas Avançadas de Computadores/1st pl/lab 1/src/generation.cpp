#include "generation.h"
#include <vector>
#include <random>


// Generates a realistic followers matrix automatically.
// - Few super-celebs with massive followers
// - Most users follow a random number of others
// - Preferential attachment for rich-get-richer effect
std::vector<std::vector<int>> generate_followers_matrix(int num_users) {
    std::vector<std::vector<int>> matrix(num_users, std::vector<int>(num_users, 0));

    // RNG
    std::random_device rd;
    std::mt19937 gen(rd());

    // Randomize parameters to make each run unique but realistic
    int initial_links       = std::max(3, num_users / 200); // small fully-connected core
    int num_supercelebs     = std::max(1, num_users / 500); // ~0.2% super celebs
    int superceleb_weight   = 1000 + (gen() % 5000);        // huge initial weight
    int avg_follows_per_user = std::max(10, num_users / 100); // average follows per user

    std::exponential_distribution<> follow_dist(1.0 / avg_follows_per_user);

    // Track degree for preferential attachment
    std::vector<int> degree(num_users, 1);
    int total_degree = num_users;

    // Give super-celebs huge initial weight
    for (int i = 0; i < num_supercelebs && i < num_users; ++i) {
        degree[i] += superceleb_weight;
        total_degree += superceleb_weight;
    }

    // Seed a small fully-connected core
    for (int u = 0; u < initial_links; ++u) {
        for (int v = 0; v < initial_links; ++v) {
            if (u != v) {
                matrix[u][v] = 1;
                degree[v]++;
                total_degree++;
            }
        }
    }

    // Add new users
    for (int u = initial_links; u < num_users; ++u) {
        int follows_to_make = std::max(1, (int)follow_dist(gen));
        if (follows_to_make > u) follows_to_make = u;

        while (follows_to_make > 0) {
            for (int v = 0; v < u && follows_to_make > 0; ++v) {
                double prob = (double)degree[v] / total_degree;
                if (std::generate_canonical<double, 10>(gen) < prob) {
                    if (matrix[u][v] == 0) {
                        matrix[u][v] = 1;
                        degree[v]++;
                        total_degree++;
                        follows_to_make--;
                    }
                }
            }
        }
    }

    return matrix;
}





std::vector<std::vector<double>> generate_user_features(int num_users, int feature_dim) {
    std::vector<std::vector<double>> features(num_users, std::vector<double>(feature_dim));
    std::mt19937 gen(123);
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for (int u = 0; u < num_users; u++) {
        for (int f = 0; f < feature_dim; f++) {
            features[u][f] = dist(gen);
        }
    }
    return features;
}
