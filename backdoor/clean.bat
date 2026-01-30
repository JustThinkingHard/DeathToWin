@echo off
echo [*] Arret du processus malveillant...
taskkill /F /IM update.exe >nul 2>&1
taskkill /F /IM OneDriveUpdate.exe >nul 2>&1

echo [*] Suppression de la persistance (Dossier Demarrage)...
del "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup\OneDriveUpdate.exe" /F /Q

echo [*] Suppression du virus dans Temp...
del "%TEMP%\update.exe" /F /Q

echo [*] Suppression du raccourci piege sur le Bureau...
del "%USERPROFILE%\Desktop\Facture.pdf.lnk" /F /Q
del "%USERPROFILE%\Desktop\Facture.pdf" /F /Q

echo [+] Nettoyage termine.
pause