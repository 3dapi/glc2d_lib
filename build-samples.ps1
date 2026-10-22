#requires -Version 7.0
[CmdletBinding()]
param(
    [ValidateSet('Debug','Release')][string[]]$Configuration = @('Debug','Release'),
    [ValidateSet('Win32','x64')][string[]]$Platform = @('Win32','x64'),
    [ValidateRange(1,8)][int]$ThrottleLimit = 4
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuild = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (!$msbuild) { throw 'MSBuild with the Visual C++ toolchain is required.' }
$nuget = (Get-Command nuget -ErrorAction Stop).Source
$packageRoot = Join-Path $root 'packages'
$dxName = 'Microsoft.DXSDK.D3DX.9.29.952.8'
$dxRoot = Join-Path $packageRoot $dxName
$logRoot = Join-Path $root 'build/samples'
New-Item -ItemType Directory -Path $logRoot -Force | Out-Null

# Prefer installed packages so local sample builds do not require a download.
if (!(Test-Path -LiteralPath (Join-Path $dxRoot 'build/native/Microsoft.DXSDK.D3DX.targets'))) {
    $candidates = @(
        (Join-Path $root "sdk_src/packages/$dxName"),
        (Join-Path $env:USERPROFILE '.nuget/packages/microsoft.dxsdk.d3dx/9.29.952.8')
    )
    $cached = $candidates | Where-Object { Test-Path -LiteralPath (Join-Path $_ 'build/native/Microsoft.DXSDK.D3DX.targets') } | Select-Object -First 1
    if ($cached) {
        New-Item -ItemType Directory -Path $dxRoot -Force | Out-Null
        Copy-Item -Path (Join-Path $cached '*') -Destination $dxRoot -Recurse -Force
    } else {
        & $nuget install Microsoft.DXSDK.D3DX -Version 9.29.952.8 -OutputDirectory $packageRoot -NonInteractive
        if ($LASTEXITCODE) { throw 'DirectX SDK package restore failed.' }
    }
}

# Build the matching source libraries before packing or compiling samples.
foreach ($arch in $Platform) {
    foreach ($config in $Configuration) {
        $log = Join-Path $logRoot "sdk-$arch-$config.log"
        & $msbuild (Join-Path $root 'sdk_src/glc2d.vcxproj') "/p:Configuration=$config" "/p:Platform=$arch" /v:minimal /nologo *> $log
        if ($LASTEXITCODE) { Get-Content -LiteralPath $log -Tail 20; throw "SDK $arch $config failed." }
        Write-Host "SDK $arch $config passed"
    }
}

[xml]$spec = Get-Content -LiteralPath (Join-Path $root 'nuget/glc2d.nuspec') -Raw
$version = [string]$spec.package.metadata.version
$package = Join-Path $root "nuget/glc2d.$version.nupkg"
& $nuget pack (Join-Path $root 'nuget/glc2d.nuspec') -OutputDirectory (Join-Path $root 'nuget') -NoPackageAnalysis -NonInteractive *> (Join-Path $logRoot 'nuget-pack.log')
if ($LASTEXITCODE) { Get-Content -LiteralPath (Join-Path $logRoot 'nuget-pack.log') -Tail 20; throw 'Local glc2d package creation failed.' }
$installed = Join-Path $packageRoot "glc2d.$version"
New-Item -ItemType Directory -Path $installed -Force | Out-Null
# Refresh this development package on every run, even if its version already exists locally.
[System.IO.Compression.ZipFile]::ExtractToDirectory($package, $installed, $true)
Copy-Item -LiteralPath $package -Destination (Join-Path $installed "glc2d.$version.nupkg") -Force
Write-Host "Local glc2d $version package refreshed"

$projects = @(
    Get-ChildItem -LiteralPath (Join-Path $root 'example') -Filter '*.vcxproj' -Recurse | Where-Object { $_.Directory.Parent.Name -eq 'example' }
    Get-ChildItem -LiteralPath (Join-Path $root 'app_nuget') -Filter '*.vcxproj' -Recurse | Where-Object { $_.Directory.Parent.Name -eq 'app_nuget' }
)
$results = @($projects | ForEach-Object -ThrottleLimit $ThrottleLimit -Parallel {
    $project = $_
    foreach ($arch in $using:Platform) {
        foreach ($config in $using:Configuration) {
            $label = "$($project.Directory.Parent.Name)-$($project.Directory.Name)-$arch-$config"
            $log = Join-Path $using:logRoot "$label.log"
            & $using:msbuild $project.FullName "/p:Configuration=$config" "/p:Platform=$arch" /p:BuildProjectReferences=false /v:minimal /nologo *> $log
            $passed = $LASTEXITCODE -eq 0
            Write-Host "$label $(if($passed){'passed'}else{'FAILED'})"
            [pscustomobject]@{ Project=$project.FullName; Platform=$arch; Configuration=$config; Passed=$passed; Log=$log }
        }
    }
})
$results | Sort-Object Project,Platform,Configuration | Export-Csv -LiteralPath (Join-Path $logRoot 'summary.csv') -NoTypeInformation -Encoding utf8
$failed = @($results | Where-Object { !$_.Passed })
Write-Host "$($results.Count-$failed.Count)/$($results.Count) sample builds passed across $($projects.Count) projects"
if ($failed.Count) {
    foreach ($failure in $failed) { Write-Host $failure.Log; Get-Content -LiteralPath $failure.Log -Tail 12 }
    throw "$($failed.Count) sample builds failed. See build/samples/summary.csv."
}
