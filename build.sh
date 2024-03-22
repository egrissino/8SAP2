timestamp=`echo $(date +%d.%m.%y-%H:%M:%S)`
echo $timestamp
mkdir -p build
cmake --build ./build | tee build.log
if [[ "$?" -eq "0" ]] ; then
    echo "BUild Succeeded"
    mkdir -p logs
    ./build/8SAP.exe | tee >> ./logs/8SAP_log_${timestamp}.txt
    cp logs/8SAP_log_${timestamp}.txt ./8SAP_log.txt
fi
exit