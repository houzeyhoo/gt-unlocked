$x64_path = "build/x64/Release/dinput8.dll"
$x86_path = "build/x86/Release/dinput8.dll"

$temp_dir = ".temp"
$legacy_dir = "$temp_dir/legacy"
$steam_uc_dir = "$temp_dir/steam-uc"

$zip_file = "release_files.zip"

New-Item -Path $temp_dir -ItemType Directory -Force | Out-Null
New-Item -Path $legacy_dir -ItemType Directory -Force | Out-Null
New-Item -Path $steam_uc_dir -ItemType Directory -Force | Out-Null
Copy-Item -Path $x64_path -Destination $legacy_dir -Force
Copy-Item -Path $x86_path -Destination $steam_uc_dir -Force

Compress-Archive -Path $legacy_dir, $steam_uc_dir -DestinationPath $zip_file -Force
Remove-Item -Path $temp_dir -Recurse -Force
Write-Host "Zipped $x64_path and $x86_path into $zip_file."