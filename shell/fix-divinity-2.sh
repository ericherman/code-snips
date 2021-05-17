#!/bin/bash

#find . -name 'SupportTool.exe'
cd ~/.steam/debian-installation/steamapps/common/Divinity\ Original\ Sin\ 2/

mv -iv ./bin ./bin.bak \
	&& ln -sv DefEd/bin bin \
	&& cd bin \
	&& mv -iv ./SupportTool.exe ./SupportTool.bak \
	&& ln -sv EoCApp.exe SupportTool.exe
