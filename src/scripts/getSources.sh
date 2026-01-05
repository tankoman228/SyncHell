echo "set(SOURCES"
    find src -name "*.cpp" | sort | awk '{print "    "$0}'
echo ")"