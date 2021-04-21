#include "SolutionParser/ISolution.h"
#include "SolutionParser/ISolutionSource.h"
#include "SolutionParser/SolutionParser.h"
#include "TestSuite.h"
#include "SolutionParser/ISolutionProject.h"
#include "SolutionParser/SolutionProjectType.h"
#include "SolutionParser/SolutionGlobal.h"
#include "SolutionParser/FileSolutionSource.h"
#include <QtTest>
#include <memory>

using namespace std::string_literals;

class TestSolutionSource: public ISolutionSource
{
public:
    TestSolutionSource(const char* content);
    virtual const std::vector<char>& content() const override;

private:
    std::vector<char> m_content;
};

TestSolutionSource::TestSolutionSource(const char* content)
{
    while(*content != '\0') {
        m_content.push_back(*content);
        ++content;
    }
}

const std::vector<char>& TestSolutionSource::content() const
{
    return m_content;
}

class SolutionParserTest: public TestSuite
{
    Q_OBJECT

public:
    SolutionParserTest();
    ~SolutionParserTest();

private slots:
    void ProjectWithoutDependencies();
    void ProjectWithDependencies();
    void SolutionWithoutProjects();
    void ComplexSolutionFile();
};

static SolutionParserTest SolutionParserTestObject;

SolutionParserTest::SolutionParserTest()
{
}

SolutionParserTest::~SolutionParserTest()
{
}

void SolutionParserTest::ProjectWithoutDependencies()
{
    TestSolutionSource solutionSource =
        "\xEF\xBB\xBF\r\n"
        "Microsoft Visual Studio Solution File, Format Version 12.00\r\n"
        "# Visual Studio Version 16\r\n"
        "VisualStudioVersion = 16.0.30204.135\r\n"
        "MinimumVisualStudioVersion = 10.0.40219.1\r\n"
        "Project(\"{F184B08F-C81C-45F6-A57F-5ABD9991F28F}\") = \"Project1\", \"Project1.vbproj\", \"{8CDD8387-B905-44A8-B5D5-07BB50E05BEA}\"\r\n"
        "EndProject\r\n"
    ;

    std::unique_ptr<ISolutionParser> solutionParser = std::make_unique<SolutionParser>();
    auto solution = solutionParser->parse(solutionSource);

    const auto& error = solutionParser->lastError();
    QVERIFY2(error.code == ISolutionParser::ErrorCode::Ok, error.description.c_str());

    QVERIFY2(solution, "nullptr");
    QCOMPARE(solution->formatVersion(), "12.00"s);
    QCOMPARE(solution->visualStudioVersion(), "16.0.30204.135"s);
    QCOMPARE(solution->minimumVisualStudioVersion(), "10.0.40219.1"s);

    const auto& projects = solution->projects();

    QVERIFY(projects.size() == 1);
    const auto& project = projects.front();
    QCOMPARE(project->type(), SolutionProjectType::VisualBasicNet);
    QCOMPARE(project->name(), "Project1"s);
    QCOMPARE(project->path(), "Project1.vbproj"s);
    QCOMPARE(project->uuid(), "8CDD8387-B905-44A8-B5D5-07BB50E05BEA"s);
    QVERIFY(project->dependencies().empty());
}

