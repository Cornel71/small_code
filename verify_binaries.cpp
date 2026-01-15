#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

namespace fs = std::filesystem;

std::string calculate_current_hash(const std::string& filepath) {
    char buffer[128];
    std::string result = "";
    std::string command = "sha256sum \"" + filepath + "\" 2>/dev/null | awk '{print $1}'";
    std::unique_ptr<FILE, void(*)(FILE*)> pipe(popen(command.c_str(), "r"), [](FILE* fp) { if (fp) pclose(fp); });
    if (!pipe) return "ERROR";
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) result += buffer;
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result.empty() ? "ERROR" : result;
}

int main() {
    const std::string db_host = "tcp://127.0.0.1:3306";
    const std::string db_user = "audit_user";
    const std::string db_pass = "secure_password_2026";
    const std::string db_name = "security_audit";

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> con(driver->connect(db_host, db_user, db_pass));
        con->setSchema(db_name);
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT sha256_hash FROM system_binaries WHERE file_path = ?"
        ));

        std::cout << "--- 2026 Binary Integrity Check ---" << std::endl;
        std::vector<std::string> paths_to_check = {"/usr/bin/ls", "/usr/bin/grep", "/bin/bash"};

        for (const auto& bin_path : paths_to_check) {
            if (!fs::exists(bin_path)) {
                std::cout << "[MISSING] " << bin_path << std::endl;
                continue;
            }
            std::string current_hash = calculate_current_hash(bin_path);
            pstmt->setString(1, bin_path);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            if (res->next()) {
                std::string stored_hash = res->getString("sha256_hash");
                if (current_hash == stored_hash) {
                    std::cout << "[OK] " << bin_path << " matches database record." << std::endl;
                } else {
                    std::cerr << "[CRITICAL] " << bin_path << " HASH MISMATCH!" << std::endl;
                    std::cerr << "  Stored:  " << stored_hash << std::endl;
                    std::cerr << "  Current: " << current_hash << std::endl;
                }
            } else {
                std::cout << "[NEW] " << bin_path << " has no record in database." << std::endl;
            }
        }

    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
