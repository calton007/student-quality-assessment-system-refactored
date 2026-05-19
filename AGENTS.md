# AGENTS.md

## 构建与测试安全规则

1. 构建或测试前，先检查残留进程：
   - `CollegeStudentsQualityAssessment.exe`
   - `CollegeStudentsQualityAssessment.Tests.exe`

2. 如果 Debug exe 被占用，先列出 PID、进程名、路径，再处理。
   不允许在 exe 被占用时反复运行 MSBuild。

3. 运行测试必须设置超时。
   测试超过 60 秒无退出，视为卡住；停止测试进程并定位最后一个可能卡住的用例。

4. 自动化测试不得依赖真实控制台输入。
   禁止测试调用 `ConsoleInput::pause()`、主菜单死循环、`system("pause")` 等交互等待路径。
   测输入必须用 `std::istringstream` 替换 `std::cin`。

5. 不要运行主程序 exe 来验证自动化测试。
   主程序是交互程序，只能用于手工演示；自动验证使用测试 exe。

6. MSBuild 报 `LNK1168` 时，优先判断为目标 exe 被占用。
   先查进程，不要改代码、不要清理工程、不要重复构建。

7. MSBuild 报 `Path/PATH` 环境变量重复时，可在沙箱外重跑构建。
   这不是代码错误，不要因此修改工程文件。

