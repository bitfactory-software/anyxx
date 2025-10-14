# === KONFIGURATION ===

# Quelle: Ordner, in dem gesucht wird
$sourceDir = "virtual_void"

# Dateimuster: z. B. *.cs, *.txt, *.md
$filePattern = "*.hpp"

# Zielordner (alle passenden Dateien werden dorthin verschoben)
$destinationRoot = "anyxx"

# === DEBUG / Logging ===

# Zeige, wo wir starten
Write-Host "Skript gestartet in Verzeichnis: $(Get-Location)" -ForegroundColor Cyan

# Finde Dateien
$files = Get-ChildItem -Path $sourceDir -Recurse -Filter $filePattern -File -ErrorAction SilentlyContinue

if (!$files) {
    Write-Host "Keine Dateien gefunden mit Muster '$filePattern' unter '$sourceDir'" -ForegroundColor Yellow
} else {
    Write-Host "Gefundene Dateien:" -ForegroundColor Green
    foreach ($f in $files) {
        Write-Host "  $($f.FullName)"
    }
}

foreach ($file in $files) {
    # relativer Pfad vom Repo-Stamm aus
    $repoRoot = Get-Location
    $absSource = $file.FullName
    # +1 für den Backslash nach dem Pfad
    $relativeSource = $absSource.Substring($repoRoot.Path.Length + 1)

    Write-Host "=== Bearbeite Datei ===" -ForegroundColor Cyan
    Write-Host "Absolute Quelle: $absSource"
    Write-Host "Relativer Pfad: $relativeSource"

    # berechne Teilpfad unter $sourceDir
    $prefix = (Join-Path $repoRoot $sourceDir)
    Write-Host "Prefix (Quelle Basis): $prefix"
    $rest = $absSource.Substring($prefix.Length).TrimStart("\", "/")
    Write-Host "Rest (nach Quelle): $rest"

    $destinationPath = Join-Path $destinationRoot $rest
    Write-Host "Geplantes Ziel: $destinationPath"

    # Zielverzeichnis
    $destDir = Split-Path $destinationPath
    if (!(Test-Path $destDir)) {
        Write-Host "Erstelle Verzeichnis: $destDir"
        New-Item -ItemType Directory -Path $destDir -Force | Out-Null
    } else {
        Write-Host "Verzeichnis existiert bereits: $destDir"
    }

    # Git mv-Aufruf
    $cmd = "git mv -- `"$relativeSource`" `"$destinationPath`""
    Write-Host "Führe aus: $cmd" -ForegroundColor Magenta

    # Tatsächlicher Aufruf
    & git mv -- "$relativeSource" "$destinationPath"
}

Write-Host "Fertig. Zeige git status..." -ForegroundColor Cyan
git status
