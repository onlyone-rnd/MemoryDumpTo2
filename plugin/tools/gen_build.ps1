param(
  [Parameter(Mandatory=$true)][string]$Out,
  # Optional: pass a specific GUID so multiple build steps can share the same value.
  # Example:
  #   .\gen_build_year.ps1 -Out path\build.h -Guid "{01234567-89AB-CDEF-0123-456789ABCDEF}"
  [Parameter(Mandatory=$false)][string]$Guid
)

$y = (Get-Date).ToString('yyyy')
$d = (Get-Date).ToString('yyyy.MM.dd')

if ([string]::IsNullOrWhiteSpace($Guid))
{
  $g = ([guid]::NewGuid()).ToString('B').ToUpperInvariant()  # {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
}
else
{
  # Normalize to {D} format with braces.
  $g = ([guid]::Parse($Guid)).ToString('B').ToUpperInvariant()
}

$c = @(
  '#pragma once'
  ''
  '/* Auto-generated. Do not edit. */'
  ('#define BUILD_YEAR_W L"{0}"' -f $y)
  ('#define BUILD_YEAR_A "{0}"' -f $y)
  ('#define BUILD_DATE_W L"{0}"' -f $d)
  ('#define BUILD_DATE_A "{0}"' -f $d)
  ('#define BUILD_GUID_W L"{0}"' -f $g)
  ('#define BUILD_GUID_A "{0}"' -f $g)
  ''
) -join "`r`n"

New-Item -ItemType Directory -Force -Path (Split-Path $Out) | Out-Null
[System.IO.File]::WriteAllText($Out, $c, [System.Text.Encoding]::ASCII)
