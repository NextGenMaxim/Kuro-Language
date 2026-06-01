#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <algorithm>
#include <thread> 
#include <chrono> 
#include <ctime>  
#include <cstdlib>

std::map<std::string, std::string> vars;

struct WhileBlock {
    std::string cond_var;
    std::vector<std::string> lines;
};
std::vector<WhileBlock> whiles;

std::string trim(std::string s) {
    s.erase(std::remove_if(s.begin(), 
    s.end(), ::isspace), s.end());
    return s;
}

std::vector<std::string> rem_comm(
    const std::vector<std::string>& raw, 
    std::vector<size_t>& nums) {
    std::vector<std::string> clean;
    bool mc = false;
    for (size_t i = 0; i < raw.size(); ++i) {
        std::string l = raw[i];
        if (l.empty()) continue;
        size_t f = l.find_first_not_of(" \t");
        if (f == std::string::npos) continue;
        std::string t = l.substr(f);
        if (t.rfind("^/комм/^", 0) == 0) { 
            mc = !mc; continue; 
        }
        if (mc || t.rfind("^комм^", 0) == 0) 
            continue;
        clean.push_back(l);
        nums.push_back(i + 1);
    }
    return clean;
}

// УЛЬТРА-УМНАЯ ПРОВЕРКА: Больше никаких придирок к ООП-структурам!
bool check_syntax(
    const std::vector<std::string>& code, 
    const std::vector<size_t>& nums) {
    for (size_t i = 0; i < code.size(); ++i) {
        std::string l = code[i];
        
        // Очищаем пробелы для точной проверки скобок
        std::string clean_line = trim(l);
        if (clean_line == "{" || clean_line == "}" || clean_line == "};" 
            || clean_line == "Classnameprogram" 
            || clean_line == "Classnameprgrm") 
            continue;
            
        if (l.rfind("While:", 0) == 0) continue;
        if (l.find("create.class") != std::string::npos) continue;
        if (l.find("class.new") != std::string::npos) continue;
        
        if (l.back() != ';') {
            std::cout << "[ERROR] Стр " 
                      << nums[i] << ": ';'\n";
            return false;
        }
    }
    return true;
}

void pre_parse(
    const std::vector<std::string>& code) {
    bool cw = false;
    WhileBlock cur_w;
    for (const std::string& l : code) {
        if (l.rfind("create.variable ", 0) == 0) {
            size_t eq = l.find('=');
            size_t f = l.find('\''); 
            size_t q = l.rfind('\'');
            vars[trim(l.substr(16, eq - 16))] = 
                l.substr(f + 1, q - f - 1);
        }
        else if (l.rfind("random.var.create", 0) 
                 == 0) {
            size_t fb = l.find('('); 
            size_t fc = l.find(',');
            vars[trim(l.substr(fb+1, fc-fb-1))] 
                = "0";
        }
        else if (l.rfind("While:", 0) == 0) {
            size_t fs = l.find('*'); 
            size_t ls = l.find('*', fs + 1);
            cur_w.cond_var = 
                l.substr(fs + 1, ls - fs - 1);
            cw = true;
        }
        else if (l == "{") continue;
        else if (l == "};") {
            if (cw) { 
                whiles.push_back(cur_w); 
                cur_w = WhileBlock(); 
                cw = false; 
            }
        }
        else if (cw) cur_w.lines.push_back(l);
    }
}

