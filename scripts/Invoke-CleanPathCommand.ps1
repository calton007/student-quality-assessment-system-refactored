param(
    [Parameter(Mandatory = $true)]
    [string]$FilePath,

    [string[]]$ArgumentList = @()
)

$pathValue = $null
$cleanEnvironment = [System.Collections.Generic.Dictionary[string, string]]::new([System.StringComparer]::OrdinalIgnoreCase)

foreach ($entry in [System.Environment]::GetEnvironmentVariables().GetEnumerator()) {
    $name = [string]$entry.Key
    $value = [string]$entry.Value

    if ($name -ieq 'Path') {
        if ($null -eq $pathValue -or $name -ceq 'Path') {
            $pathValue = $value
        }
        continue
    }

    if (-not $cleanEnvironment.ContainsKey($name)) {
        $cleanEnvironment.Add($name, $value)
    }
}

if ($null -ne $pathValue) {
    $cleanEnvironment['Path'] = $pathValue
}

$startInfo = [System.Diagnostics.ProcessStartInfo]::new()
$startInfo.FileName = $FilePath
function ConvertTo-CommandLineArgument {
    param([string]$Argument)

    if ($Argument -notmatch '[\s"]') {
        return $Argument
    }

    $builder = [System.Text.StringBuilder]::new()
    [void]$builder.Append('"')
    $backslashes = 0

    foreach ($character in $Argument.ToCharArray()) {
        if ($character -eq '\') {
            $backslashes++
            continue
        }

        if ($character -eq '"') {
            [void]$builder.Append('\' * ($backslashes * 2 + 1))
            [void]$builder.Append('"')
            $backslashes = 0
            continue
        }

        if ($backslashes -gt 0) {
            [void]$builder.Append('\' * $backslashes)
            $backslashes = 0
        }
        [void]$builder.Append($character)
    }

    if ($backslashes -gt 0) {
        [void]$builder.Append('\' * ($backslashes * 2))
    }

    [void]$builder.Append('"')
    return $builder.ToString()
}

$startInfo.Arguments = ($ArgumentList | ForEach-Object { ConvertTo-CommandLineArgument $_ }) -join ' '
$startInfo.UseShellExecute = $false
$startInfo.RedirectStandardOutput = $false
$startInfo.RedirectStandardError = $false

$startInfo.Environment.Clear()
foreach ($entry in $cleanEnvironment.GetEnumerator()) {
    $startInfo.Environment[$entry.Key] = $entry.Value
}

$process = [System.Diagnostics.Process]::Start($startInfo)
$process.WaitForExit()
exit $process.ExitCode
