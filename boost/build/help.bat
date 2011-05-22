@boost-dir.bat
@pushd
@cd "%LE_BJAMDIR%"
bjam --help
REM to be used in --with-##### or --without-#####
@popd
@PAUSE