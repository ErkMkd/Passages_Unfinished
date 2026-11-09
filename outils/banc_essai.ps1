# Banc d'essai de Passages : rejoue chaque scène à temps fixe (60 images/s) et compare
# les images obtenues aux images de référence.
#
#   .\outils\banc_essai.ps1                 compare aux références (banc_essai\references)
#   .\outils\banc_essai.ps1 -References     crée ou remplace les références
#   .\outils\banc_essai.ps1 -Scenes tunnel,arbre
#
# Les captures et les images des différences (amplifiées) sont dans banc_essai\captures.
param(
    [switch]$References,
    [string[]]$Scenes,
    [string]$Configuration = "Debug"
)

[Console]::OutputEncoding = [Text.Encoding]::UTF8
$racine = Split-Path $PSScriptRoot
$exe = Join-Path $racine "build\$Configuration\Passages.exe"
if (-not (Test-Path $exe)) { Write-Error "Introuvable : $exe (compiler d'abord la démo)"; exit 2 }

# Instants capturés pour chaque scène de départ (en secondes depuis le début de la scène).
$plan_complet = [ordered]@{
    logo      = "2,6,10"
    tunnel    = "3,10,20,30,40"
    caducee   = "3,15,30,45,55"
    starfield = "5,15,25,40,60,80,95,105"
    arbre     = "2,6,9"
    greetings = "3,10,20"
    credits   = "3,10"
    fin       = "3,10,30"
}
$plan = $plan_complet
if ($Scenes) { $plan = [ordered]@{}; foreach ($s in $Scenes) { $plan[$s] = $plan_complet[$s] } }

$dossier = Join-Path $racine "banc_essai"
$dossier_references = Join-Path $dossier "references"
$dossier_captures = Join-Path $dossier "captures"
if (-not $References) {
    if (-not (Test-Path $dossier_references)) { Write-Error "Pas de références : lancer d'abord avec -References"; exit 2 }
    if (Test-Path $dossier_captures) { Remove-Item "$dossier_captures\*.png" -ErrorAction SilentlyContinue }
}

$echecs = 0
foreach ($scene in $plan.Keys) {
    $arguments = @("--scene", $scene, "--temps-fixe", "60", "--captures", $plan[$scene])
    if ($References) { $arguments += @("--sortie", $dossier_references) }
    else { $arguments += @("--sortie", $dossier_captures, "--references", $dossier_references) }
    $sortie = & $exe @arguments 2>&1
    $code = $LASTEXITCODE
    if ($References) {
        $nbr = ($sortie | Select-String -Pattern '^Capture: ').Count
        "{0,-10} {1} référence(s)" -f $scene, $nbr
    } else {
        $sortie | Select-String -CaseSensitive -Pattern 'conforme|DIFFÉRENTE|pas de référence' | ForEach-Object {
            "{0,-10} {1}" -f $scene, $_.Line.Trim()
        }
    }
    $sortie | Select-String -Pattern 'ERREUR|ASSERTION|EXCEPTION' | ForEach-Object { "{0,-10} {1}" -f $scene, $_.Line.Trim() }
    if ($code -ne 0) { $echecs++ }
}

if ($References) { "Références enregistrées dans $dossier_references" }
elseif ($echecs -eq 0) { "Banc d'essai : toutes les scènes sont conformes aux références." }
else { "Banc d'essai : $echecs scène(s) avec des différences ou des erreurs (voir $dossier_captures)." }
exit [int]($echecs -ne 0)