void SolutionParserTest::ProjectWithDependencies()
{
    TestSolutionSource solutionSource =
        "\xEF\xBB\xBF\r\n"
        "Microsoft Visual Studio Solution File, Format Version 12.00\r\n"
        "# Visual Studio Version 16\r\n"
        "VisualStudioVersion = 16.0.30204.135\r\n"
        "MinimumVisualStudioVersion = 10.0.40219.1\r\n"
        "Project(\"{F184B08F-C81C-45F6-A57F-5ABD9991F28F}\") = \"Project1\", \"Project1.vbproj\", \"{8CDD8387-B905-44A8-B5D5-07BB50E05BEA}\"\r\n"
        "\tProjectSection(ProjectDependencies) = postProject\r\n"
        "\t\t{0A9C3D1F-3D94-4085-9EA3-56B43FA5FE87} = {0A9C3D1F-3D94-4085-9EA3-56B43FA5FE87}\r\n"
        "\t\t{D9B28851-AEAE-472F-B416-81147A831155} = {D9B28851-AEAE-472F-B416-81147A831155}\r\n"
        "\tEndProjectSection\r\n"
        "EndProject\r\n"
    ;

    std::unique_ptr<ISolutionParser> solutionParser = std::make_unique<SolutionParser>();
    auto solution = solutionParser->parse(solutionSource);

    const auto& error = solutionParser->lastError();
    QVERIFY2(error.code == ISolutionParser::ErrorCode::Ok, error.description.c_str());

    QVERIFY2(solution, "nullptr");
    QCOMPARE(solution->formatVersion(), "12.00"s);
    QCOMPARE(solution->visualStudioVersion(), "16.0.30204.135"s);
    QCOMPARE(solution->minimumVisualStudioVersion(), "10.0.40219.1"s);

    const auto& projects = solution->projects();

    QVERIFY(projects.size() == 1);
    const auto& project = projects.front();
    QCOMPARE(project->type(), SolutionProjectType::VisualBasicNet);
    QCOMPARE(project->name(), "Project1"s);
    QCOMPARE(project->path(), "Project1.vbproj"s);
    QCOMPARE(project->uuid(), "8CDD8387-B905-44A8-B5D5-07BB50E05BEA"s);
    const auto& projectDependencies = project->dependencies();
    decltype(projectDependencies) expectedDependencies{"0A9C3D1F-3D94-4085-9EA3-56B43FA5FE87",
                                                       "D9B28851-AEAE-472F-B416-81147A831155"};
    QCOMPARE(projectDependencies, expectedDependencies);
}

void SolutionParserTest::SolutionWithoutProjects()
{
    // Microsoft Visual Studio Community 2019, Version 16.8.6
    TestSolutionSource solutionSource =
        "\xEF\xBB\xBF\r\n"
        "Microsoft Visual Studio Solution File, Format Version 12.00\r\n"
        "# Visual Studio Version 16\r\n"
        "VisualStudioVersion = 16.0.31019.35\r\n"
        "MinimumVisualStudioVersion = 10.0.40219.1\r\n"
        "Global\r\n"
        "\tGlobalSection(SolutionProperties) = preSolution\r\n"
        "\t\tHideSolutionNode = FALSE\r\n"
        "\tEndGlobalSection\r\n"
        "\tGlobalSection(ExtensibilityGlobals) = postSolution\r\n"
        "\t\tSolutionGuid = {DA7347E1-5A6D-4A1C-A0F8-10A000F06D18}\r\n"
        "\tEndGlobalSection\r\n"
        "EndGlobal\r\n"
    ;

    std::unique_ptr<ISolutionParser> solutionParser = std::make_unique<SolutionParser>();
    auto solution = solutionParser->parse(solutionSource);

    const auto& error = solutionParser->lastError();
    QVERIFY2(error.code == ISolutionParser::ErrorCode::Ok, error.description.c_str());

    QVERIFY2(solution, "nullptr");
    QCOMPARE(solution->formatVersion(), "12.00"s);
    QCOMPARE(solution->visualStudioVersion(), "16.0.31019.35"s);
    QCOMPARE(solution->minimumVisualStudioVersion(), "10.0.40219.1"s);

    QVERIFY(solution->projects().size() == 0);

    QCOMPARE(solution->global()->solutionGuid(), "DA7347E1-5A6D-4A1C-A0F8-10A000F06D18"s);
    QVERIFY(solution->global()->solutionConfigurations().empty());
    QVERIFY(solution->global()->projectConfigurations().empty());
    QVERIFY(solution->global()->buildedProjectConfigurations().empty());
    QVERIFY(solution->global()->nestedProjects().empty());
    QVERIFY(solution->global()->solutionProperties().size() == 1);
    QVERIFY(solution->global()->solutionProperties().at("HideSolutionNode") == "FALSE");
}

