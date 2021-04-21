#include "SolutionParser.h"
#include "Solution.h"
#include "SolutionProjectType.h"
#include "SolutionProject.h"
#include "SolutionGlobal.h"
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
        if (uuid == "2150E333-8FDC-42A3-9474-1A3956D46DE8") {
            return SolutionProjectType::SolutionFolder;
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

    namespace LineParser
    {
        static const std::string uuidPattern =
            "\\{([A-F0-9]{8}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{12})\\}";

        bool headerWithFormatVersion(const std::string& line,
                                     std::unique_ptr<Solution>& solution)
        {
            const std::string header = "Microsoft Visual Studio Solution File, Format Version ";
            if (!startWith(line, header)) {
                return false;
            }
            auto it = line.begin();
            std::advance(it, header.size());
            solution->m_formatVersion = std::string{it, line.cend()};
            return true;
        }

        bool vsMainVersion(const std::string& line)
        {
            return startWith(line, "# Visual Studio Version ");
        }

        bool fullVsVersion(const std::string& line,
                           std::unique_ptr<Solution>& solution)
        {
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

        bool minimumVsVersion(const std::string& line,
                              std::unique_ptr<Solution>& solution)
        {
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

        bool startProject(const std::string& line,
                          std::shared_ptr<SolutionProject>& currentSolutionProject)
        {
            try {
                const std::string quotedUuidPattern = fmt::format("\"{}\"", uuidPattern);
                const std::regex pattern{
                    fmt::format("Project\\({uuid}\\) = \"(.+)\", \"(.+)\", {uuid}",
                                "uuid"_a=quotedUuidPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+4) {
                    return false;
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

        bool projectDependenciesSection(const std::string& line)
        {
            return line == "\tProjectSection(ProjectDependencies) = postProject";
        }

        bool endProjectDependenciesSection(const std::string& line)
        {
            return line == "\tEndProjectSection";
        }

        bool projectDependency(const std::string& line,
                               std::shared_ptr<SolutionProject>& currentSolutionProject)
        {
            try {
                const std::regex pattern{fmt::format("\t\t{uuid} = {uuid}",
                                                     "uuid"_a=uuidPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+2) {
                    return false;
                }

                const std::string uuid = match[1].str();

                if (uuid != match[2].str()) {
                    throw ::ParsingError{
                        "The left side doesn't equal to the right side of a project dependency"};
                }

                if (!currentSolutionProject) {
                    throw ::BadParserState{
                        "ProjectDependencies section, but there is no a project"};
                }

                currentSolutionProject->m_dependencies.push_back(uuid);
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }

            return true;
        }

        bool endProject(const std::string& line,
                        std::unique_ptr<Solution>& solution,
                        std::shared_ptr<SolutionProject>& currentSolutionProject)
        {
            if (line == "EndProject") {
                if (!currentSolutionProject) {
                    throw ::BadParserState{"EndProject token, but there is no a project"};
                }
                solution->m_solutionProjects.push_back(currentSolutionProject);
                currentSolutionProject = nullptr;
                return true;
            }
            return false;
        }

        bool startGlobal(const std::string& line)
        {
            return line == "Global";
        }

        bool endGlobal(const std::string& line)
        {
            return line == "EndGlobal";
        }

        bool globalSolutionConfigurationPlatforms(const std::string& line)
        {
            return line == "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution";
        }

        bool globalProjectConfigurationPlatforms(const std::string& line)
        {
            return line == "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution";
        }

        bool globalSolutionProperties(const std::string& line)
        {
            return line == "\tGlobalSection(SolutionProperties) = preSolution";
        }

        bool globalNestedProjects(const std::string& line)
        {
            return line == "\tGlobalSection(NestedProjects) = preSolution";
        }

        bool globalExtensibilityGlobals(const std::string& line)
        {
            return line == "\tGlobalSection(ExtensibilityGlobals) = postSolution";
        }

        bool endGlobalSection(const std::string& line)
        {
            return line == "\tEndGlobalSection";
        }

        static const std::string configurationPlatformPattern = "(.+)\\|(.+)";

        bool globalSolutionConfigurationPlatformsItem(const std::string& line,
                                                      std::shared_ptr<SolutionGlobal>& global)
        {
            try {
                const std::regex pattern{fmt::format("\t\t{conf} = {conf}",
                                                     "conf"_a = configurationPlatformPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+4) {
                    return false;
                }

                const auto configuration = match[1].str();
                const auto platform = match[2].str();

                if (configuration != match[3].str() || platform != match[4].str()) {
                    throw ::ParsingError{
                        "The left side doesn't equal to the right side of a configuration"};
                }

                global->m_solutionConfigurations.push_back({configuration, platform});
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }
            return true;
        }

        bool globalProjectConfigurationPlatformsItem(const std::string& line,
                                                     std::shared_ptr<SolutionGlobal>& global)
        {
            try {
                const std::regex pattern{
                    fmt::format("\t\t{uuid}\\.{conf}\\.(ActiveCfg|Build.0) = {conf}",
                                "uuid"_a=uuidPattern,
                                "conf"_a=configurationPlatformPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+6) {
                    return false;
                }

                const auto projectUuid = match[1].str();
                const auto solutionConfigurationName = match[2].str();
                const auto solutionPlatform = match[3].str();
                const auto field = match[4].str();
                const auto projectConfigurationName = match[5].str();
                const auto projectPlatform = match[6].str();

                const ISolutionGlobal::ProjectConfigurationMap projectConfigurationMap{
                    projectUuid,
                    {solutionConfigurationName, solutionPlatform},
                    {projectConfigurationName, projectPlatform}
                };

                if (field == "ActiveCfg") {
                    global->m_projectConfigurations.push_back(projectConfigurationMap);
                }
                else if (field == "Build.0") {
                    global->m_builedProjectConfigurations.push_back(projectConfigurationMap);
                }
                else {
                    throw ::ParsingError{"Unexpected project configuration"};
                }
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }
            return true;
        }

        bool globalSolutionPropertiesItem(const std::string& line,
                                          std::shared_ptr<SolutionGlobal>& global)
        {
            try {
                const std::regex pattern{"\t\t(\\w+) = (\\w+)"};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+2) {
                    return false;
                }

                const auto key = match[1].str();
                const auto value = match[2].str();

                global->m_solutionProperties[key] = value;
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }
            return true;
        }

        bool globalExtensibilityGlobalsItem(const std::string& line,
                                            std::shared_ptr<SolutionGlobal>& global)
        {
            try {
                const std::regex pattern{fmt::format("\t\tSolutionGuid = {uuid}",
                                                     "uuid"_a=uuidPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+1) {
                    return false;
                }

                const auto solutionUuid = match[1].str();
                global->m_solutionGuid = solutionUuid;
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }
            return true;
        }

        bool globalNestedProjectsItem(const std::string& line,
                                      std::shared_ptr<SolutionGlobal>& global)
        {
            try {
                const std::regex pattern{fmt::format("\t\t{uuid} = {uuid}",
                                                     "uuid"_a=uuidPattern)};
                std::smatch match;
                const bool ok = std::regex_match(line, match, pattern);

                if (!ok || match.size() != 1+2) {
                    return false;
                }

                const auto projectUuid = match[1].str();
                const auto parentProjectUuid = match[2].str();

                global->m_nestedProjects.push_back({projectUuid, parentProjectUuid});
            }
            catch (const std::regex_error& error) {
                throw ::ParsingError{error.what()};
            }
            return true;
        }
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

std::shared_ptr<State>& operator>>(std::shared_ptr<State>& state,
                                   std::shared_ptr<State>& nextState)
{
    state->addNextState(nextState);
    return nextState;
}

std::unique_ptr<ISolution> SolutionParser::parse(const ISolutionSource& solutionSource)
{
    auto solution = std::make_unique<Solution>();
    std::shared_ptr<SolutionProject> currentSolutionProject;
    auto solutionGlobal = std::make_shared<SolutionGlobal>();
    solution->m_solutionGlobal = solutionGlobal;

    auto start = std::make_shared<State>(
        [](const std::string&)->bool { return true; }
    );

    auto headerWithFormatVersion = std::make_shared<State>([&](const std::string& line) {
        return ::LineParser::headerWithFormatVersion(line, solution);
    });

    auto vsMainVersion = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::vsMainVersion(line);
    });

    auto fullVsVersion = std::make_shared<State>([&](const std::string& line) {
        return ::LineParser::fullVsVersion(line, solution);
    });

    auto minimumVsVersion = std::make_shared<State>([&](const std::string& line) {
        return ::LineParser::minimumVsVersion(line, solution);
    });

    auto startProject = std::make_shared<State>([&](const std::string& line) {
        return ::LineParser::startProject(line, currentSolutionProject);
    });

    auto projectDependenciesSection = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::projectDependenciesSection(line);
    });

    auto endProjectDependenciesSection = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::endProjectDependenciesSection(line);
    });

    auto projectDependency = std::make_shared<State>([&](const std::string& line) {
        return ::LineParser::projectDependency(line, currentSolutionProject);
    });

    auto endProject = std::make_shared<State>([&](const std::string& line) {
        return ::LineParser::endProject(line, solution, currentSolutionProject);
    });

    auto startGlobal = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::startGlobal(line);
    });

    auto endGlobal = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::endGlobal(line);
    });

    auto globalSolutionConfigurationPlatforms = std::make_shared<State>(
        [](const std::string& line) {
            return ::LineParser::globalSolutionConfigurationPlatforms(line);
    });

    auto globalProjectConfigurationPlatforms = std::make_shared<State>(
        [](const std::string& line) {
            return ::LineParser::globalProjectConfigurationPlatforms(line);
    });

    auto globalSolutionProperties = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::globalSolutionProperties(line);
    });

    auto globalNestedProjects = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::globalNestedProjects(line);
    });

    auto globalExtensibilityGlobals = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::globalExtensibilityGlobals(line);
    });

    auto endGlobalSection = std::make_shared<State>([](const std::string& line) {
        return ::LineParser::endGlobalSection(line);
    });

    auto globalSolutionConfigurationPlatformsItem = std::make_shared<State>(
        [&](const std::string& line) {
            return ::LineParser::globalSolutionConfigurationPlatformsItem(line, solutionGlobal);
    });

    auto globalProjectConfigurationPlatformsItem = std::make_shared<State>(
        [&](const std::string& line) {
            return ::LineParser::globalProjectConfigurationPlatformsItem(line, solutionGlobal);
    });

    auto globalSolutionPropertiesItem = std::make_shared<State>(
        [&](const std::string& line) {
            return ::LineParser::globalSolutionPropertiesItem(line, solutionGlobal);
    });

    auto globalExtensibilityGlobalsItem = std::make_shared<State>(
        [&](const std::string& line) {
            return ::LineParser::globalExtensibilityGlobalsItem(line, solutionGlobal);
    });

    auto globalNestedProjectsItem = std::make_shared<State>(
        [&](const std::string& line) {
            return ::LineParser::globalNestedProjectsItem(line, solutionGlobal);
    });

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
        >> endProject
        >> startGlobal
        >> endGlobal;

    projectDependenciesSection >> projectDependency >> endProjectDependenciesSection;
    projectDependency >> projectDependency;

    minimumVsVersion >> startGlobal;

    startGlobal
        >> globalSolutionConfigurationPlatforms
            >> globalSolutionConfigurationPlatformsItem
            >> globalSolutionConfigurationPlatformsItem
        >> endGlobalSection;

    startGlobal
        >> globalProjectConfigurationPlatforms
            >> globalProjectConfigurationPlatformsItem
            >> globalProjectConfigurationPlatformsItem
        >> endGlobalSection;

    startGlobal
        >> globalSolutionProperties
            >> globalSolutionPropertiesItem
            >> globalSolutionPropertiesItem
        >> endGlobalSection;

    startGlobal
        >> globalExtensibilityGlobals
            >> globalExtensibilityGlobalsItem
            >> globalExtensibilityGlobalsItem
        >> endGlobalSection;

    startGlobal
        >> globalNestedProjects
            >> globalNestedProjectsItem
            >> globalNestedProjectsItem
        >> endGlobalSection;

    endGlobalSection >> globalSolutionConfigurationPlatforms >> endGlobalSection;
    endGlobalSection >> globalProjectConfigurationPlatforms >> endGlobalSection;
    endGlobalSection >> globalSolutionProperties >> endGlobalSection;
    endGlobalSection >> globalNestedProjects >> endGlobalSection;
    endGlobalSection >> globalExtensibilityGlobals >> endGlobalSection;
    endGlobalSection >> endGlobal;

    minimumVsVersion->addNextEnd();
    endProject->addNextEnd();
    endGlobal->addNextEnd();

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
        catch (const ::BadParserState& error) {
            m_lastError = {ErrorCode::BadParserState,
                           fmt::format("Bad parser state: '{}'. Line: '{}'.", error.what(), line)};
            return nullptr;
        }
        catch (const ::ParsingError& error) {
            m_lastError = {ErrorCode::ParsingError,
                           fmt::format("Parsing error: '{}'. Line: '{}'.", error.what(), line)};
            return nullptr;
        }
    }

    if (!currentState->nextEnd()) {
        m_lastError = {ErrorCode::BadParserState,
                       "Unexpected file end. Maybe the file is not full or truncated."};
    }

    return std::move(solution);
}

const ISolutionParser::Error& SolutionParser::lastError() const
{
    return m_lastError;
}
