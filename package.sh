#!/bin/bash
ftpHost="mafreebox.free.fr"
ftpUsername="anonymous"
ftpPassword=""
ftpDirectory="/Disque dur/Jeux/remigame"
packageName="remigame"

make
if [ $? -eq 0 ]
then
	make windows
	if [ $? -eq 0 ]
	then
		# Linux x86_64
		cd linux64 ; zip -r "../${packageName}-linux-x86_64.zip" *
		cd ..

		# Windows i686
		cd windows32 ; zip -r "../${packageName}-windows-i686.zip" *
		cd ..

		# Windows x86_64
		cd windows64 ; zip -r "../${packageName}-windows-x86_64.zip" *
		cd ..

		ftp -n ${ftpHost} <<EOF
quote USER ${ftpUsername}
quote PASS ${ftpPassword}
cd "${ftpDirectory}"
put ${packageName}-linux-x86_64.zip
put ${packageName}-windows-i686.zip
put ${packageName}-windows-x86_64.zip
ls
exit
EOF
		rm -f ${packageName}-linux-x86_64.zip
		rm -f ${packageName}-windows-i686.zip
		rm -f ${packageName}-windows-x86_64.zip
	fi
fi


