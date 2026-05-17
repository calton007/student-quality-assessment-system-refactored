# 班级综合素质测评管理系统

这是一个 Windows 控制台版 C++ 课程设计项目，用 `.txt` 文件保存用户、课程、活动、思想品德评分、附加分和综测总分。

## 项目内容

| 文件 | 说明 |
|---|---|
| `CollegeStudentsQualityAssessment.sln` | Visual Studio 2026 解决方案 |
| `CollegeStudentsQualityAssessment.vcxproj` | MSVC v145 C++ 工程 |
| `*.cpp` / `*.h` | 程序源码 |
| `*.txt` | 演示数据和运行数据 |
| `.gitignore` | 忽略 VS 缓存和编译产物 |

## 运行环境

- Windows
- Visual Studio 2026 Community / Professional / Enterprise
- 安装 `Desktop development with C++`
- 安装 MSVC v145 工具集
- 使用 Visual Studio 2026 安装器默认推荐的 Windows SDK
- 打开 `CollegeStudentsQualityAssessment.sln`
- 选择 `Debug | x64` 或 `Release | x64` 后生成并运行

源码和数据文件均为 UTF-8。工程显式使用 C++17、MSVC v145 和 `/utf-8`，程序启动时会设置控制台输入输出代码页为 UTF-8。

## 编译方式

推荐方式：

1. 用 Visual Studio 2026 打开 `CollegeStudentsQualityAssessment.sln`。
2. 选择 `Debug | x64`。
3. 点击“生成解决方案”。
4. 直接运行项目。

命令行方式：

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\MSBuild.exe' CollegeStudentsQualityAssessment.sln /p:Configuration=Debug /p:Platform=x64
```

编译产物会生成到 `x64/`，该目录是本机生成文件，已被 `.gitignore` 忽略，不需要提交。

## 默认账号

| 身份 | 账号 | 密码 |
|---|---|---|
| 测评小组 | `10000` | `888888` |
| 辅导员 | `10001` | `888888` |
| 学生1 | `10002` | `888888` |
| 学生2 | `10003` | `888888` |
| 学生3 | `10004` | `888888` |

## 数据文件

| 文件 | 内容 |
|---|---|
| `User.txt` | 用户账号、姓名、密码、身份、打分/生成状态 |
| `Course.txt` | 学生课程成绩 |
| `Act.txt` | 学生课外活动 |
| `Add.txt` | 附加分 |
| `Moral_s.txt` | 学生互评 |
| `Moral_t.txt` | 辅导员评分 |
| `Total.txt` | 综测总分和排名 |

数据格式保持原规则：第一行是记录数，后面每行是一条记录，字段用空白分隔。

不要删除这些 `.txt` 文件。程序启动和功能流程依赖它们；如果文件缺失或格式错误，程序会直接报错退出。

## 手工演示流程

1. 学生 `10002`、`10003`、`10004` 分别登录，进入“思想品德项目”，完成互评。
2. 任一学生进入“课外活动项目”，录入一个课外活动。
3. 辅导员 `10001` 登录，完成思想品德打分。
4. 测评小组 `10000` 登录，录入学习成绩，审核课外活动，录入附加分。
5. 测评小组生成综测总分。
6. 学生或测评小组查询综测成绩和排名。

如果数据文件缺失或格式错误，程序会直接报错退出，不继续生成错误结果。

## 仓库清理规则

只提交源码、工程文件、README、`.gitignore` 和必要的 `.txt` 数据文件。

不要提交：

- `.vs/`
- `x64/`
- `Debug/`
- `Release/`
- `*.exe`
- `*.obj`
- `*.pdb`
- `*.ilk`
