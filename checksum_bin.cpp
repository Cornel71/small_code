#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdio>
#include <memory>
#include <algorithm>

namespace fs = std::filesystem;

struct PipeDeleter {
    void operator()(FILE* fp) const {
        if (fp) pclose(fp);
    }
};

std::string get_sha256(const std::string& filepath) {
    char buffer[128];
    std::string result = "";
    std::string command = "sha256sum \"" + filepath + "\" 2>/dev/null | awk '{print $1}'";
    std::unique_ptr<FILE, PipeDeleter> pipe(popen(command.c_str(), "r"));
    if (!pipe) return "ERROR";

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result.empty() ? "ERROR" : result;
}

int main() {
    std::vector<std::string> search_paths = {"/bin", "/usr/bin"};
    std::ofstream sql_file("binaries_import.sql");

    if (!sql_file.is_open()) {
        std::cerr << "Failed to create SQL output file." << std::endl;
        return 1;
    }
    sql_file << "-- Generated on 2026-01-13\n"
             << "CREATE TABLE IF NOT EXISTS system_binaries (\n"
             << "    id INT AUTO_INCREMENT PRIMARY KEY,\n"
             << "    file_path TEXT NOT NULL,\n"
             << "    sha256_hash CHAR(64) NOT NULL,\n"
             << "    last_scanned DATETIME DEFAULT CURRENT_TIMESTAMP\n"
             << ");\n\n";

    std::cout << "Scanning system binaries..." << std::endl;

    for (const auto& path : search_paths) {
        if (!fs::exists(path)) continue;

        for (const auto& entry : fs::directory_iterator(path)) {
            try {
                if (fs::is_regular_file(entry)) {
                    std::string filepath = entry.path().string();
                    std::string hash = get_sha256(filepath);

                    if (hash != "ERROR") {
                        std::string safe_path = filepath;
                        size_t pos = 0;
                        while ((pos = safe_path.find("'", pos)) != std::string::npos) {
                            safe_path.replace(pos, 1, "''");
                            pos += 2;
                        }

                        sql_file << "INSERT INTO system_binaries (file_path, sha256_hash) "
                                 << "VALUES ('" << safe_path << "', '" << hash << "');\n";
                    }
                }
            } catch (const fs::filesystem_error& e) {
                continue;
            }
        }
    }

    sql_file.close();
    std::cout << "Done! Import 'binaries_import.sql' into your SQL database." << std::endl;

    return 0;
}
