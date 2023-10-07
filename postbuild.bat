set SolutionDir=%1
set Configuration=%2
set Platform=%3

echo 

mkdir "%SolutionDir%out\Diva Record Label"
copy %SolutionDir%\config.toml "%SolutionDir%out\Diva Record Label"
copy %SolutionDir%\DivaRecordLabel.sqlite "%SolutionDir%out\Diva Record Label"
copy %SolutionDir%%Platform%\%Configuration%\DivaRecordLabel.dll "%SolutionDir%out\Diva Record Label"
copy "%SolutionDir%out\Diva Record Label" "D:\SteamLibrary\steamapps\common\Hatsune Miku Project DIVA Mega Mix Plus\mods\Diva Record Label"