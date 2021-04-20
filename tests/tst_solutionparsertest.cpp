#include "SolutionParser/ISolution.h"
#include "SolutionParser/ISolutionSource.h"
#include "SolutionParser/SolutionParser.h"
#include "TestSuite.h"
#include "SolutionParser/ISolutionProject.h"
#include "SolutionParser/SolutionProjectType.h"
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

    QVERIFY2(solution, "nullptr");
    QCOMPARE(solution->formatVersion(), "12.00"s);
    QCOMPARE(solution->visualStudioVersion(), "16.0.30204.135"s);
    QCOMPARE(solution->minimumVisualStudioVersion(), "10.0.40219.1"s);

    const auto& projects = solution->solutionProjects();

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

    QVERIFY2(solution, "nullptr");
    QCOMPARE(solution->formatVersion(), "12.00"s);
    QCOMPARE(solution->visualStudioVersion(), "16.0.30204.135"s);
    QCOMPARE(solution->minimumVisualStudioVersion(), "10.0.40219.1"s);

    const auto& projects = solution->solutionProjects();

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

#include "tst_solutionparsertest.moc"
