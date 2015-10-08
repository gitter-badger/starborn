@echo off

if %1. == . cd ..

bin\bundler.exe a bin\assets.zip assets -i 100 -r -u all -d zpaq
bin\sha1.exe bin\assets.zip patch\assets.zip.sha1
bin\base91.exe -e bin\assets.zip patch\assets.zip.b91