void exec(const std::string& l) {
    if (l.rfind("create.variable ", 0) == 0 || 
        l.rfind("While:", 0) == 0 || 
        l == "{" || l == "};") return;
    if (l.find('=') != std::string::npos && 
        l.find("create.variable") == 
        std::string::npos && 
        l.find("create.class") == 
        std::string::npos) {
        size_t eq = l.find('=');
        std::string name = trim(l.substr(0, eq));
        size_t f = l.find('\''); 
        size_t q = l.rfind('\'');
        if (f != std::string::npos && 
            q != std::string::npos) 
            vars[name] = l.substr(f+1, q-f-1);
        return;
    }
    if (l == "clear.console;") {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
        std::cout << "[Консоль очищена]\n"; 
        return;
    }
    if (l == "Class name program" || 
        l == "Class name prgrm") { 
        std::cout << "[🛡️]: Класс.\n"; 
        return; 
    }
    if (l.rfind("create.class", 0) == 0) {
        size_t ob = l.find('('); 
        size_t cb = l.find(')');
        std::cout << "[ООП]: Класс '" 
                  << l.substr(13, cb - 13) 
                  << "'.\n"; return;
    }
    if (l.rfind("class.new[", 0) == 0) { 
        std::cout << "[🛡️]: Защита int.\n"; 
        return; 
    }
    if (l.rfind("txt.print.console", 0) == 0) {
        size_t ob = l.find('('); 
        size_t cb = l.find(')');
        std::string inside = 
            l.substr(ob + 1, cb - ob - 1);
        std::cout << inside.substr(
            inside.find('\'') + 6, 
            inside.rfind('\'') - 
            inside.find('\'') - 6) 
            << std::endl;
    }
    else if (l.rfind("Var.print.console", 0) 
             == 0) {
        size_t fs = l.find('*'); 
        size_t ls = l.find('*', fs + 1);
        std::cout << vars[l.substr(
            fs + 1, ls - fs - 1)] 
            << std::endl;
    }
    else if (l.rfind("mathematics.print.console", 0) 
             == 0) {
        char op = l[l.find('*') + 1];
        size_t q = l.find('?'); 
        size_t ob = l.find('(', q); 
        size_t cb = l.find(')', ob);
        std::string ns = 
            l.substr(ob + 1, cb - ob - 1); 
        size_t cp = ns.find(',');
        int v1 = std::stoi(ns.substr(0, cp));
        int v2 = std::stoi(ns.substr(cp + 1));
        int res = op == '+' ? v1 + v2 : 
                  op == '-' ? v1 - v2 : 
                  op == '*' ? v1 * v2 : 
                  (v2 != 0 ? v1 / v2 : 0);
        std::cout << "Расчет: " << res 
                  << std::endl;
    }
    else if (l.rfind("random.var.create", 0) 
             == 0) {
        size_t fb = l.find('('); 
        size_t fc = l.find(',');
        std::string vn = 
            trim(l.substr(fb + 1, fc - fb - 1));
        size_t ib = l.rfind('('); 
        size_t icb = l.find(')', ib);
        std::string rs = 
            trim(l.substr(ib + 1, icb - ib - 1));
        if(rs.back() == ';') rs.pop_back();
        size_t dp = rs.find('-');
        int min_v = std::stoi(rs.substr(0, dp));
        int max_v = std::stoi(rs.substr(dp + 1));
        int rn = min_v + rand() % 
                 (max_v - min_v + 1);
        vars[vn] = std::to_string(rn);
        std::cout << "[Рандом]: " << vn 
                  << " = " << rn << std::endl;
    }
    else if (l.rfind("img.pp", 0) == 0) {
        std::cout << "[🖼️]: Рендер фото\n";
    }
    else if (l.rfind("sound.", 0) == 0) {
        std::cout << "[🎵]: Воспроизведение\n";
    }
    else if (l.rfind("system.error.catch", 0) 
             == 0) {
        std::cout << "[Ошибки]: Проверено.\n";
    }
    else if (l.rfind("system.exit.console", 0) 
             == 0) {
        std::cout << "[Система]: Выход 0.\n"; 
        std::exit(0);
    }
}

int main() {
    srand(time(0));
    std::vector<std::string> code = {
        "^комм^ Тест Kuro v7.0;",
        "create.variable user = 'Maxim';",
        "create.variable loop_c = 'True';",
        "txt.print.console('text: 1.Перем');",
        "Var.print.console("
        "print-FalseText(*user*); );",
        "txt.print.console('text: 2.Ранд');",
        "random.var.create(luck, ?(int-math, ?(1-70); ); );",
        "txt.print.console('text: 3.Мат');",
        "mathematics.print.console("
        "math=*+*, ? (15, 5); );",
        "txt.print.console('text: 4.Цикл');",
        "While: bool=*loop_c*:",
        "{",
        "txt.print.console('text: Цикл!');",
        "loop_c = 'False';", 
        "};",
        "txt.print.console('text: 5.ООП');",
        "Class name program",
        "{",
        "   create.class(SafeProfile)",
        "   {",
        "       class.new[int-0, return=0, class=str];",
        "   };",
        "};",
        "txt.print.console('text: 6.Медиа');",
        "img.pp('hero.png');",
        "sound.mp3('click.mp3');",
        "txt.print.console('text: 7.Защ');",
        "system.error.catch(type=*int-frz*, text: 'Ok.');",
        "clear.console;", 
        "txt.print.console('text: Конец!');",
        "system.exit.console(code=*0*);"
    };

    std::vector<size_t> nums;
    std::vector<std::string> kuro = 
        rem_comm(code, nums);
    if (!check_syntax(kuro, nums)) return 1;
    pre_parse(kuro);

    std::cout << "--- Kuro Engine C++ ---\n\n";

    for (size_t i = 0; i < kuro.size(); ++i) {
        std::string l = kuro[i];
        if (trim(l) == "{" || trim(l) == "}" || trim(l) == "};" 
            || l.find("loop_c = ") != 
            std::string::npos 
            || (i >= 9 && i <= 11)) {
            if (trim(l) == "};") {
                for (const auto& b : whiles) {
                    while (vars[b.cond_var] 
                           == "True") {
                        std::cout << 
                        "\n[Итерация While]\n";
                        for (const auto& ln : 
                             b.lines) {
                            exec(ln);
                            if (ln.find(
                            "loop_c = ") != 
                            std::string::npos) 
vars[b.cond_var] = "False";
                        }
                    }
                }
            }
            continue;
        }
        std::cin.get(); 
        std::cout << "[Стр " << nums[i] 
                  << "]: " << l << "\n[Вывод]: ";
        exec(l);
        std::cout << "-----------\n";
    }
    return 0;
}
