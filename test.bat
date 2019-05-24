@echo OFF
echo %DATE% %TIME% > out\results\%1\%2.txt

out\Release\colour.exe %1 < data\%2.txt >> out\results\%1\%2.txt

echo %DATE% %TIME% >> out\results\%1\%2.txt
