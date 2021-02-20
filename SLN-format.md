# Visual Studio Solution Format

## Project Statement

```
Project("<PROJECT_TYPE_GUID>") = "<PROJECT_NAME>", "<PROJECT_PATH>", "<UNIQUE_PROJECT_GUID>"
EndProject
```

For example:

```
Project("{F184B08F-C81C-45F6-A57F-5ABD9991F28F}") = "Project1", "Project1.vbproj", "{8CDD8387-B905-44A8-B5D5-07BB50E05BEA}"
EndProject
```

`<PROJECT_TYPE_GUID>`

| Project Type |                 GUID                   |
|--------------|----------------------------------------|
| C++          | {8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942} |
| VB.NET       | {F184B08F-C81C-45F6-A57F-5ABD9991F28F} |

List of known project type Guids
https://www.mztools.com/articles/2008/mz2008017.aspx

```
Project("<PROJECT_TYPE_GUID>") = "<PROJECT_NAME>", "<PROJECT_PATH>", "<UNIQUE_PROJECT_GUID>"
	ProjectSection(ProjectDependencies) = postProject
		<UNIQUE_PROJECT_GUID> = <UNIQUE_PROJECT_GUID>
		...
	EndProjectSection
EndProject
```

Example

```
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "UnitTests", "UnitTests\UnitTests.vcxproj", "{136C613B-F172-42DC-A1CA-DD9F8B569686}"
	ProjectSection(ProjectDependencies) = postProject
		{0A9C3D1F-3D94-4085-9EA3-56B43FA5FE87} = {0A9C3D1F-3D94-4085-9EA3-56B43FA5FE87}
		{D9B28851-AEAE-472F-B416-81147A831155} = {D9B28851-AEAE-472F-B416-81147A831155}
	EndProjectSection
EndProject
```

The project references to the dependencies by `<UNIQUE_PROJECT_GUID>` in the `ProjectSection(ProjectDependencies)` statement.