void SolutionParserTest::ComplexSolutionFile()
{
    FileSolutionSource solutionFile{"TestSolutions/Project1.sln"};

    std::unique_ptr<ISolutionParser> solutionParser = std::make_unique<SolutionParser>();
    auto solution = solutionParser->parse(solutionFile);

    const auto& error = solutionParser->lastError();
    QVERIFY2(error.code == ISolutionParser::ErrorCode::Ok, error.description.c_str());

    QVERIFY2(solution, "nullptr");
    QCOMPARE(solution->formatVersion(), "12.00"s);
    QCOMPARE(solution->visualStudioVersion(), "16.0.31019.35"s);
    QCOMPARE(solution->minimumVisualStudioVersion(), "10.0.40219.1"s);

    QVERIFY(solution->projects().size() == 6);

    auto projectIt = solution->projects().begin();
    const auto& project1 = *projectIt++;
    const auto& project2 = *projectIt++;
    const auto& project3 = *projectIt++;
    const auto& project4 = *projectIt++;
    const auto& project5 = *projectIt++;
    const auto& project6 = *projectIt++;

    QCOMPARE(project1->type(), SolutionProjectType::Cpp);
    QCOMPARE(project1->name(), "Project1"s);
    QCOMPARE(project1->path(), "Project1\\Project1.vcxproj"s);
    QCOMPARE(project1->uuid(), "F662A396-E2A5-4624-83D4-F4D1302EB40F"s);

    QCOMPARE(project2->type(), SolutionProjectType::Cpp);
    QCOMPARE(project2->name(), "Project2"s);
    QCOMPARE(project2->path(), "Project2\\Project2.vcxproj"s);
    QCOMPARE(project2->uuid(), "07300CAA-A7BF-46FE-9F6A-99F1483AF3DD"s);

    QCOMPARE(project3->type(), SolutionProjectType::Cpp);
    QCOMPARE(project3->name(), "Project3"s);
    QCOMPARE(project3->path(), "Project3\\Project3.vcxproj"s);
    QCOMPARE(project3->uuid(), "08CA648A-BBA8-424D-BD56-3E92917332EA"s);

    QCOMPARE(project4->type(), SolutionProjectType::Cpp);
    QCOMPARE(project4->name(), "Project4"s);
    QCOMPARE(project4->path(), "Project4\\Project4.vcxproj"s);
    QCOMPARE(project4->uuid(), "C77AD214-64E6-4953-8738-892587E078E2"s);

    QCOMPARE(project5->type(), SolutionProjectType::SolutionFolder);
    QCOMPARE(project5->name(), "NewFolder1"s);
    QCOMPARE(project5->path(), "NewFolder1"s);
    QCOMPARE(project5->uuid(), "9C755DE8-C972-49A7-98BE-3FEAEB827021"s);

    QCOMPARE(project6->type(), SolutionProjectType::SolutionFolder);
    QCOMPARE(project6->name(), "NewFolder2"s);
    QCOMPARE(project6->path(), "NewFolder2"s);
    QCOMPARE(project6->uuid(), "91128EF1-6268-4067-9CCC-2E9015FBEB5C"s);

    QVERIFY(project1->dependencies().size() == 2);
    QVERIFY(project2->dependencies().size() == 2);
    QVERIFY(project3->dependencies().size() == 0);
    QVERIFY(project4->dependencies().size() == 0);
    QVERIFY(project5->dependencies().size() == 0);
    QVERIFY(project6->dependencies().size() == 0);

    const auto& projectDependencies1 = project1->dependencies();
    decltype(projectDependencies1) expectedDependencies1{"C77AD214-64E6-4953-8738-892587E078E2",
                                                         "08CA648A-BBA8-424D-BD56-3E92917332EA"};
    QCOMPARE(projectDependencies1, expectedDependencies1);

    const auto& projectDependencies2 = project2->dependencies();
    decltype(projectDependencies2) expectedDependencies2{"C77AD214-64E6-4953-8738-892587E078E2",
                                                         "08CA648A-BBA8-424D-BD56-3E92917332EA"};
    QCOMPARE(projectDependencies2, expectedDependencies2);

    const auto& global = solution->global();

    QVERIFY(global->solutionConfigurations().size() == 4);
    auto configurationsIt = global->solutionConfigurations().begin();
    QCOMPARE(configurationsIt->name, "Debug"s);
    QCOMPARE(configurationsIt->platform, "x64"s);
    ++configurationsIt;
    QCOMPARE(configurationsIt->name, "Debug"s);
    QCOMPARE(configurationsIt->platform, "x86"s);
    ++configurationsIt;
    QCOMPARE(configurationsIt->name, "Release"s);
    QCOMPARE(configurationsIt->platform, "x64"s);
    ++configurationsIt;
    QCOMPARE(configurationsIt->name, "Release"s);
    QCOMPARE(configurationsIt->platform, "x86"s);

    QVERIFY(global->solutionProperties().size() == 1);
    QCOMPARE(global->solutionProperties().at("HideSolutionNode"), "FALSE"s);

    QCOMPARE(global->solutionGuid(), "DA7347E1-5A6D-4A1C-A0F8-10A000F06D18"s);

    QVERIFY(global->nestedProjects().size() == 2);
    auto nestedProjectsIt = global->nestedProjects().begin();
    QCOMPARE(nestedProjectsIt->projectUuid, "08CA648A-BBA8-424D-BD56-3E92917332EA"s);
    QCOMPARE(nestedProjectsIt->parentProjectUuid, "91128EF1-6268-4067-9CCC-2E9015FBEB5C"s);
    ++nestedProjectsIt;
    QCOMPARE(nestedProjectsIt->projectUuid, "C77AD214-64E6-4953-8738-892587E078E2"s);
    QCOMPARE(nestedProjectsIt->parentProjectUuid, "91128EF1-6268-4067-9CCC-2E9015FBEB5C"s);

    QVERIFY(global->projectConfigurations().size() == 16);
    QVERIFY(global->buildedProjectConfigurations().size() == 16);
    auto projectConfigurationsIt = global->projectConfigurations().begin();
    auto buildedProjectConfigurationsIt = global->buildedProjectConfigurations().begin();

    auto compareProjectConfiguration = [&](const std::string& projectUuid){
        QCOMPARE(projectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.name, "Debug"s);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.platform, "x64"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.name, "Debug"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.platform, "x64"s);
        ++projectConfigurationsIt;
        QCOMPARE(buildedProjectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.name, "Debug"s);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.platform, "x64"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.name, "Debug"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.platform, "x64"s);
        ++buildedProjectConfigurationsIt;
        QCOMPARE(projectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.name, "Debug"s);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.platform, "x86"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.name, "Debug"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.platform, "Win32"s);
        ++projectConfigurationsIt;
        QCOMPARE(buildedProjectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.name, "Debug"s);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.platform, "x86"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.name, "Debug"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.platform, "Win32"s);
        ++buildedProjectConfigurationsIt;
        QCOMPARE(projectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.name, "Release"s);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.platform, "x64"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.name, "Release"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.platform, "x64"s);
        ++projectConfigurationsIt;
        QCOMPARE(buildedProjectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.name, "Release"s);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.platform, "x64"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.name, "Release"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.platform, "x64"s);
        ++buildedProjectConfigurationsIt;
        QCOMPARE(projectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.name, "Release"s);
        QCOMPARE(projectConfigurationsIt->solutionConfiguration.platform, "x86"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.name, "Release"s);
        QCOMPARE(projectConfigurationsIt->projectConfiguration.platform, "Win32"s);
        ++projectConfigurationsIt;
        QCOMPARE(buildedProjectConfigurationsIt->projectUuid, projectUuid);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.name, "Release"s);
        QCOMPARE(buildedProjectConfigurationsIt->solutionConfiguration.platform, "x86"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.name, "Release"s);
        QCOMPARE(buildedProjectConfigurationsIt->projectConfiguration.platform, "Win32"s);
        ++buildedProjectConfigurationsIt;
    };

    compareProjectConfiguration("F662A396-E2A5-4624-83D4-F4D1302EB40F");
    compareProjectConfiguration("07300CAA-A7BF-46FE-9F6A-99F1483AF3DD");
    compareProjectConfiguration("08CA648A-BBA8-424D-BD56-3E92917332EA");
    compareProjectConfiguration("C77AD214-64E6-4953-8738-892587E078E2");
}

#include "tst_solutionparsertest.moc"
