powershell write-host -fore Green Compiling...
xmake build AC
xmake build ACServer
powershell write-host -fore Green Compilation done

powershell write-host -fore Green Running projects...
wt nt -d ./ cmd /k xmake run ACServer
wt -w 1 sp -d ./ cmd /k xmake run AC
powershell write-host -fore Green Now running!
