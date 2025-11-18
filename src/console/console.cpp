#include "Console.hpp"
#include <future>
#include <iomanip>
#include <sstream>

Console::Console(World *world, ImGuiIO *io) : WorldPointer(world), IOContext(io) {
    Log("[CONSOLE] Initialized console system");
    RegisterDefaultCommands();
}

bool Console::WantsInput() const { return IsActive; }

void Console::Update(float deltaTime) {
    {
        std::lock_guard<std::mutex> lock(Mutex);

        for (auto &entry : LogEntries)
            entry.TimeAlive -= deltaTime;

        LogEntries.erase(
            std::remove_if(LogEntries.begin(), LogEntries.end(), [](const LogEntry &e) { return e.TimeAlive <= 0.f; }),
            LogEntries.end());

        for (auto it = PendingWaits.begin(); it != PendingWaits.end();) {
            it->RemainingTime -= deltaTime;
            if (it->RemainingTime <= 0.0f) {
                QueuedCommands.push_back(it->NextCommand);
                it = PendingWaits.erase(it);
            } else {
                ++it;
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Slash))
            IsActive = !IsActive;

        if (IsActive) {
            ImGuiIO &io = *IOContext;
            for (int n = 0; n < io.InputQueueCharacters.Size; n++) {
                ImWchar c = io.InputQueueCharacters[n];
                if (c >= 32 && c != 127)
                    InputBuffer.push_back((char)c);
            }
            io.InputQueueCharacters.resize(0);

            if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && !InputBuffer.empty())
                InputBuffer.pop_back();
            else if (InputBuffer.empty() && ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
                IsActive = false;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                QueuedCommands.push_back(InputBuffer);
                InputBuffer.clear();
                IsActive = false;
            }
        }
    }
}

void Console::ExecuteCommands() {
    for (auto &cmd : QueuedCommands)
        Execute(cmd);

    QueuedCommands.clear();
}

void Console::Draw() {
    if (!IsActive)
        return;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 io_size = IOContext->DisplaySize;
    float padding = 5.f, title_h = 20.f, input_h = 20.f;
    float max_w = 0.f;
    for (auto &e : LogEntries)
        max_w = std::max(max_w, ImGui::CalcTextSize(e.Text.c_str()).x);
    max_w = std::max(max_w, ImGui::CalcTextSize(("> " + InputBuffer).c_str()).x);
    float width = std::clamp(max_w + padding * 2, 200.f, io_size.x * 0.8f);
    float height = std::clamp(title_h + input_h + LogEntries.size() * 20.f + padding, 60.f, io_size.y * 0.5f);
    float x = padding, y = io_size.y - height - padding;

    draw_list->AddRectFilled({x, y}, {x + width, y + height}, IM_COL32(40, 40, 40, 220), 0.f);
    draw_list->AddRect({x, y}, {x + width, y + height}, IM_COL32(235, 203, 139, 200), 0.f, 0, 1.5f);
    draw_list->AddRectFilled({x, y}, {x + width, y + title_h}, IM_COL32(60, 60, 60, 255), 0.f, 0);
    draw_list->AddLine({x, y + title_h}, {x + width, y + title_h}, IM_COL32(235, 203, 139, 200));
    draw_list->AddText({x + 5, y + 3}, IM_COL32(235, 203, 139, 255), "console");

    std::string prompt = "> " + InputBuffer + ((int)(ImGui::GetTime() * 2) % 2 ? "_" : "");
    draw_list->AddText({x + 5, y + height - input_h}, IM_COL32(142, 192, 124, 255), prompt.c_str());

    float log_y = y + title_h + 5.f;
    for (auto it = LogEntries.rbegin(); it != LogEntries.rend(); ++it) {
        draw_list->AddText({x + 5, log_y}, IM_COL32(215, 153, 33, 255), it->Text.c_str());
        log_y += 20.f;
        if (log_y > y + height - input_h - padding)
            break;
    }
}

