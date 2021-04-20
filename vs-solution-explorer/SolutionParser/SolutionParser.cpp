#include "SolutionParser.h"
#include "Solution.h"
#include "SolutionProjectType.h"
#include "SolutionProject.h"
#include "Utils/Bom.h"
#include "Utils/StringAlgorithms.h"
#include <list>
#include <functional>
#include <stdexcept>
#include <string>
#include <iostream>
#include <regex>
#include <fmt/format.h>

using namespace fmt::literals;

namespace {
    SolutionProjectType solutionProjectTypeFromUuid(const std::string& uuid)
    {
        if (uuid == "F184B08F-C81C-45F6-A57F-5ABD9991F28F") {
            return SolutionProjectType::VisualBasicNet;
        }
        if (uuid == "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942") {
            return SolutionProjectType::Cpp;
        }

        return SolutionProjectType::Unknown;
    }

    struct ParsingError: public std::runtime_error
    {
        ParsingError(const std::string& message)
            : std::runtime_error{message}
        {}
    };

    struct BadParserState: public std::runtime_error
    {
        BadParserState(const std::string& message)
            : std::runtime_error{message}
        {}
    };
}

class State
{
public:
    using Handler = std::function<bool(const std::string& line)>;

    State(Handler handler)
        : m_handler{handler}
    {}

    void addNextState(const std::shared_ptr<State>& nextState)
    {
        m_nextStates.push_back(nextState);
    }

    void addNextEnd() {
        m_nextEnd = true;
    }

    bool nextEnd() const {
        return m_nextEnd;
    }

    const std::shared_ptr<State>& getNextState(const std::string& line) const
    {
        for (const auto& state: m_nextStates) {
            if (state->m_handler(line)) {
                return state;
            }
        }
        throw BadParserState{"Unexpected line"};
    }

private:
    std::list<std::shared_ptr<State>> m_nextStates;
    Handler m_handler;
    bool m_nextEnd = false;
};

std::shared_ptr<State>& operator>>(std::shared_ptr<State>& state, std::shared_ptr<State>& nextState)
{
    state->addNextState(nextState);
    return nextState;
}

