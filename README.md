# 班级综合素质测评管理系统

这是一个 Windows 控制台版 C++ 工程项目，用 `User.txt` 保存默认账号，用运行时 `.txt` 文件保存课程、活动、思想品德评分、附加分和综测总分。`User.txt` 中的密码使用 PBKDF2-SHA256 哈希保存；运行数据文件保存时使用 Windows DPAPI 保护。

## 项目内容

| 文件 | 说明 |
|---|---|
| `CollegeStudentsQualityAssessment.sln` | Visual Studio 2026 解决方案 |
| `CollegeStudentsQualityAssessment.vcxproj` | MSVC v145 C++ 工程 |
| `*.cpp` / `*.h` | 程序源码 |
| `User.txt` | 默认账号数据 |
| `.gitignore` | 忽略 VS 缓存和编译产物 |

## 运行环境

- Windows
- Visual Studio 2026 Community / Professional / Enterprise
- 安装 `Desktop development with C++`
- 安装 MSVC v145 工具集
- 使用 Visual Studio 2026 安装器默认推荐的 Windows SDK
- 打开 `CollegeStudentsQualityAssessment.sln`
- 选择 `Debug | x64` 或 `Release | x64` 后生成并运行

源码和 `User.txt` 均为 UTF-8。工程显式使用 C++17、MSVC v145 和 `/utf-8`，程序启动时会设置控制台输入输出代码页为 UTF-8。运行数据文件由程序生成和读取，不需要人工编辑。

默认情况下，程序从当前工作目录读取 `User.txt`，把运行数据写入 `%LOCALAPPDATA%\StudentQualityAssessment`。可以用命令行参数覆盖运行数据目录：

```powershell
.\CollegeStudentsQualityAssessment.exe --data-dir C:\SQAS\Data
```

`--data-dir` 只改变运行数据目录，不改变 `User.txt` 的读取位置。

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
| `User.txt` | 必需文件，位于当前工作目录，保存用户账号、姓名、密码哈希、身份、打分/生成状态 |
| `Course.txt` | 运行时生成，位于运行数据目录，DPAPI 保护，保存学生课程成绩 |
| `Act.txt` | 运行时生成，位于运行数据目录，DPAPI 保护，保存学生课外活动 |
| `Add.txt` | 运行时生成，位于运行数据目录，DPAPI 保护，保存附加分 |
| `Moral_s.txt` | 运行时生成，位于运行数据目录，DPAPI 保护，保存学生互评 |
| `Moral_t.txt` | 运行时生成，位于运行数据目录，DPAPI 保护，保存辅导员评分 |
| `Total.txt` | 运行时生成，位于运行数据目录，DPAPI 保护，保存综测总分和排名 |
| `app.log` | 运行时生成，位于运行数据目录，记录启动、登录失败、生成综测、保存失败、异常退出 |

`User.txt` 格式保持原规则：第一行是记录数，后面每行是一条记录，字段用空白分隔。运行数据文件保存后不是人工可读明文；程序仍能读取旧明文运行数据，并在下一次保存时写成受保护格式。

不要删除 `User.txt`。程序启动依赖默认账号文件，`User.txt` 缺失或格式错误会直接报错退出。其它运行数据文件可以不存在，程序会按空表启动，并在对应功能保存时自动生成；如果这些文件存在但格式错误，程序仍会直接报错退出。

运行数据文件使用当前 Windows 用户的 DPAPI 保护。复制到其它 Windows 用户或其它机器后，通常不能解密，这是预期行为。保存已有文件前，程序会先生成同名 `.bak` 备份；如果写入失败，旧文件必须保留。程序不会自动修复损坏数据。

## 手工演示流程

1. 学生 `10002`、`10003`、`10004` 分别登录，进入“思想品德项目”，完成互评。
2. 任一学生进入“课外活动项目”，录入一个课外活动。
3. 辅导员 `10001` 登录，完成思想品德打分。
4. 测评小组 `10000` 登录，录入学习成绩，审核课外活动，录入附加分。
5. 测评小组生成综测总分。
6. 学生或测评小组查询综测成绩和排名。

如果 `User.txt` 缺失，或任何已存在的数据文件格式错误，程序会直接报错退出，不继续生成错误结果。

## 仓库清理规则

只提交源码、工程文件、README、`.gitignore` 和必要的默认账号文件 `User.txt`。

不要提交：

- `.vs/`
- `x64/`
- `Debug/`
- `Release/`
- `*.exe`
- `*.obj`
- `*.pdb`
- `*.ilk`
- `Course.txt`
- `Act.txt`
- `Add.txt`
- `Moral_s.txt`
- `Moral_t.txt`
- `Total.txt`
- `*.bak`
