timestamp=`echo $(date +%d.%m.%y-%H:%M:%S)`
echo $timestamp
mkdir -p build
cd build
cmake .. | tee build.log
cmake --build ./
if [[ "$?" -eq "0" ]] ; then
    echo "BUild Succeeded"
    ./8SAP.app | tee >> ./logs/8SAP_log_${timestamp}.txt
    cp logs/8SAP_log_${timestamp}.txt ./8SAP_log.txt
fi
exit 0
