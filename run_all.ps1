Get-ChildItem -Path "./build" -Filter "*.exe" | ForEach-Object {
    Invoke-Expression $_.FullName
}