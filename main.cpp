#include "crow.h"
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using json = nlohmann::json;

static const fs::path ROOT = CHERRIES_ROOT;
static const fs::path PUBLIC_DIR = ROOT / "public";
static const fs::path DATA_FILE = ROOT / "data" / "games.json";

std::string read_file(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return {};
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

json load_catalog() {
    std::ifstream input(DATA_FILE);
    if (!input) return json::array();

    json data;
    try {
        input >> data;
        if (!data.is_array()) return json::array();
    } catch (const std::exception& e) {
        std::cerr << "Catalog parse error: " << e.what() << '\n';
        return json::array();
    }
    return data;
}

bool ends_with(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string mime_type(const std::string& path) {
    if (ends_with(path, ".html")) return "text/html; charset=utf-8";
    if (ends_with(path, ".css")) return "text/css; charset=utf-8";
    if (ends_with(path, ".js")) return "application/javascript; charset=utf-8";
    if (ends_with(path, ".json")) return "application/json; charset=utf-8";
    if (ends_with(path, ".svg")) return "image/svg+xml";
    if (ends_with(path, ".png")) return "image/png";
    if (ends_with(path, ".jpg") || ends_with(path, ".jpeg")) return "image/jpeg";
    return "application/octet-stream";
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/health")([] {
        return crow::response(200, R"({"status":"ok","service":"cherries"})");
    });

    CROW_ROUTE(app, "/api/games").methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        json catalog = load_catalog();
        const char* category = req.url_params.get("category");
        const char* query = req.url_params.get("q");

        json result = json::array();
        for (const auto& game : catalog) {
            if (!game.is_object()) continue;

            if (category && std::string(category) != "All") {
                if (!game.contains("category") || game["category"] != category) continue;
            }

            if (query && *query) {
                std::string needle(query);
                std::string haystack = game.value("title", "") + " " +
                                       game.value("description", "") + " " +
                                       game.value("category", "");
                auto lower = [](std::string value) {
                    for (char& c : value) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    return value;
                };
                if (lower(haystack).find(lower(needle)) == std::string::npos) continue;
            }
            result.push_back(game);
        }

        crow::response response(result.dump());
        response.set_header("Content-Type", "application/json; charset=utf-8");
        response.set_header("Cache-Control", "no-store");
        return response;
    });

    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)([] {
        crow::response response(read_file(PUBLIC_DIR / "index.html"));
        response.set_header("Content-Type", "text/html; charset=utf-8");
        return response;
    });

    CROW_ROUTE(app, "/<string>").methods(crow::HTTPMethod::GET)
    ([](const std::string& filename) {
        // Only serve known frontend assets; never accept arbitrary filesystem paths.
        if (filename != "styles.css" && filename != "app.js" && filename != "favicon.svg") {
            return crow::response(404, "Not found");
        }
        std::string body = read_file(PUBLIC_DIR / filename);
        if (body.empty()) return crow::response(404, "Not found");
        crow::response response(body);
        response.set_header("Content-Type", mime_type(filename));
        return response;
    });

    const char* env_port = std::getenv("PORT");
    const uint16_t port = env_port ? static_cast<uint16_t>(std::stoi(env_port)) : 8080;
    std::cout << "Cherries is running at http://localhost:" << port << '\n';
    app.bindaddr("127.0.0.1").port(port).multithreaded().run();
}
