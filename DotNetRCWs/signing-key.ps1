# Dot-source this file before running build.ps1 to enable signing:
#
#     . .\signing-key.ps1
#     .\build.ps1
#
# Leave a value blank to disable that part of signing. This file is committed
# with blank values as a template - fill it in locally and never commit real
# secrets.

# --- Authenticode code signing via Azure Key Vault -------------------------
# Binaries are signed with AzureSignTool and the .nupkg packages with
# NuGetKeyVaultSignTool. Signing runs only when ALL of CertName, ClientId,
# ClientSecret, TenantId and VaultURL are set.
$env:SigningCertName     = ''   # certificate name in the Key Vault
$env:SigningClientId     = ''   # Entra app (service principal) client id
$env:SigningClientSecret = ''   # client secret
$env:SigningTenantId     = ''   # Entra tenant id
$env:SigningVaultURL     = ''   # https://<vault-name>.vault.azure.net
$env:SigningURL          = ''   # RFC 3161 timestamp URL (blank = DigiCert)

# --- Strong-name signing ---------------------------------------------------
# Path to the OPC strong-name key (.snk) used to give the assemblies their
# official identity. Leave blank to build with the placeholder key under .\Keys.
$env:SigningKeyFile      = ''   # e.g. C:\Keys\OPC Key Pair.master (COM).snk