std::unique_ptr<ISolution> SolutionParser::parse(const ISolutionSource& solutionSource)
{
    auto solution = std::make_unique<Solution>();

    auto start = std::make_shared<State>(
        [](const std::string&)->bool { return true; }
    );

    const std::string header = "Microsoft Visual Studio Solution File, Format Version ";
    auto headerWithFormatVersion = std::make_shared<State>(
        [&](const std::string& line) {
            if (!startWith(line, header)) {
                return false;
            }
            auto it = line.begin();
            std::advance(it, header.size());
            solution->m_formatVersion = std::string{it, line.cend()};
            return true;
       }
    );

    auto vsMainVersion = std::make_shared<State>(
        [](const std::string& line) {
            if (startWith(line, "# Visual Studio Version ")) {
                return true;
            }
            return false;
        }
    );

    auto fullVsVersion = std::make_shared<State>(
        [&](const std::string& line) {
            const auto parts = split(line, "=");
            if (parts.size() == 2 && trim(parts[0]) == "VisualStudioVersion") {
                const auto version = trim(parts[1]);
                if (!version.empty()) {
                    solution->m_visualStudioVersion = version;
                    return true;
                }
            }
            return false;
        }
    );

    auto minimumVsVersion = std::make_shared<State>(
        [&](const std::string& line) {
            const auto parts = split(line, "=");
            if (parts.size() == 2 && trim(parts[0]) == "MinimumVisualStudioVersion") {
                const auto version = trim(parts[1]);
                if (!version.empty()) {
                    solution->m_minimumVisualStudioVersion = version;
                    return true;
                }
            }
            return false;
        }
    );

    std::shared_ptr<SolutionProject> currentSolutionProject;

    const std::string uuidPattern = "\\{([A-F0-9]{8}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{12})\\}";

    auto startProject = std::make_shared<State>(
        [&](const std::string& line) {
            try {
                const std::string quotedUuidPattern = fmt::format("\"{}\"", uuidPattern);
                const std::regex pattern{fmt::format("Project\\({uuid}\\) = \"(.+)\", \"(.+)\", {uuid}",
                                                     "uuid"_a=quotedUuidPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+4) {
                    throw ::ParsingError{"Cannot parse a Project line"};
                }

                currentSolutionProject = std::make_shared<SolutionProject>();

                currentSolutionProject->m_type = solutionProjectTypeFromUuid(match[1].str());
                currentSolutionProject->m_name = match[2].str();
                currentSolutionProject->m_path = match[3].str();
                currentSolutionProject->m_uuid = match[4].str();
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }
            return true;
        }
    );

    auto projectDependenciesSection = std::make_shared<State>(
        [](const std::string& line) {
            if (line == "\tProjectSection(ProjectDependencies) = postProject") {
                return true;
            }
            return false;
        }
    );

    auto endProjectDependenciesSection = std::make_shared<State>(
        [](const std::string& line) {
            if (line == "\tEndProjectSection") {
                return true;
            }
            return false;
        }
    );

    auto projectDependency = std::make_shared<State>(
        [&](const std::string& line) {
            try {
                const std::regex pattern{fmt::format("\t\t{uuid} = {uuid}",
                                                     "uuid"_a=uuidPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+2) {
                    throw ::ParsingError{"Cannot parse a project dependency"};
                }

                const std::string uuid = match[1].str();

                if (uuid != match[2].str()) {
                    throw ::ParsingError{"The left side doesn't equal to the right side of a project dependency"};
                }

                if (!currentSolutionProject) {
                    throw ::BadParserState{"ProjectDependencies section, but there is no a project"};
                }

                currentSolutionProject->m_dependencies.push_back(uuid);
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }

            return true;
        }
    );

    auto endProject = std::make_shared<State>(
        [&](const std::string& line) {
            if (line == "EndProject") {
                if (!currentSolutionProject) {
                    throw ::BadParserState{"EndProject token, but there is no a project"};
                }
                solution->m_solutionProjects.push_back(currentSolutionProject);
                return true;
            }
            return false;
        }
    );

    start
        >> headerWithFormatVersion
        >> vsMainVersion
        >> fullVsVersion
        >> minimumVsVersion
        >> startProject
        >> endProject
        >> startProject
        >> projectDependenciesSection
        >> endProjectDependenciesSection
        >> endProject;

    projectDependenciesSection >> projectDependency >> endProjectDependenciesSection;
    projectDependency >> projectDependency;

    minimumVsVersion->addNextEnd();
    endProject->addNextEnd();

    const std::vector<char>& content = solutionSource.content();

    const auto bom = findBom(content);

    if (bom != Bom::Utf8) {
        m_lastError = {ErrorCode::Utf8BomNotFound, "BOM is not UTF-8 or BOM is missing."};
        return nullptr;
    }

    const auto lines = getLines(content, bomSize(bom));

    auto currentState = start;
    for (const std::string& line: lines) {
        if (line.empty()) {
            continue;
        }
        try {
            currentState = currentState->getNextState(line);
        }
        catch (const ::BadParserState&) {
            m_lastError = {ErrorCode::BadParserState,fmt::format("Bad parser state: '{}'. Line: '{}'.", line)};
            return nullptr;
        }
        catch (const ::ParsingError& error) {
            m_lastError = {ErrorCode::ParsingError,
                           fmt::format("Parsing error: '{}'. Line: '{}'.", error.what(), line)};
            return nullptr;
        }
    }

    if (!currentState->nextEnd()) {
        m_lastError = {ErrorCode::BadParserState, "Unexpected file end. Maybe the file is not full or truncated."};
    }

    return std::move(solution);
}

const ISolutionParser::Error& SolutionParser::lastError() const
{
    return m_lastError;
}
