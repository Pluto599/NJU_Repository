$ErrorActionPreference = 'Stop'

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path | Split-Path -Parent
$WrapperDir = Join-Path $ProjectRoot '.mvn/wrapper'
New-Item -ItemType Directory -Force -Path $WrapperDir | Out-Null

$PropsPath = Join-Path $WrapperDir 'maven-wrapper.properties'
if (-not (Test-Path $PropsPath)) {
  throw "Missing $PropsPath."
}

$props = Get-Content $PropsPath | Where-Object { $_ -match '=' } | ForEach-Object {
  $k, $v = $_.Split('=', 2)
  [PSCustomObject]@{ Key = $k.Trim(); Value = $v.Trim() }
}

$wrapperUrl = ($props | Where-Object Key -eq 'wrapperUrl' | Select-Object -First 1).Value
if (-not $wrapperUrl) {
  throw 'wrapperUrl not found in maven-wrapper.properties'
}

$JarPath = Join-Path $WrapperDir 'maven-wrapper.jar'
Write-Host "Downloading Maven Wrapper jar..."
Invoke-WebRequest -Uri $wrapperUrl -OutFile $JarPath
Write-Host "Saved: $JarPath"