void Console::Execute(const std::string &commandLine) {
    if (commandLine.empty())
        return;

    Log("> " + commandLine);

    auto tokens = SplitArgs(commandLine);
    if (tokens.empty())
        return;

    auto cmdName = tokens.front();
    tokens.erase(tokens.begin());

    auto it = Commands.find(cmdName);
    if (it != Commands.end()) {
        try {
            it->second.Execute(this, tokens);
        } catch (const std::exception &e) {
            Log(std::string("[ERROR] Exception in command '") + cmdName + "': " + e.what());
        }
    } else {
        Log("[ERROR] Unknown command: " + cmdName);
    }
}

void Console::ExecuteAsync(const std::string &commandLine) {
    std::thread([this, commandLine]() { Execute(commandLine); }).detach();
}

void Console::ExecuteScript(const std::string &script) {
    std::istringstream stream(script);
    std::string line;

    while (std::getline(stream, line)) {
        if (!line.empty())
            Execute(line);
    }
}

void Console::ExecuteFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Log("[ERROR] Could not open script file: " + filename);
        return;
    }

    std::ostringstream script;
    script << file.rdbuf();
    ExecuteScript(script.str());
}

void Console::ParseFunction(const std::string &commandLine) {
    std::string name;
    std::vector<std::string> params;
    std::string body;

    std::regex func_regex(R"(function\s+(\w+)\((.*?)\)\s+do\s*(.*?)\s*end)");
    std::smatch match;
    if (std::regex_search(commandLine, match, func_regex)) {
        name = match[1];
        std::string param_str = match[2];
        body = match[3];

        std::stringstream ss(param_str);
        std::string param;
        while (std::getline(ss, param, ',')) {
            param.erase(std::remove_if(param.begin(), param.end(), ::isspace), param.end());
            if (!param.empty())
                params.push_back(param);
        }

        Functions[name] = {params, body};
        Log("Defined function: " + name + " with " + std::to_string(params.size()) + " parameters");
    } else {
        Log("Invalid function syntax!");
    }
}

void Console::CallFunction(const std::string &commandLine) {
    auto args = SplitArgs(commandLine);
    if (args.size() < 2) {
        Log("Usage: call <function_name> [args...]");
        return;
    }

    std::string func_name = args[1];
    auto it = Functions.find(func_name);
    if (it == Functions.end()) {
        Log("Unknown function: " + func_name);
        return;
    }

    const auto &func = it->second;
    std::unordered_map<std::string, std::string> locals;

    for (size_t i = 0; i < func.Params.size() && (i + 2) < args.size(); ++i)
        locals[func.Params[i]] = args[i + 2];

    std::string expanded = func.Body;
    for (auto &[param, value] : locals)
        expanded = std::regex_replace(expanded, std::regex("\\b" + param + "\\b"), value);

    ExecuteScript(expanded);
}

bool Console::EvaluateCondition(const std::string &fullParam, const std::string &op, const std::string &value) {
    auto dot = fullParam.find('.');
    if (dot == std::string::npos)
        return false;

    std::string entity_name = fullParam.substr(0, dot);
    std::string param = fullParam.substr(dot + 1);

    auto entity = FindEntity(entity_name);
    if (!entity)
        return false;

    float left = 0.f, right = std::stof(value);
    auto v = entity->Params().Get(param);
    if (!v)
        return false;

    if (auto f = std::get_if<float>(&*v))
        left = *f;
    else if (auto i = std::get_if<int>(&*v))
        left = (float)(*i);
    else
        return false;

    if (op == ">")
        return left > right;
    if (op == "<")
        return left < right;
    if (op == "==")
        return left == right;
    if (op == "!=")
        return left != right;

    return false;
}

