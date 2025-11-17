#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "../world/world.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

struct LogEntry {
    std::string Text;
    float TimeAlive; // seconds
};

struct ConsoleCommand {
    std::string Name;
    std::string Description;
    std::function<void(class Console*, const std::vector<std::string>& args)> Execute;
};

struct RunningScript {
    std::string ScriptText;
    int RemainingLoops; // optional for while
};

struct ScriptFunction {
    std::vector<std::string> Params;
    std::string Body; // everything inside the "do ... end"
};

struct PendingWait {
    float RemainingTime;     // seconds left to wait
    std::string NextCommand; // command to execute after wait
};

class Console {
public:
    Console(World* world, ImGuiIO* contextio);

    bool WantsInput() const;
    void Update(float deltaTime);
    void Draw();
    void Execute(const std::string& commandLine); // public for scripts

    bool EvaluateCondition(const std::string& fullParam, const std::string& op, const std::string& value);
    bool EvaluateExpression(const std::string& entityName, const std::string& expression);
    float EvaluateNumericExpression(const std::string& expression, BaseEntity* entity = nullptr);
    void ExecuteScript(const std::string& script);
    void ExecuteFile(const std::string& filename);
    void ExecuteAsync(const std::string& commandLine);
    void ParseFunction(const std::string& commandLine);
    void CallFunction(const std::string& commandLine);

    void ExecuteCommands();

    void Log(const std::string& message);
    std::shared_ptr<BaseEntity> FindEntity(const std::string& name);

    void RegisterDefaultCommands();
    std::vector<std::string> SplitArgs(const std::string& str, char delimiter = ' ');

private:
    World* WorldPointer;
    std::string InputBuffer;
    std::vector<LogEntry> LogEntries;
    bool IsActive = false;
    float LogDuration = 5.0f;
    std::mutex Mutex;
    ImGuiIO* IOContext;

    std::unordered_map<std::string, ConsoleCommand> Commands;
    std::vector<std::string> QueuedCommands;
    std::vector<RunningScript> RunningScripts;
    std::unordered_map<std::string, ScriptFunction> Functions;
    std::vector<PendingWait> PendingWaits;
};

#endif /* CONSOLE_HPP */
