#include <iostream>
#include <filesystem>
#include <string>
#include <memory>
#include <algorithm>
#include <cstdio>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

namespace fs = std::filesystem;

std::string get_current_hash(const std::string& filepath) {
    char buffer[128];
    std::string result = "";
    std::string command = "sha256sum \"" + filepath + "\" 2>/dev/null | awk '{print $1}'";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) return "ERROR";

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result.empty() ? "ERROR" : result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_binary>" << std::endl;
        return 1;
    }

    std::string target_path = fs::absolute(argv[1]).string();
    if (!fs::exists(target_path)) {
        std::cerr << "Error: File '" << target_path << "' not found." << std::endl;
        return 1;
    }

    const std::string db_host = "tcp://127.0.0.1:3306";
    const std::string db_user = "audit_admin";
    const std::string db_pass = "Secure_Pass_2026!";
    const std::string db_name = "security_db";

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> con(driver->connect(db_host, db_user, db_pass));
        con->setSchema(db_name);

        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT sha256_hash FROM system_binaries WHERE file_path = ?"
        ));
        pstmt->setString(1, target_path);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            std::cout << "[!] No record found in database for: " << target_path << std::endl;
            return 1;
        }

        std::string stored_hash = res->getString("sha256_hash");
        std::string current_hash = get_current_hash(target_path);

        std::cout << "Checking: " << target_path << std::endl;
        if (current_hash == stored_hash) {
            std::cout << "[OK] Integrity Verified. Hashes match." << std::endl;
            std::cout << "Hash: " << current_hash << std::endl;
        } else {
            std::cerr << "[CRITICAL FAILURE] Hash mismatch detected!" << std::endl;
            std::cerr << "  Expected: " << stored_hash << std::endl;
            std::cerr << "  Actual:   " << current_hash << std::endl;
            return 2;
        }

    } catch (sql::SQLException &e) {
        std::cerr << "Database Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