bool Console::EvaluateExpression(const std::string &entityName, const std::string &expression) {
    std::istringstream ss(expression);
    std::vector<std::string> tokens;
    std::string t;
    while (ss >> t)
        tokens.push_back(t);

    std::function<bool(size_t &, bool)> parse_expr = [&](size_t &idx, bool in_and) -> bool {
        bool result = true;
        while (idx < tokens.size()) {
            t = tokens[idx];

            if (t == "and") {
                idx++;
                result = result && parse_expr(idx, true);
            } else if (t == "or") {
                idx++;
                result = result || parse_expr(idx, false);
            } else if (t == "not") {
                idx++;
                result = !parse_expr(idx, true);
            } else {
                if (idx + 2 >= tokens.size())
                    return false;
                std::string param = t;
                std::string op = tokens[idx + 1];
                std::string val = tokens[idx + 2];
                idx += 3;
                result = EvaluateCondition(param, op, val);
            }

            if (!in_and)
                break;
        }
        return result;
    };

    size_t i = 0;
    return parse_expr(i, true);
}

float Console::EvaluateNumericExpression(const std::string &expression, BaseEntity *entity) {
    std::istringstream ss(expression);
    std::vector<std::string> tokens;
    std::string t;
    while (ss >> t)
        tokens.push_back(t);

    float result = 0.f;
    std::string op = "+";

    for (size_t idx = 0; idx < tokens.size(); ++idx) {
        t = tokens[idx];
        float val = 0.f;

        try {
            val = std::stof(t); // numeric literal
        } catch (...) {
            // entity.param
            auto dot = t.find('.');
            if (dot != std::string::npos) {
                std::string entity_name = t.substr(0, dot);
                std::string param_name = t.substr(dot + 1);
                auto e = FindEntity(entity_name);
                if (e) {
                    auto v = e->Params().Get(param_name);
                    if (v) {
                        if (auto f = std::get_if<float>(&*v))
                            val = *f;
                        else if (auto i = std::get_if<int>(&*v))
                            val = (float)(*i);
                    }
                }
            } else if (entity) {
                auto v = entity->Params().Get(t);
                if (v) {
                    if (auto f = std::get_if<float>(&*v))
                        val = *f;
                    else if (auto i = std::get_if<int>(&*v))
                        val = (float)(*i);
                }
            }
        }

        if (op == "+")
            result += val;
        else if (op == "-")
            result -= val;
        else if (op == "*")
            result *= val;
        else if (op == "/")
            result /= val;

        if (idx + 1 < tokens.size()) {
            std::string next = tokens[idx + 1];
            if (next == "+" || next == "-" || next == "*" || next == "/") {
                op = next;
                idx++;
            }
        }
    }

    return result;
}

std::shared_ptr<BaseEntity> Console::FindEntity(const std::string &name) {
    if (!WorldPointer)
        return nullptr;
    return WorldPointer->GetEntity(name);
}

void Console::Log(const std::string &message) {
    std::lock_guard<std::mutex> lock(Mutex);
    LogEntries.push_back({message, LogDuration});
    std::cout << message << std::endl;
}

std::vector<std::string> Console::SplitArgs(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty())
            tokens.push_back(item);
    }
    return tokens;
}

void Console::RegisterDefaultCommands() {
    Commands["/echo"] = {"echo", "Prints text to the console",
                         [](Console *console, const std::vector<std::string> &args) {
                             if (args.empty()) {
                                 console->Log("[USAGE] echo <message>");
                                 return;
                             }
                             std::ostringstream out;
                             for (const auto &s : args)
                                 out << s << " ";
                             console->Log(out.str());
                         }};

    Commands["/help"] = {"help", "Displays list of commands", [](Console *console, const std::vector<std::string> &) {
                             console->Log("Available commands:");
                             for (const auto &[name, cmd] : console->Commands)
                                 console->Log(" - " + name + ": " + cmd.Description);
                         }};

    Commands["/clear"] = {"clear", "Clears console log", [](Console *console, const std::vector<std::string> &) {
                              console->LogEntries.clear();
                              console->Log("[CONSOLE] Log cleared");
                          }};

    Commands["/list"] = {"list", "Lists all entities", [](Console *self, const std::vector<std::string> &) {
                             size_t idx = 0;
                             for (auto &e : self->WorldPointer->GetEntities()) {
                                 if (e)
                                     self->Log(std::to_string(idx++) + ": " + e->GetName());
                             }
                         }};
}